#!/usr/local/bin/python3
#   Seed the database.

import random
import sqlite3
import sys
import json
from PIL import Image

################################################################################
#   Elevation processing
################################################################################

land = \
    dict(
        sea = ( 132, 176, 200 ),
        coast = ( 155, 153, 43 ),
        lowland = ( 151, 211, 92 ),
        highland = ( 99, 134, 64 ),
        mountain = ( 180, 180, 179 )
    )

land_code = dict( sea = 0, coast = 1, lowland = 2, highland = 3, mountain = 4 )

def find_nearest_match( color ):
    ( r0, g0, b0 ) = color
    sofar = None
    best = float( 'inf' )
    for ( k, ( r1, g1, b1 ) ) in land.items():
        d = ( r0 - r1 ) ** 2 + ( g0 - g1 ) ** 2 + ( b0 - b1 ) ** 2
        if d < best:
            sofar = k
            best = d
    return sofar

def pixelToLandCode( i, j, img ):
    ( r, g, b, a ) = img.getpixel( ( i, j ) )
    land_type = find_nearest_match( ( r, g, b ) )
    return land_code[ land_type ]

def convert( img ):
    ( w, h ) = img.size
    for i in range( 0, w ):
        for j in range( 0, h ):
            code = pixelToLandCode( i, j, img )
            yield( ( i, j ), code )

################################################################################
#   JSON Processing
################################################################################

def getSeedInfo( json_name ):
    with open( json_name ) as f:
        return json.load( f )

def addFactions( db, factions_info ):
    for f in factions_info:
        db.execute( 'insert into faction (id, title, color) values(?,?,?)', ( f['id'], f['title'], f['color'] ) )

def addElevation( db, image ):
    worldq = db.cursor()
    count = 0
    for ( ( x, y ), land_code ) in convert( image ):
        worldq.execute( "UPDATE HEX SET elevation = ? WHERE X = ? AND Y = ?", ( land_code, x, y ) ) 
        count += 1
        if ( count & 0xFFFF ) == 0:
            print( count )

def addBoard( db, size, board_info, img ):
    ( w, h ) = size
    db.execute( 'insert into world values(?,?)', ( w, h ) )
    for i in range( 0, w ):
        for j in range( 0, h ):
            db.execute( 'insert into hex ( x, y ) values (?,?)', ( i, j ) )
    if img:
        addElevation( db, img )

def pick( size, img ):
    ( w, h ) = size
    for i in range( 0, 16 ):
        i = random.randrange( 0, w )
        j = random.randrange( 0, h )
        code = pixelToLandCode( i, j, img )
        if code:
            # Code 0 = sea & we don't place units in the sea.
            return ( i, j )
    return None

def addUnits( db, size, count, factions, num_goals, img ):
    for n in range( 0, count ):
        position = pick( size, img )
        if position:
            ( x, y ) = position
            faction_id = random.choice( factions )
            goal_id = random.randrange( 0, num_goals ) + 1
            db.execute( 'insert into team ( x, y, faction_id, goal_id ) values (?,?,?,?)', ( x, y, faction_id, goal_id ) )
        
def addGoal( db, commands ):
    for row in commands:
        title = row[ 'title' ]
        code = row[ 'code' ]
        x = row[ 'x' ] if 'x' in row else 0
        y = row[ 'y' ] if 'y' in row else 0
        db.execute( 'insert into goal ( title, code, x, y ) values (?,?,?,?)', ( title, code, x, y ) )

def main( db_name, json_name ):
    seed_info = getSeedInfo( json_name )
    board_info = seed_info[ 'board' ]
    size = ( board_info[ 'width' ], board_info[ 'height' ] )
    img = None
    if 'elevation' in board_info:
        img = Image.open( board_info[ 'elevation' ] )

    with sqlite3.connect( db_name ) as db:
        addFactions( db, seed_info[ 'factions' ] )
        addGoal( db, seed_info[ 'goals' ] )
        addBoard( db, size, board_info, img )
        addUnits( 
            db, 
            size, 
            seed_info[ 'units' ], 
            [ f['id'] for f in seed_info[ 'factions' ] ], 
            len( seed_info[ 'goals' ] ),
            img
        )

if __name__ == "__main__":
    main( sys.argv[ 1 ], sys.argv[ 2 ] )
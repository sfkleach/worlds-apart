#!/usr/local/bin/python3
#	Seed the database.

import random
import sqlite3
import sys
import json
from PIL import Image

################################################################################
#   Elevation processing
################################################################################

land = dict(
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

def convert( img_path ):
	img = Image.open( img_path )
	( w, h ) = img.size
	for i in range( 0, w ):
		for j in range( 0, h ):
			( r, g, b, a ) = img.getpixel( ( i, j ) )
			land_type = find_nearest_match( ( r, g, b ) )
			code = land_code[ land_type ]
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


def addBoard( db, size, board_info ):
	( w, h ) = size
	db.execute( 'insert into world values(?,?)', ( w, h ) )
	for i in range( 0, w ):
		for j in range( 0, h ):
			db.execute( 'insert into hex ( x, y ) values (?,?)', ( i, j ) )
	if 'elevation' in board_info:
		addElevation( db, board_info[ 'elevation' ] )

def addUnits( db, size, count, factions, goals ):
	( w, h ) = size
	for n in range( 0, count ):
		width = random.randrange( 0, w )
		height = random.randrange( 0, h )
		faction_id = random.choice( factions )
		goal_id = random.choice( goals )
		db.execute( 'insert into team ( x, y, faction_id, goal_id ) values (?,?,?,?)', ( width, height, faction_id, goal_id ) )
		
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
	with sqlite3.connect( db_name ) as db:
		addFactions( db, seed_info[ 'factions' ] )
		addGoal( db, seed_info[ 'goals' ] )
		addBoard( db, size, board_info )
		addUnits( db, size, seed_info[ 'units' ], [ f['id'] for f in seed_info[ 'factions' ] ], [ g['code'] for g in seed_info[ 'goals' ] ] )

if __name__ == "__main__":
	main( sys.argv[ 1 ], sys.argv[ 2 ] )
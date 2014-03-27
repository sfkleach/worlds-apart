#!/usr/local/bin/python3

from PIL import Image
import sqlite3
import sys
from contextlib import closing

def trail( db ):
	with closing( db.cursor() ) as worldq:
		worldq.execute( 'select x, y, mark, elevation from hex' )
		for row in worldq:
			x = row[0]
			y = row[1]
			m = row[2]
			t = row[3]
			yield( ( x, y ), m, t )

def units( db ):
	with closing( db.cursor() ) as worldq:
		worldq.execute( 'select team.x, team.y, faction.color from team join faction on team.faction_id = faction.id' )
		for row in worldq:
			x = row[0]
			y = row[1]
			c = row[2]
			yield ( ( x, y ), c )


def worldSize( db ):
	with closing( db.cursor() ) as worldq:
		worldq.execute( 'select width, height from world' );
		row = worldq.fetchone()
		return ( row[0], row[1] )

RED = ( 255, 0, 0 )
GREEN = ( 0, 255, 0 )
BLUE = ( 0, 0, 255 )
YELLOWISH = ( 0, 48, 64 )

def colorTuple( color ):
	if color == "red":
		return RED
	elif color == "green":
		return GREEN
	elif color == "blue":
		return BLUE
	else:
		return YELLOWISH

def unitsImage( db_name ):
	with sqlite3.connect( db_name ) as db:
		img = Image.new( "RGB", worldSize( db ) )
		for ( coord, m, t ) in trail( db ):
			img.putpixel( coord, YELLOWISH if m else ( 40 * t, 40 * t, 40 * t ) )
		for ( coord, color ) in units( db ):
			img.putpixel( coord, colorTuple( color ) )
		return img

if __name__ == "__main__":
	unitsImage( sys.argv[ 1 ] ).save( sys.argv[ 2 ] )

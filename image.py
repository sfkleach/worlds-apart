#!/usr/local/bin/python3

from PIL import Image, ImageDraw
import sqlite3
import sys

DIM = ( 1024, 1024 )

def trail( db_name ):
	with sqlite3.connect( db_name ) as world_db:
		worldq = world_db.cursor()
		worldq.execute( 'select x, y, mark, land from hex' )
		for row in worldq:
			x = row[0]
			y = row[1]
			m = row[2]
			t = row[3]
			yield( ( x, y ), m, t )

def units( db_name ):
	with sqlite3.connect( db_name ) as world_db:
		worldq = world_db.cursor()
		worldq.execute( 'select team.x, team.y from team' )
		for row in worldq:
			x = row[0]
			y = row[1]
			yield( x, y )

def unitsImage( db_name ):
	img = Image.new( "RGB", DIM )
	for ( coord, m, t ) in trail( db_name ):
		img.putpixel( coord, ( 255, 0, 0 ) if m else ( 40 * t, 40 * t, 40 * t ) )
	for coord in units( db_name ):
		img.putpixel( coord, ( 255, 255, 255 ) )
	return img

if __name__ == "__main__":
	unitsImage( sys.argv[ 1 ] ).save( sys.argv[ 2 ] )

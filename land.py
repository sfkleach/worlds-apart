#!/usr/local/bin/python3

from PIL import Image
import sqlite3
import sys

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
	for i in range( 0, 1024 ):
		for j in range( 0, 1024 ):
			( r, g, b, a ) = img.getpixel( ( i, j ) )
			land_type = find_nearest_match( ( r, g, b ) )
			code = land_code[ land_type ]
			yield( ( i, j ), code )

def run( image, db_name ):
	with sqlite3.connect( db_name ) as world_db:
		worldq = world_db.cursor()
		count = 0
		for ( ( x, y ), land_code ) in convert( image ):
			worldq.execute( "UPDATE HEX SET LAND = ? WHERE X = ? AND Y = ?", ( land_code, x, y ) )	
			count += 1
			if ( count & 0xFFFF ) == 0:
				print( count )


if __name__ == "__main__":
	run( sys.argv[ 1 ], sys.argv[ 2 ] )

		

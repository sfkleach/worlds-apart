#ifndef COMMON_HPP
#define COMMON_HPP

#include <ostream>
#include <math.h> 

#include "maybe.hpp"

struct Move {
	Move( int dx, int dy ) : dx( dx ), dy( dy ) {}
	Move() : dx( 0 ), dy( 0 ) {}
	short dx;
	short dy;
};

struct Coord {
	Coord( int x, int y ) : x( x ), y( y ) {}
	short x;
	short y;

	Move moveTo( const Coord & dst ) {
		return Move( dst.x - this->x, dst.y - this->y );
	}

	Coord shift( const Move & move ) const {
		return Coord( this->x + move.dx, this->y + move.dy );
	}

	static double sqr( double d ) { return d * d; }

	double distanceTo( const Coord & dst ) const {
		return sqrt( sqr( dst.x - this->x ) + sqr( dst.y - this->y ) );
	}
};



std::ostream & operator <<( std::ostream & str, const Coord & coord );
std::ostream & operator <<( std::ostream & str, const Move & move );

#endif
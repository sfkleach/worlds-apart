#ifndef LINK_HPP
#define LINK_HPP

class Hex;

struct Link {

	Hex *	source;
	Hex * 	destination;
	double	delta;

	Link( Hex * src, double time, Hex * dst ) :
		source( src ),
		destination( dst ),
		delta( time )
	{}

	Link() :
		source( nullptr ),
		destination( nullptr ),
		delta( 0.0 )
	{}

	bool operator <( const Link & rhs ) const;
};


#endif
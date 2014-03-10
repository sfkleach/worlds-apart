#ifndef HEX_HPP
#define HEX_HPP


#include "sqlite3lib.hpp"
#include "common.hpp"
#include "embedded.hpp"

class Hex : public Embedded {
private:
	Coord location;
	int elevation;
	class Unit * occupier;
	int mark_value;

public:
	Hex( Coord location, int elevation ) :
		location( location ),
		elevation( elevation ),
		occupier( nullptr ),
		mark_value( 0 )
	{}

public:
	int getX() const {
		return this->location.x;
	}

	int getY() const {
		return this->location.y;
	}

	bool isOccupied() const {
		return this->occupier != nullptr;
	}

	void setLand( const int elevation ) {
		this->elevation = elevation;
	}

public:
	void mark() {
		this->mark_value = 1;
	}

public:
	void attach( class Unit * unit ) {
		this->occupier = unit;	
	}

	void unattach( class Unit * unit ) {
		this->occupier = nullptr;
	}

public:
	void dump( StatementCache & db );

};

#endif
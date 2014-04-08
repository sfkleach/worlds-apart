#ifndef HEX_HPP
#define HEX_HPP

#include <vector>
#include <memory>

#include "sqlite3lib.hpp"
#include "common.hpp"
#include "embedded.hpp"

#include "link.hpp"

class Hex : public Embedded {
private:
	Coord location;
	int elevation;
	class Unit * occupier;
	int mark_value;
	std::unique_ptr< std::vector< Link > > neighbors;

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

	Coord getLocation() {
		return this->location;
	}

	Move moveTo( Hex * there ) {
		return there->location.moveTo( this->location );
	}

	bool isOccupied() const {
		return this->occupier != nullptr;
	}

	void setLand( const int elevation ) {
		this->elevation = elevation;
	}

	double getSpeed() const;

	bool isLocked() const;

	std::vector< Link > & findNeighbors();

	double getVolume() const { return 5.0; }

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
	Hex * tryFindHex( const Coord & loc ) const;
	Hex * tryFindHex( const Move & move ) const;
	void dump( StatementCache & db );

};

#endif
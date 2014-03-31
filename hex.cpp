#include <iostream>

#include "sqlite3lib.hpp"
#include "hex.hpp"
#include "world.hpp"

using namespace std;

void Hex::dump( StatementCache & db ) {
	Statement & insert = db.get( "INSERT INTO HEX VALUES(?,?,?,?,0,0)" );
	insert.reset();
	insert.bind( 1, this->location.x );
	insert.bind( 2, this->location.y );
	insert.bind( 3, this->mark_value );
	insert.bind( 4, this->elevation );

	insert.step();
}

double Hex::getSpeed() const {
	switch ( this->elevation ) {
		case 1: return 6.0;
		case 2: return 4.0;
		case 3: return 1.0;
		case 4: return 0.5;
		default: return 0.0;
	}
}

bool Hex::isLocked() const {
	return this->elevation < 1;
}

void Hex::findNeighbors( std::vector< Link > & links ) {
	for ( int dx = -1; dx < 2; dx++ ) {
		for ( int dy = -1; dy < 2; dy++ ) {
			if ( dx == 0 && dy == 0 ) continue;
			Hex * next = this->world->tryFind( this->getX() + dx, this->getY() + dy );
			if ( next != nullptr ) {
				const double d = sqrt( dx * dx + dy * dy );
				const double s1 = next->getSpeed();
				const double s2 = this->getSpeed();
				if ( s1 > 0.0 && s2 > 0.0 ) {
					const double av_spd = 0.5 * ( s1 + s2 );
					const double dt = d / av_spd;
					links.emplace_back( this, dt, next );
				}
			}
		}
	}
}

Hex * Hex::tryFindHex( const Coord & loc ) const {
	return this->world->tryFind( loc );
}

Hex * Hex::tryFindHex( const Move & move ) const {
	return this->world->tryFind( this->location.shift( move ) );
}


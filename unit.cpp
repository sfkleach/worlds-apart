#include <utility>

#include "world.hpp"
#include "unit.hpp"

using namespace std;

void Unit::move( Hex * new_hex ) {
	if ( this->location != nullptr ) {
		this->location->unattach( this );
	}
	new_hex->attach( this );
	this->location = new_hex;
}

std::shared_ptr< Goal > Unit::getGoal() { 
	return this->goal; 
}

void Unit::setGoal( std::shared_ptr< Goal > & g ) {
	this->goal = g;
}

static pair< int, int > pick( const int d ) {
	switch ( d ) {
		case 0: return pair< int, int >( 1, 0 );
		case 1: return pair< int, int >( 0, 1 );
		case 2: return pair< int, int >( -1, 0 );
		case 3: return pair< int, int >( 0, -1 );
	}
	return pair< int, int >( 0, 0 );
}

void Unit::stay() {
	//	No action needed!
}

void Unit::tryMove( pair< int, int > dxdy ) {
	if ( dxdy.first == 0 and dxdy.second == 0 ) return;
	Hex * hex = (
		this->world->tryFind( 
			this->location->getX() + dxdy.first,
			this->location->getY() + dxdy.second
		)
	);
	if ( hex == nullptr ) {
		//	No way through.
	} else if ( hex->isOccupied() ) {
		//	Blocked.
	} else {
		//cout << "Dirn (" << i << "," << j << ") : " << "OK." << endl;
		this->move( hex );
		this->location->mark();
		return;
	}
}

void Unit::jiggle() {
	this->tryMove( pick( world->roll( 4 ) ) );
}

void Unit::goTo( const int x, const int y ) {
	const int dx = x - this->location->getX();
	const int dy = y - this->location->getY();

	if ( dx == 0 && dy == 0 ) return;

	const int adx = abs( dx );
	const int ady = abs( dy );
	const double ratio = double( adx ) / ( double( adx ) + double( ady ) );
 
	const double d = this->world->roll();
	if ( d <= ratio ) {
		this->tryMove( pair< int, int >( dx < 0 ? -1 : dx > 0 ? 1 : 0, 0 ) );
	} else {
		this->tryMove( pair< int, int >( 0, dy < 0 ? -1 : dy > 0 ? 1 : 0 ) );
	}
}

void Unit::action() {
	if ( !! this->goal ) {
		this->goal->activity( this );
	}
}
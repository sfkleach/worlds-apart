#include <utility>


#include "team.hpp"
#include "world.hpp"

using namespace std;

static pair< int, int > pick( const int d ) {
	switch ( d ) {
		case 0: return pair< int, int >( 1, 0 );
		case 1: return pair< int, int >( 0, 1 );
		case 2: return pair< int, int >( -1, 0 );
		case 3: return pair< int, int >( 0, -1 );
	}
	return pair< int, int >( 0, 0 );
}

void Team::action() {
	pair< int, int > dxdy = pick( world->roll( 4 ) );
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

void Team::dump( StatementCache & db ) {
	Statement & insert = db.get( "INSERT INTO TEAM VALUES(?,?,?,NULL)" );
	insert.reset();
	insert.bind( 1, this->location->getX() );
	insert.bind( 2, this->location->getY() );
	insert.bind( 3, this->faction );
	insert.step();
}

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

static Move pick( const int d ) {
	switch ( d ) {
		case 0: return Move( 1, 0 );
		case 1: return Move( 0, 1 );
		case 2: return Move( -1, 0 );
		case 3: return Move( 0, -1 );
		case 4: return Move( 1, 1 );
		case 5: return Move( 1, -1 );
		case 6: return Move( -1, 1 );
		case 7: return Move( -1, -1 );
	}
	return Move( 0, 0 );
}

void Unit::stay( Goal & goal ) {
	//	No action needed!
}

auto Unit::tryMove( const Move & move ) -> MoveStatus {
	if ( this->location->isLocked() ) return MoveStatus::INACCESSIBLE;
	if ( move.dx == 0 and move.dy == 0 ) return MoveStatus::OK;
	Hex * hex = this->world->tryFind( this->location->getLocation().shift( move ) );
	if ( hex == nullptr || hex->isLocked() ) {
		//	No way through.
		return MoveStatus::INACCESSIBLE;
	} else if ( hex->isOccupied() ) {
		//	Blocked.
		return MoveStatus::BLOCKED;
	} else {
		this->move( hex );
		this->location->mark();
		return MoveStatus::OK;
	}
}

void Unit::jiggle( Goal & goal ) {
	this->tryMove( pick( world->roll( 8 ) ) );
}

void Unit::goTo( Goal & goal ) {
	Maybe< Move > m = goal.moveTowardsGoal( this->location );
	if ( m.hasValue() ) {
		MoveStatus status = this->tryMove( m.fastValue() );
		if ( status == MoveStatus::BLOCKED ) {
			this->jiggle( goal );
		}
	}

}

void Unit::action() {
	if ( !! this->goal ) {
		this->goal->activity( this );
	}
}
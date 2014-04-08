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

auto Unit::couldMoveTo( Hex * hex ) const -> MoveStatus {
	if ( hex == nullptr || hex->isLocked() ) {
		//	No way through.
		return MoveStatus::INACCESSIBLE;
	} else if ( hex->isOccupied() ) {
		//	Blocked.
		return MoveStatus::BLOCKED;
	} else {
		return MoveStatus::OK;
	}	
}

auto Unit::isAllowedToOccupy( Hex * hex ) const -> bool {
	return this->couldMoveTo( hex ) == MoveStatus::OK;
}

auto Unit::couldMove( const Move & move ) const -> MoveStatus {
	Hex * hex = this->location->tryFindHex( move );	
	return this->couldMoveTo( hex );
}

auto Unit::tryMove( const Move & move ) -> MoveStatus {
	Hex * hex = this->location->tryFindHex( move );	
	return this->tryMove( hex );
}

auto Unit::tryMove( Hex * hex ) -> MoveStatus {
	if ( this->location->isLocked() ) return MoveStatus::INACCESSIBLE;
	MoveStatus status = this->couldMoveTo( hex );
	if ( status == MoveStatus::OK ) {
		this->move( hex );
		this->location->mark();		
	}
	return status;
}

void Unit::jiggle( Goal & goal ) {
	this->tryMove( pick( world->roll( 8 ) ) );
}

bool Unit::isInMiniTrail( Hex * hex ) const {
	return find( this->mini_trail.begin(), this->mini_trail.end(), hex ) != this->mini_trail.end();
}

void Unit::replan( Goal & _goal ) {
	Hex * sofar = nullptr;
	double delta_sofar = INFINITY;
	for ( auto link : this->location->findNeighbors() ) {
		if ( this->isAllowedToOccupy( link.destination ) ) {
			double penalty = this->isInMiniTrail( link.destination ) ? 1.0 : 0.0;
			if ( link.delta < delta_sofar ) {
				sofar = link.destination;
				delta_sofar = link.delta;
			}
		}
	}
	if ( sofar != nullptr ) {
		this->tryMove( sofar );
	}
}

void Unit::extendMiniTrail() {
	this->mini_trail.push_back( this->location );
	if ( this->mini_trail.size() > 4 ) {
		this->mini_trail.pop_front();
	}
}

void Unit::goTo( Goal & goal ) {
	//this->extendMiniTrail();
	Maybe< Move > m = goal.moveTowardsGoal( this->location );
	if ( m.hasValue() ) {
		MoveStatus status = this->tryMove( m.fastValue() );
		/*if ( status == MoveStatus::BLOCKED ) {
			this->replan( goal );
		}*/
	}
}

void Unit::action() {
	if ( !! this->goal ) {
		this->goal->activity( this );
	}
}
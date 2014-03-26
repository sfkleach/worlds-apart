#include "unit.hpp"

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
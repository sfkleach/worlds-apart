#include <cassert>

#include "goal.hpp"
#include "unit.hpp"
#include "mishap.hpp"

void Goal::dump( StatementCache & db ) {
	Statement & insert = db.get( "INSERT INTO GOAL (id,title,code,x,y) VALUES(?,?,?,?,?)" );
	insert.reset();
	insert.bind( 1, db.idAllocator().objectID( this ) );
	insert.bind( 2, this->title );
	insert.bind( 3, int( this->code ) );
	insert.bind( 4, this->x );
	insert.bind( 5, this->y );
	insert.step();
}

void Goal::activity( Unit * unit ) {
	assert( unit != nullptr );
	switch ( this->code ) {
		case GoalType::Stay:
			unit->stay();
			return;
		case GoalType::Jiggle:
			unit->jiggle();
			return;
		case GoalType::GoTo:
			unit->goTo( this->x, this->y );
			return;
	}
	throw Mishap( "Internal error" );
}

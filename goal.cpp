#include <cassert>
#include <cmath>
#include <iostream>

#include "goal.hpp"
#include "unit.hpp"
#include "mishap.hpp"
#include "world.hpp"

using namespace std;

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
			unit->stay( *this );
			return;
		case GoalType::Jiggle:
			unit->jiggle( *this );
			return;
		case GoalType::GoTo:
			unit->goTo( *this );
			return;
	}
	throw Mishap( "Internal error" );
}

void Goal::instantiateFindRoute( Hex * here ) {
	if ( this->code != GoalType::GoTo ) return;
	Hex * goal = here->tryFindHex( Coord( this->x, this->y ) );
	if ( goal == nullptr ) return;
	this->find_route.reset( new FindRoute() );
	//cout << "Attempting to generate travel time matrix .... " << endl;
	this->find_route->findRoute( goal );	
	//cout << ".... done!" << endl;
}

Maybe< Move > Goal::moveTowardsGoal( Hex * here ) {
	if ( not this->find_route ) {
		this->instantiateFindRoute( here );
		if ( not this->find_route ) return Maybe< Move >();
	}
	return this->find_route->moveToPrevious( here );
}

double Goal::predictedTimeFrom( Hex * here ) {
	if ( not this->find_route ) {
		this->instantiateFindRoute( here );
		if ( not this->find_route ) return INFINITY;
	}
	return this->find_route->predictedTimeFrom( here );
}



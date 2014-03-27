#include <utility>
#include <cassert>

#include "team.hpp"
#include "world.hpp"

using namespace std;

void Team::dump( StatementCache & db ) {
	Statement & insert = db.get( "INSERT INTO TEAM VALUES(?,?,?,NULL)" );
	insert.reset();
	insert.bind( 1, this->location->getX() );
	insert.bind( 2, this->location->getY() );
	insert.bind( 3, this->faction );
	insert.step();
}

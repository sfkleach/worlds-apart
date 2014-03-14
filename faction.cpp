#include "faction.hpp"

void Faction::dump( StatementCache & db ) {
	Statement & insert = db.get( "INSERT INTO FACTION VALUES(?,?,?)" );
	insert.reset();
	insert.bind( 1, this->id );
	insert.bind( 2, this->name );
	insert.bind( 3, this->color );
	insert.step();
}
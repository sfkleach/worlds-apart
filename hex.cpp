#include "sqlite3lib.hpp"
#include "hex.hpp"

void Hex::dump( StatementCache & db ) {
	Statement & insert = db.get( "INSERT INTO HEX VALUES(?,?,?,?,0,0)" );
	insert.reset();
	insert.bind( 1, this->location.x );
	insert.bind( 2, this->location.y );
	insert.bind( 3, this->mark_value );
	insert.bind( 4, this->elevation );

	insert.step();
}

#ifndef FACTION_HPP
#define FACTION_HPP

#include <string>

#include "sqlite3lib.hpp"
#include "common.hpp"
#include "embedded.hpp"



class Faction {
private:
	int id;
	std::string name;
	std::string color;

public:
	Faction( int id, const std::string & name, const std::string & color ) :
		id( id ),
		name( name ),
		color( color )
	{}

public:
	void dump( StatementCache & db );

};

#endif
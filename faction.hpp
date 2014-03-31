#ifndef FACTION_HPP
#define FACTION_HPP

#include <string>

#include "sqlite3lib.hpp"
#include "common.hpp"
#include "embedded.hpp"


class Faction {
private:
	int id;
	std::string title;
	std::string color;

public:
	Faction( int id, const std::string & title, const std::string & color ) :
		id( id ),
		title( title ),
		color( color )
	{}

public:
	std::string getTitle() {
		return this->title;
	}

public:
	void dump( StatementCache & db );

};

#endif
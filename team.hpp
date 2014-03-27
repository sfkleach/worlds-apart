#ifndef TEAM_HPP
#define TEAM_HPP

#include "sqlite3lib.hpp"
#include "unit.hpp"

class Team : public Unit {
public:
	Team( Hex * location, int faction ) :
		Unit( location, faction )
	{}
	virtual ~Team() {}

public:
	double refractoryPeriod() { return 1.0; }

public:
	void dump( StatementCache & db );

};

#endif
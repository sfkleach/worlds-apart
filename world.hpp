#ifndef WORLD_HPP
#define WORLD_HPP

#include <random>
#include <vector>
#include <map>
#include <deque>
#include <memory>

#include "sqlite3lib.hpp"

class Faction;
class Hex;
class Unit;
class Goal;

class World {
private:
	std::default_random_engine generator;
	std::uniform_real_distribution< double > rdistribution;
	
private:
	int width;
	int height;
	std::vector< std::vector< Hex * > > hexes;
	std::deque< Unit * > units;
public:
	std::map< int, Faction * > factions;

public:
	World() :
		width( 0 ),
		height( 0 )
	{}

	~World() {
		this->clear();
	}

public:
	Hex * tryFind( const int x, const int y );
	int roll( int n );
	double roll();
	void initSize( const int width, const int height );
	void clear();

private:
	void depopulateHexRow( std::vector< Hex * > & row );
	void depopulateHexes();
	void depopulateUnits();
	void deleteFactions();
	void deleteGoals();

public:
	void forkDump( const char * db );
	void dump( const char * db );

public:
	void restore( const char * db_path_name );
private:
	typedef std::map< int, std::shared_ptr< Goal > > GoalMap;

	void restoreFactions( Sqlite3 & db );
	void restoreSize( Sqlite3 & db );
	void restoreHexes( Sqlite3 & db );
	void restoreUnits( Sqlite3 & db );
	void restoreGoals( Sqlite3 & db, GoalMap & goals );

public:
	void runOneUnit();

};

#endif
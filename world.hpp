#ifndef WORLD_HPP
#define WORLD_HPP

#include <random>
#include <vector>
#include <deque>

#include "sqlite3lib.hpp"

class Faction;
class Hex;
class Unit;

class World {
private:
	std::default_random_engine generator;
	std::uniform_real_distribution< double > rdistribution;
	
private:
	int width;
	int height;
	std::vector< std::vector< Hex * > > hexes;
	std::deque< Unit * > units;
	std::vector< Faction * > factions;

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
	void init( const int width, const int height );
	void clear();

private:
	void populateHexes( const int width, const int height );
	void depopulateHexRow( std::vector< Hex * > & row );
	void depopulateHexes();
	void populateUnits( const int width, const int height, const int n_units );
	void depopulateUnits();

public:
	void populate( const int width, const int height, const int n_units );

public:
	void forkDump( const char * db );
	void dump( const char * db );

public:
	void restore( const char * db_path_name );
private:
	void restoreFactions( Sqlite3 & db );
	void restoreSize( Sqlite3 & db );
	void restoreHexes( Sqlite3 & db );
	void restoreUnits( Sqlite3 & db );

public:
	void runOneUnit();

};

#endif
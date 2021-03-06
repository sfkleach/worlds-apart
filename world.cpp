#include <iostream>
#include <map>
#include <set>
#include <memory>
#include <unistd.h>
#include <deque>
#include <vector>
#include <queue>
#include <utility>

#include "common.hpp"
#include "world.hpp"
#include "hex.hpp"
#include "team.hpp"
#include "faction.hpp"
#include "goal.hpp"
#include "link.hpp"
#include "mishap.hpp"
#include "findroute.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//	Utilities.
////////////////////////////////////////////////////////////////////////////////

Hex * World::tryFind( const int x, const int y ) {
	if ( 0 <= x && x < this->width && 0 <= y && y < this->height ) {
		return this->hexes.at( x ).at( y );
	} else {
		return nullptr;
	}
}

Hex * World::tryFind( const Coord & xy ) {
	if ( 0 <= xy.x && xy.x < this->width && 0 <= xy.y && xy.y < this->height ) {
		return this->hexes.at( xy.x ).at( xy.y );
	} else {
		return nullptr;
	}
}

int World::roll( int n ) {
	std::uniform_int_distribution< int > distribution( 0, n );
	return distribution( this->generator );
}

double World::roll() {
	return this->rdistribution( this->generator );
}


////////////////////////////////////////////////////////////////////////////////
//	Simulate.
////////////////////////////////////////////////////////////////////////////////

void World::runOneUnit() {
	Unit * top_unit = this->units.front();
	this->units.pop_front();
	top_unit->action();
	this->units.push_back( top_unit );
}

////////////////////////////////////////////////////////////////////////////////
//	Teardown existing structures.
////////////////////////////////////////////////////////////////////////////////

void World::depopulateUnits() {
	for ( auto it = this->units.begin(); it != this->units.end(); ++it ) {
		delete( *it );
	}
}

void World::depopulateHexRow( vector< Hex * > & row ) {
	for ( auto it = row.begin(); it != row.end(); ++it ) {
		delete( *it );
	}
}

void World::depopulateHexes() {
	for ( auto it = this->hexes.begin(); it != this->hexes.end(); ++it ) {
		depopulateHexRow( *it );
	}
	this->hexes.clear();
}

void World::deleteFactions() {
	for ( auto it = this->factions.begin(); it != this->factions.end(); ++it ) {
		delete it->second;
	}
	this->factions.clear();
}


void World::clear() {
	this->width = 0;
	this->height = 0;
	this->depopulateHexes();
	this->depopulateUnits();
	this->deleteFactions();
}


////////////////////////////////////////////////////////////////////////////////
//	Dump to a database.
////////////////////////////////////////////////////////////////////////////////



void World::dump( const char * db_path_name ) {
	Sqlite3 db( db_path_name );
	StatementCache cache( db );
	IdAllocator & ids = cache.idAllocator();
	db.exec( "BEGIN TRANSACTION" );
	for ( auto it : this->factions ) {
		it.second->dump( cache );
	}
	{
		Statement s( db, "INSERT INTO WORLD VALUES(?,?)" );
		s.bind( 1, this->width );
		s.bind( 2, this->height );
		s.step();
	}
	for ( auto it : this->hexes ) {
		for ( auto jt : it ) {
			jt->dump( cache );
		}
	}
	for ( auto it: this->units ) {
		std::shared_ptr< Goal > goal = it->getGoal();
		if ( not ids.hasObjectID( goal.get() ) ) {
			ids.objectID( goal.get() );
			goal->dump( cache );
		}
	}
	for ( auto it : this->units ) {
		it->dump( cache );
	}
	db.exec( "END TRANSACTION" );
}

void World::forkDump( const char * db ) {
	pid_t pid = fork();
	if ( pid == 0 ) {
		//	In the child.
		this->dump( db );
		cout << "Dumped" << endl;
		exit( EXIT_SUCCESS );
	}
}


////////////////////////////////////////////////////////////////////////////////
//	Restore from a database.
////////////////////////////////////////////////////////////////////////////////

void World::initSize( const int width, const int height ) {
	this->width = width;
	this->height = height;
	for ( int i = 0; i < width; i++ ) {
		hexes.emplace_back();
		vector< Hex * > & row = hexes.back();
		for ( int j = 0; j < height; j++ ) {
			row.push_back( new Hex( Coord( i, j ), 1 ) );
			row.back()->setWorld( this );
		}
	}	
}


void World::restoreSize( Sqlite3 & db ) {
	Statement dim( db, "SELECT width, height FROM WORLD" );
	dim.step();
	int w, h;
	dim.column( 0, w );
	dim.column( 1, h );	
	this->initSize( w, h );
	cout << "Init "<< w << "," << h << endl;
}

void World::restoreHexes( Sqlite3 & db ) {
	Statement hexdata( db, "SELECT x, y, elevation FROM HEX" );
	int count;
	for ( count = 0; ; count++ ) {
		const int rc = hexdata.step();
		if ( rc == SQLITE_DONE ) break;
		int x, y, elevation;
		hexdata.column( 0, x );
		hexdata.column( 1, y );
		hexdata.column( 2, elevation );
		this->tryFind( x, y )->setLand( elevation );
	}
	cout << "Read " << count << " hex records." << endl;	
}

void World::restoreUnits( Sqlite3 & db, GoalMap & goal_map ) {
	Statement teamdata( db, "SELECT x, y, faction_id, goal_id FROM TEAM" );
	int count = 0;
	for (;;) {
		const int rc = teamdata.step();
		if ( rc == SQLITE_DONE ) break;
		int x, y, faction, goal_id;
		teamdata.column( 0, x );
		teamdata.column( 1, y );
		teamdata.column( 2, faction );
		teamdata.column( 3, goal_id );
		auto goal_it = goal_map.find( goal_id );
		Hex * hex = this->tryFind( x, y );
		if ( hex == nullptr ) {
			cerr << "Cannot place team at: (" << x << "," << y << ")" << endl;
		} else {
			Team * team = new Team( hex, faction );
			this->units.push_back( team );
			if ( goal_it != goal_map.end() ) {
				team->setGoal( goal_it->second );
			}
			count += 1;
		}
	}
	cout << "Read " << count << " team records." << endl;	
}

void World::restoreFactions( Sqlite3 & db ) {
	Statement factiondata( db, "SELECT id, title, color FROM FACTION" );
	int count;
	for ( count = 0; ; count++ ) {
		const int rc = factiondata.step();
		if ( rc == SQLITE_DONE ) break;
		int id;
		string title, color;
		factiondata.column( 0, id );
		factiondata.column( 1, title );
		factiondata.column( 2, color );
		this->factions[ id ] = new Faction( id, title, color );
	}
	cout << "Read " << count << " faction records." << endl;	
}

void World::restoreGoals( Sqlite3 & db, GoalMap & goals ) {
	Statement goal_data( db, "SELECT id, title, code, x, y FROM GOAL" );
	int count;
	for ( count = 0; ; count++ ) {
		const int rc = goal_data.step();
		if ( rc == SQLITE_DONE ) break;
		int id, code, x, y;
		string title, color;
		goal_data.column( 0, id );
		goal_data.column( 1, title );
		goal_data.column( 2, code );
		goal_data.column( 3, x );
		goal_data.column( 4, y );
		goals[ id ] = make_shared< Goal >( id, title, GoalType( code ), x, y );
	}	
}

void World::restore( const char * db_path_name ) {
	Sqlite3 db( db_path_name );
	this->restoreFactions( db );
	GoalMap goals;
	this->restoreGoals( db, goals );
	this->restoreSize( db );
	this->restoreHexes( db );
	this->restoreUnits( db, goals );
}

////////////////////////////////////////////////////////////////////////////////
//	Route finding
////////////////////////////////////////////////////////////////////////////////



void World::findRoute( Hex * start, Hex * finish, deque< Link > & route ) {
	FindRoute finder;
	finder.findRoute( start, finish );
	finder.outwardsRoute( finish, route );
}
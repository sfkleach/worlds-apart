#include <iostream>
#include <map>
#include <unistd.h>

#include "common.hpp"
#include "world.hpp"
#include "hex.hpp"
#include "team.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//	Utilities.
////////////////////////////////////////////////////////////////////////////////

Hex * World::tryFind( const int x, const int y ) {
	try {
		return this->hexes.at( x % this->width ).at( y % this->height );
	} catch ( std::exception & e ) {
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
	top_unit->reschedule();
	this->units.push_back( top_unit );
}


////////////////////////////////////////////////////////////////////////////////
//	Randomly populate.
////////////////////////////////////////////////////////////////////////////////

void World::populateHexes( const int width, const int height ) {
	cout << "Populate hexes" << endl;
	this->init( width, height );
}

void World::populateUnits( const int width, const int height, const int n_units ) {
	cout << "Populate units" << endl;
	std::default_random_engine generator;
	std::uniform_int_distribution< int > idistribution;
	auto irandom = std::bind( idistribution, generator );
	std::uniform_real_distribution< double > rdistribution;
	auto r = std::bind( rdistribution, generator );
	for ( int i = 0; i < n_units; i++ ) {
		for ( int count = 0; true; count++ ) {
			Hex * hex = this->tryFind( irandom( 0, width ) , irandom( 0, height ) );
			if ( hex == nullptr or hex->isOccupied() ) {
				//cout << "Conflict at (" << location.getX() << "," << location.getY() << ")" << endl;
				continue;	//	 Re-roll.
			}
			units.push_back( new Team( hex, irandom( 0, 6 ), r() ) );
			break;
		}
	}
	cout << "Populated" << endl;
}

void World::depopulateUnits() {
	for ( auto it = this->units.begin(); it != this->units.end(); ++it ) {
		delete( *it );
	}
}

void World::populate( const int width, const int height, const int n_units ) {
	this->populateHexes( width, height );
	this->populateUnits( width, height, n_units );
}

////////////////////////////////////////////////////////////////////////////////
//	Teardown existing structures.
////////////////////////////////////////////////////////////////////////////////


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

void World::clear() {
	this->width = 0;
	this->height = 0;
	this->depopulateHexes();
	this->depopulateUnits();
}


////////////////////////////////////////////////////////////////////////////////
//	Dump to a database.
////////////////////////////////////////////////////////////////////////////////

void World::dump( const char * db_path_name ) {
	Sqlite3 db( db_path_name );
	StatementCache cache( db );
	db.exec( "BEGIN TRANSACTION" );
	{
		Statement s( db, "INSERT INTO WORLD VALUES(?,?)" );
		s.bind( 1, this->width );
		s.bind( 2, this->height );
		s.step();
	}
	for ( auto it = this->hexes.begin(); it != this->hexes.end(); ++it ) {
		for ( auto jt = it->begin(); jt != it->end(); ++jt ) {
			(*jt)->dump( cache );
		}
	}
	for ( auto it = this->units.begin(); it != this->units.end(); ++it ) {
		(*it)->dump( cache );
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

void World::init( const int width, const int height ) {
	this->clear();
	this->width = width;
	this->height = height;
	for ( int i = 0; i < width; i++ ) {
		hexes.emplace_back();
		vector< Hex * > & row = hexes.back();
		for ( int j = 0; j < height; j++ ) {
			row.push_back( new Hex( Coord( i, j ), 0 ) );
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
	this->init( w, h );
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

void World::restoreUnits( Sqlite3 & db ) {
	Statement teamdata( db, "SELECT x, y FROM TEAM" );
	int count;
	for ( count = 0; ; count++ ) {
		const int rc = teamdata.step();
		if ( rc == SQLITE_DONE ) break;
		int x, y;
		teamdata.column( 0, x );
		teamdata.column( 1, y );
		Hex * hex = this->tryFind( x, y );
		if ( hex == nullptr ) {
			cerr << "Cannot place team at: (" << x << "," << y << ")" << endl;
		} else {
			this->units.push_back( new Team( hex, 0, 0.0 ) );
			count += 1;
		}
	}
	cout << "Read " << count << " team records." << endl;	
}

void World::restore( const char * db_path_name ) {
	Sqlite3 db( db_path_name );
	this->restoreSize( db );
	this->restoreHexes( db );
	this->restoreUnits( db );
}

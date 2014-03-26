#ifndef SQLITE3_LIB_HPP
#define SQLITE3_LIB_HPP

#include <string>
#include <iostream>
#include <map>
#include <memory>

#include <stdlib.h>

#include <sqlite3.h>

class Sqlite3 {
friend class Statement;
private:
	sqlite3 * db;

public:
	sqlite3 * getDB() {
		return this->db;
	}

public:
	Sqlite3( const std::string & pathname ) {
		const int rc = sqlite3_open( pathname.c_str(), &this->db );
		if ( rc ) {
			fprintf( stderr, "Can't open database: %s\n", sqlite3_errmsg( this->db ) );
			sqlite3_close( db );
			exit( EXIT_FAILURE );
		}
	}

	~Sqlite3() {
		sqlite3_close( this->db );		
	}

public:
	void exec( const std::string & query ) {
		char *zErrMsg = 0;		
		const int rc = sqlite3_exec( this->db, query.c_str(), NULL, NULL, &zErrMsg );
		if ( rc != SQLITE_OK ) {
			std::cerr << "SQL error: " << zErrMsg << std::endl;	
			sqlite3_free( zErrMsg );
		}	
	}

};

class Statement {
private:
	sqlite3_stmt * statement;
public:
	Statement( Sqlite3 & db, const std::string & query ) {
		const int rc = sqlite3_prepare_v2( db.db, query.c_str(), query.size() + 1, & this->statement, NULL );
		if ( rc != SQLITE_OK ) {
			std::cerr << "SQL error: cannot prepare statement: " << query << std::endl;
			exit( EXIT_FAILURE );
		}
	}

	~Statement() {
		sqlite3_finalize( this->statement );
	}

public:
	void bind( const int index, const int value ) {
		const int rc = sqlite3_bind_int( this->statement, index, value );
		if ( rc != SQLITE_OK ) {
			//	@todo
			std::cerr << "SQL error: binding failed (" << rc << "): " << index << ", " << value << std::endl;
			exit( EXIT_FAILURE );
		}
	}

	void bind( const int index, const sqlite3_int64 value ) {
		const int rc = sqlite3_bind_int64( this->statement, index, value );
		if ( rc != SQLITE_OK ) {
			//	@todo
			std::cerr << "SQL error: binding failed (" << rc << "): " << index << ", " << value << std::endl;
			exit( EXIT_FAILURE );
		}
	}

	void bind( const int index, const double value ) {
		const int rc = sqlite3_bind_double( this->statement, index, value );
		if ( rc != SQLITE_OK ) {
			//	@todo
			std::cerr << "SQL error: binding failed: " << index << ", " << value << std::endl;
			exit( EXIT_FAILURE );
		}
	}

	void bind( const int index, const std::string & value ) {
		const int rc = sqlite3_bind_text( this->statement, index, value.c_str(), -1, SQLITE_TRANSIENT );
		if ( rc != SQLITE_OK ) {
			//	@todo
			std::cerr << "SQL error: binding failed: " << index << ", " << value << std::endl;
			exit( EXIT_FAILURE );
		}
	}

	void column( const int index, int & result ) {
		result = sqlite3_column_int( this->statement, index );
	}

	void column( const int index, std::string & result ) {
		const unsigned char * text = sqlite3_column_text( this->statement, index );
		while ( *text != '\0' ) {
			result += static_cast< char >( *text++ );
		}
	}

	int step() {
		//	@todo: sort out return code.
		return sqlite3_step( this->statement );
	}

	void reset() {
		//	@todo: sort out return code.
		sqlite3_reset( this->statement );
	}
};

class IdAllocator {
public:
	typedef sqlite3_int64 ID;

private:
	ID next_id;
	std::map< void *, ID > id_table;

public:
	IdAllocator() : next_id( 0 ) {}

public:
	bool hasObjectID( void * item ) {
		return this->id_table.find( item ) != this->id_table.end();
	}

	ID next() { 
		return this->next_id++;
	}

	ID objectID( void * item ) {
		auto id_it = this->id_table.find( item );
		if ( id_it == this->id_table.end() ) {
			return this->id_table[ item ] = this->next();
		} else {
			return id_it->second;
		}
	}
};

class StatementCache {
private:
	Sqlite3 & db;
	std::map< std::string, std::unique_ptr< Statement > > cache;
	IdAllocator ids;

public:
	StatementCache( Sqlite3 & db ) : db( db ) {}

public:
	IdAllocator & idAllocator() { return this->ids; }

public:
	Statement & get( const std::string & key ) {
		auto it = this->cache.find( key );
		if ( it == this->cache.end() ) {
			this->cache[ key ] = std::unique_ptr< Statement >( new Statement( db, key ) );
			return *( this->cache[ key ] );
		} else {
			return *( it->second );
		}
	}
};

#endif
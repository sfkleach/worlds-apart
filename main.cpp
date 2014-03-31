#include <exception>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <queue>

#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#include "maybe.hpp"
#include "sqlite3lib.hpp"
#include "common.hpp"
#include "embedded.hpp"
#include "hex.hpp"
#include "unit.hpp"
#include "team.hpp"
#include "world.hpp"

using namespace std;

#define SIZE_X 1024
#define SIZE_Y 1024
#define N_UNITS 10000


class CommandArguments {
private:
    deque< string > args;
    map< string, string > options;
    vector< string > others;

public:
    CommandArguments( const int argc, char ** argv ) {
        for ( int i = 1; i < argc; i++ ) {
            this->args.push_back( argv[ i ] );
        }
    }

private:
	static bool startsWith( const string& haystack, const string& needle ) {
	    return (
	        needle.length() <= haystack.length() && 
	        equal( needle.begin(), needle.end(), haystack.begin() )
	    );
	}

public:
    void parse() {
        bool flags_processed = false;
        for ( auto it = this->args.begin(); it != this->args.end(); ++it ) {
            if ( flags_processed ) {
                this->others.push_back( *it );
            } else if ( startsWith( *it, "--" ) ) {
                if ( *it == "--" ) {
                    //  Explicit end of the options.
                    flags_processed = true;
                } else {
                    //  --key=value OR --key
                    const size_t n = it->find( '=', 2 );
                    if ( n == string::npos ) {
                        //  --key
                        this->options[ it->substr( 2 ) ] = "";
                        //  TODO: Mandatory/optional args not implemented yet.
                    } else {
                        //  --key=value
                        this->options[ it->substr( 2, n - 2 ) ] = it->substr( n + 1 );
                    }
                }
            } else if ( startsWith( *it, "-" ) ) {
                //  -abcde
                throw Mishap( "Short options not implemented yet" ).culprit( "Short options", *it );
            } else {
                //  The first non-option turns off the parsing.
                flags_processed = true;
                this->others.push_back( *it );
            }
        }
    }

    void summarise() {
        for ( auto it = this->options.begin(); it != this->options.end(); ++it ) {
            cout << "Option " << it->first << " -> " << it->second << endl;
        }
    }

    Maybe< string > get( const string & key ) {
        auto it = this->options.find( key );
        if ( it == this->options.end() ) {
            return Maybe< string >();
        } else {
            return Maybe< string >( it->second );
        }
    }

    Maybe< int > getInt( const string & key ) {
        auto it = this->options.find( key );
        if ( it == this->options.end() ) {
            return Maybe< int >();
        } else {
            int n = 0;
            stringstream s( it->second );
            s >> n;
            return Maybe< int >( n );
        }        
    }

    int getInt( const string & key, const int def ) {
        auto it = this->options.find( key );
        if ( it == this->options.end() ) {
            return def;
        } else {
            int n = 0;
            stringstream s( it->second );
            s >> n;
            return n;
        }        
    }
};




int main( int argc, char **argv ) {
	CommandArguments args( argc, argv );
	args.parse();
    cout << ">>> summary " << endl;
	args.summarise();
    cout << "<<< summary " << endl;


    Maybe< string > db_name = args.get( "db" );

	World world;
	cout << "Restoring" << endl;
	world.restore( db_name.hasValue() ? db_name.value().c_str() : "world.db" );

    /*Hex * a = world.tryFind( 400, 400 );
    Hex * b = world.tryFind( 410, 410 );
    deque< Link > path;
    FindRoute finder;
    finder.findRoute( a, b );
    finder.outwardsRoute( b, path );

    cout << "Path ..." << path.size() << endl;
    for ( auto it : path ) {
        cout << "(" << it.source->getX() << "," << it.source->getY() << ")";
        cout << " : ";
        cout << it.delta;
        cout << " : ";
        cout << "(" << it.destination->getX() << "," << it.destination->getY() << ")";
        cout << endl;   
    }
    cout << endl;
    cout << "Reverse path" << endl;
    {
        Hex * here = b;
        for ( int i = 0; i < 10; i++ ) {
            cout << "At: " << here->getLocation() << endl;
            Maybe< Move > m = finder.moveToPrevious( here );
            if ( m.hasValue() ){
                cout << "Move: " << m.fastValue() << endl;
                Hex * there = here->tryFindHex( here->getLocation().shift( m.fastValue() ) );
                if ( there != nullptr ) {
                    cout << "There: " << there->getLocation() << endl;
                    here = there;
                } else {
                    cout << "There does not exist" << endl;
                }
            } else {
                cout << "No route" << endl;
            }
        }
    }

    return 0;*/



	//world.populate( SIZE_X, SIZE_Y, N_UNITS );

	world.forkDump( "dump0.db" );

	cout << "Running" << endl;
	const clock_t t0 = clock();
	int count = 0;
    const int NUM_SECS = args.getInt( "max_seconds", 10 );
    const int max_turns = args.getInt( "max_turns", 5000000 );
	for (; count < max_turns;) {
		const clock_t t_now = clock();
		if ( ( t_now - t0 ) > NUM_SECS * CLOCKS_PER_SEC ) break;
		for ( int i = 0; count < max_turns && i < 10000; i++ ) {
			world.runOneUnit();
			count += 1;
		}
	}
    const double num_secs = static_cast< float >( clock() - t0 ) / CLOCKS_PER_SEC;
    cout << "Number of turns: " << count << endl;
    cout << "Time taken     : " << num_secs << " secs" << endl;
	cout << "Number of loops: " << ( count / static_cast< float >( N_UNITS ) ) << endl;
	cout << "Rescheduled " << ( count / num_secs ) << " times per sec." << endl;

	world.forkDump( "dump1.db");

	for (;;) {
		int stat_loc;
		pid_t p = wait( &stat_loc );
		if ( p < 0 ) break;
		cout << "Child " << p << endl;
	}

	return EXIT_SUCCESS;
}
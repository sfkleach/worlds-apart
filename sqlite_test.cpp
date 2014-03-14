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


#define NUM_SECS 1

int main( int argc, char **argv ) {
	World world;
	cout << "Populating" << endl;
	world.restore( "world.db" );
	//world.populate( SIZE_X, SIZE_Y, N_UNITS );

	world.forkDump( "before.db" );

	cout << "Running" << endl;
	const clock_t t0 = clock();
	int count = 0;
	for (;;) {
		const clock_t t_now = clock();
		if ( ( t_now - t0 ) > NUM_SECS * CLOCKS_PER_SEC ) break;
		for ( int i = 0; i < 10000; i++ ) {
			world.runOneUnit();
			count += 1;
		}
	}
	cout << "Number of loops: " << ( count / static_cast< float >( N_UNITS ) ) << endl;
	cout << "Rescheduled " << ( count / static_cast< float >( NUM_SECS ) ) << " times per sec." << endl;

	world.forkDump( "after.db");

	for (;;) {
		int stat_loc;
		pid_t p = wait( &stat_loc );
		if ( p < 0 ) break;
		cout << "Child " << p << endl;
	}

	return EXIT_SUCCESS;
}
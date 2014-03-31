#include <vector>

#include <cassert>

#include "findroute.hpp"
#include "mishap.hpp"

using namespace std;

const bool DEBUG = false;

void FindRoute::findRoute( Hex * start, Hex * finish ) {
	fringe.emplace( start, 0.0 );
	known_chain[ start ] = TipLink( start, 0.0, nullptr, 1 );

	int count = 0;
	for ( ; not fringe.empty(); count++ ) {
		Tip top = fringe.top();
		fringe.pop();

		this->known_chain[ top.head ].count -= 1;
		if ( this->known_chain[ top.head ].time < top.travel_time ) {
			//	Discard.
			continue;
		}

		if ( DEBUG && count % 1000000 == 0 ){
			cout << "Known chain length : " << known_chain.size() << endl;
			cout << "Fringe length      : " << fringe.size() << endl;
		}
		
		//	At this point the route for the top.head is optimal.
		if ( top.head == finish ) break;

		vector< Link > neighbors;
		top.head->findNeighbors( neighbors );

		int added = 0;

		for ( Link link : neighbors ) {
			const double t = top.travel_time + link.delta;

			auto known_it = this->known_chain.find( link.destination );
			if ( known_it != this->known_chain.end() ) {

				TipLink & known_link = known_it->second;
				if ( t + 0.5 < known_link.time ) {

					if ( DEBUG && count % 1000000 == 0 ) {
						cout << "    From       : " << top.head->getLocation() << endl;
						cout << "    To         : " << link.destination->getLocation() << endl;
						cout << "    Improvement: " << t << "<" << known_link.time << endl;
						cout << "    Delta      : " << known_link.time - t << endl;
					}

					known_link.time = t;
					known_link.previous = top.head;
					known_link.count += 1;
					fringe.emplace( link.destination, t );
					added += 1;	
				}				
			} else {
				known_chain[ link.destination ] = TipLink( link.destination, t, top.head, 1 );
				fringe.emplace( link.destination, t );
				added += 1;				
			}
		}

		if ( DEBUG && count % 1000000 == 0 ) {
			cout << "    Added : " << added << endl;
		}		
	}

	if ( DEBUG ) {
		cout << "Num popped to calculate TTM = " << count << endl;
	}
}


void FindRoute::outwardsRoute( Hex * hex, deque< Link > & route ) {
	while ( hex != nullptr ) {
		auto p = known_chain.find( hex );
		if ( p != known_chain.end() ) {
			TipLink & tip_link = p->second;
			if ( tip_link.previous == nullptr ) break;
			route.push_front( 
				Link( 
					tip_link.previous, 
					tip_link.time, 
					tip_link.head 
				) 
			);
			hex = tip_link.previous;
		} else {
			throw Mishap( "Broken chain" );
		}
	}
	cout << "Deque# = " << route.size() << endl;
}

Maybe< Move > FindRoute::moveToPrevious( Hex * here ) {
	auto p = known_chain.find( here );
	if ( p != known_chain.end() ) {
		TipLink & tip_link = p->second;
		if ( tip_link.previous != nullptr ) {
			return Maybe< Move >( here->getLocation().diff( tip_link.previous->getLocation() ) );
		} else {
			return Maybe< Move >();
		}
	} else {
		return Maybe< Move >();
	}
}

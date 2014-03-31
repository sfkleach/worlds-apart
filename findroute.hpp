#ifndef FIND_ROUTE_HPP
#define FIND_ROUTE_HPP

#include <map>
#include <queue>
#include <deque>
#include <memory>
#include <vector>

#include "hex.hpp"
#include "common.hpp"

class FindRoute {

	struct Tip {
		Hex * head;
		double travel_time;
		Tip( Hex * head, double time ) : head( head ), travel_time( time ) {}
	};

	class TipLessThan {
	public:
		bool operator()( const Tip & lhs, const Tip & rhs ) const {
			return lhs.travel_time > rhs.travel_time;
		}
	};

	struct TipLink {
		Hex * head;
		double time;
		Hex * previous;
		int count;
		TipLink( Hex * head, double time, Hex * prev, int count ) : 
			head( head ), 
			time( time ),
			previous( prev ), 
			count( count )  
		{}
		TipLink() : head( nullptr ), time( 0.0 ), previous( nullptr ), count( 0 ) {}
	};

private:
	std::map< Hex *, TipLink > known_chain;
	std::priority_queue< Tip, std::vector< Tip >, TipLessThan > fringe;

public:
	void findRoute( Hex * start, Hex * finish = nullptr );
	void outwardsRoute( Hex * hex, std::deque< Link > & route );
	Maybe< Move > moveToPrevious( Hex * here );

};

#endif
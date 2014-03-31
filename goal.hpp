#ifndef GOAL_HPP
#define GOAL_HPP

#include <string>
#include <memory>

#include "findroute.hpp"
#include "common.hpp"
#include "sqlite3lib.hpp"

class Unit;
class Hex;

enum class GoalType {
	Stay,
	Jiggle,
	GoTo
};

class Goal {
private:
	int id;
	std::string title;
	GoalType code;
	int x;
	int y;
	std::unique_ptr< FindRoute > find_route;

private:
	void instantiateFindRoute( Hex * here );

public:
	void dump( StatementCache & cache );
	int getGoalId() { return this->id; }
	void activity( Unit * );
	int getX() { return this->x; }
	int getY() { return this->y; }
	Maybe< Move > moveTowardsGoal( Hex * here );

public:
	Goal( int id, std::string title, GoalType code, int x, int y ) :
		id( id ),
		title( title ),
		code( code ),
		x( x ),
		y( y ),
		find_route( nullptr )
	{}

};

#endif
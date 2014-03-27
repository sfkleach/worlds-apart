#ifndef GOAL_HPP
#define GOAL_HPP

#include <string>
#include "sqlite3lib.hpp"

class Unit;

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

public:
	void dump( StatementCache & cache );
	int getGoalId() { return this->id; }
	void activity( Unit * );
	int getX() { return this->x; }
	int getY() { return this->y; }

public:
	Goal( int id, std::string title, GoalType code, int x, int y ) :
		id( id ),
		title( title ),
		code( code ),
		x( x ),
		y( y )
	{}

};

#endif
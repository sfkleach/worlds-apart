#ifndef GOAL_HPP
#define GOAL_HPP

#include <string>

class Goal {
private:
	int id;
	std::string title;
	int code;
	int x;
	int y;
public:
	Goal( int id, std::string title, int code, int x, int y ) :
		id( id ),
		title( title ),
		code( code ),
		x( x ),
		y( y )
	{}
};

#endif
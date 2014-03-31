#include "common.hpp"

std::ostream & operator <<( std::ostream & str, const Coord & coord ) {
	str << "(" << coord.x << "," << coord.y << ")";
	return str;
}

std::ostream & operator <<( std::ostream & str, const Move & Move ) {
	str << "(|" << Move.dx << "," << Move.dy << "|)";
	return str;
}

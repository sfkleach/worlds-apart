#include "link.hpp"

bool Link::operator <( const Link & rhs) const {
	return this->delta < rhs.delta;
}

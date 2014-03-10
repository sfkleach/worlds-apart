#ifndef UNIT_HPP
#define UNIT_HPP

#include "embedded.hpp"
#include "hex.hpp"

class Unit : public Embedded {
protected:
	Hex * location;
	int faction;

public:
	Unit( Hex * location, int faction ) :
		location( location ), 
		faction( faction )
	{
		this->location->attach( this );
		this->setWorld( *this->location );
	}

	virtual ~Unit() {
	}

public:
	///	Required for priority queue.
	//bool operator <( const Unit & other ) const {
	//	return this->priority < other.priority;
	//}

public:
	//void placeInWorld( class World * world );
	void move( Hex * new_location );

public:
	virtual double refractoryPeriod() = 0;

	virtual void action() = 0;

public:
	virtual void dump( StatementCache & db ) = 0;
};

#endif
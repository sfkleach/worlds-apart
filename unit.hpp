#ifndef UNIT_HPP
#define UNIT_HPP

#include <memory>
#include <deque>

#include "embedded.hpp"
#include "hex.hpp"
#include "goal.hpp"

class Unit : public Embedded {
protected:
	Hex * location;
	int faction;
	std::shared_ptr< Goal > goal;
	std::deque< Hex * > mini_trail;

public:
	Unit( Hex * location, int faction ) :
		location( location ), 
		faction( faction ),
		goal( nullptr )
	{
		this->location->attach( this );
		this->setWorld( *this->location );
	}

	virtual ~Unit() {
	}

public:
	double getVolumeNeeded() const { return 1.0; }

public:
	void move( Hex * new_location );

public:
	virtual double refractoryPeriod() = 0;
	std::shared_ptr< Goal > getGoal();
	void setGoal( std::shared_ptr< Goal > & g );
	Hex * getLocation() { return this->location; }
	auto isAllowedToOccupy( Hex * hex ) const -> bool;

public:
	virtual void dump( StatementCache & db ) = 0;

private:
	enum class MoveStatus {
		OK, BLOCKED, INACCESSIBLE
	};

	MoveStatus couldMoveTo( Hex * hex ) const;
	MoveStatus couldMove( const Move & move ) const;
	MoveStatus tryMove( const Move & dxdy );
	MoveStatus tryMove( Hex * hex );
	void replan( Goal & goal );
	void extendMiniTrail();
	bool isInMiniTrail( Hex * hex ) const;

public: // behaviour.
	virtual void action();
	virtual void stay( Goal & goal );
	virtual void jiggle( Goal & goal );
	virtual void goTo( Goal & goal );
};

#endif
#ifndef EMBEDDED_HPP
#define EMBEDDED_HPP

class World;

class Embedded {
protected:
	class World * world;
public:
	Embedded() : world( nullptr ) {}
public:
	void setWorld( World * const w ) {
		this->world = w;
	}
	void setWorld( Embedded & embedded ) {
		this->world = embedded.world;
	}
};

#endif
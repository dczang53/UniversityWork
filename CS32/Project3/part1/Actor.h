#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <list>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction dir, int depth, std::list<Actor*>* (*map)[64][64]);
	virtual ~Actor() {}
	virtual bool isDead() = 0;
	virtual void doSomething() = 0;
	virtual bool block() = 0;
protected:
	std::list<Actor*>* (*a_map)[64][64];
};

class Grasshopper : public Actor
{
public:
	Grasshopper(int imageID, int startX, int startY, int health, std::list<Actor*>* (*map)[64][64]);
	virtual ~Grasshopper() {}
	virtual void doSomething() = 0;
protected:
	int g_health;
	int g_state;
	int g_dest;
};

class bGrasshopper : public Grasshopper
{
public:
	bGrasshopper(int startX, int startY, std::list<Actor*>* (*map)[64][64]);
	virtual ~bGrasshopper() {}
	virtual bool isDead() { return b_isDead; }
	virtual void doSomething();
	virtual bool block() { return false; }
private:
	bool b_grow;
	bool b_isDead;
};

class aGrasshopper : public Grasshopper
{
public:
	aGrasshopper(int startX, int startY, int health, std::list<Actor*>* (*map)[64][64]);
	virtual ~aGrasshopper() {}
	virtual bool isDead() { return a_isDead; }
	virtual void doSomething() {}
	virtual bool block() { return false; }
private:
	bool a_isDead;
};

class Pebble : public Actor
{
public:
	Pebble(int startX, int startY, std::list<Actor*>* (*map)[64][64]);
	virtual ~Pebble() {}
	virtual bool isDead() { return false; }
	virtual void doSomething();
	virtual bool block() { return true; }
};

#endif // ACTOR_H_

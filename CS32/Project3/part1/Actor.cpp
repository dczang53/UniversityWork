#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, int startX, int startY, Direction dir, int depth, std::list<Actor*>* (*map)[64][64])
	: GraphObject(imageID, startX, startY, dir, depth), a_map(map)
{}

Grasshopper::Grasshopper(int imageID, int startX, int startY, int health, std::list<Actor*>* (*map)[64][64])
	: Actor(imageID, startX, startY, static_cast<GraphObject::Direction>(randInt(1, 4)), 1, map), g_health(health), g_state(0), g_dest(0)
{}

bGrasshopper::bGrasshopper(int startX, int startY, std::list<Actor*>* (*map)[64][64])
	: Grasshopper(IID_BABY_GRASSHOPPER, startX, startY, 500, map), b_grow(false), b_isDead(false)
{}

void bGrasshopper::doSomething()
{
	g_health--;
	if (g_health <= 0)
	{
		b_isDead = true;
		return;
	}
	if (g_state > 0)
	{
		g_state--;
		return;
	}
	if (g_health >= 1600)
	{
		b_grow = true;
		b_isDead = true;
		return;
	}
	if (g_dest <= 0)
	{
		setDirection(static_cast<GraphObject::Direction>(randInt(1, 4)));
		g_dest = randInt(2, 10);
	}
	switch (getDirection())
	{
	case(up):
		if (!(*a_map)[getX()][getY() + 1]->empty() && ((*a_map)[getX()][getY() + 1]->front()->block() || (*a_map)[getX()][getY() + 1]->back()->block()))
		{
			g_dest = 0;
			break;
		}
		moveTo(getX(), getY() + 1);
		g_dest--;
		break;
	case(right):
		if (!(*a_map)[getX() + 1][getY()]->empty() && ((*a_map)[getX() + 1][getY()]->front()->block() || (*a_map)[getX() + 1][getY()]->back()->block()))
		{
			g_dest = 0;
			break;
		}
		moveTo(getX() + 1, getY());
		g_dest--;
		break;
	case(down):
		if (!(*a_map)[getX()][getY() - 1]->empty() && ((*a_map)[getX()][getY() - 1]->front()->block() || (*a_map)[getX()][getY() - 1]->back()->block()))
		{
			g_dest = 0;
			break;
		}
		moveTo(getX(), getY() - 1);
		g_dest--;
		break;
	case(left):
		if (!(*a_map)[getX() - 1][getY()]->empty() && ((*a_map)[getX() - 1][getY()]->front()->block() || (*a_map)[getX() - 1][getY()]->back()->block()))
		{
			g_dest = 0;
			break;
		}
		moveTo(getX() - 1, getY());
		g_dest--;
		break;
	default:
		break;
	}
	g_state = 2;
}

aGrasshopper::aGrasshopper(int startX, int startY, int health, std::list<Actor*>* (*map)[64][64])
	: Grasshopper(IID_ADULT_GRASSHOPPER, startX, startY, health, map), a_isDead(false)
{}

Pebble::Pebble(int startX, int startY, std::list<Actor*>* (*map)[64][64])
	: Actor(IID_ROCK, startX, startY, right, 1, map)
{}

void Pebble::doSomething()
{
	return;
}
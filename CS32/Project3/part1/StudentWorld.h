#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include "Actor.h"
#include <string>
#include <list>
#include <stack>
#include <queue>
#include <iostream>
#include <sstream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{}

	~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

private:
	int s_ticks;
	bool odd;
	std::list<Actor*>*  s_map[64][64];
	std::queue<int> map_x;
	std::queue<int> map_y;
	std::queue<int> dead_x;
	std::queue<int> dead_y;
	// field[64][64] using stack (w/ structure first and insects after)
	// 2 rows queue for insect pos
};

#endif // STUDENTWORLD_H_

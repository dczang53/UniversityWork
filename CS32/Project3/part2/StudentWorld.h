#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include "Compiler.h"
#include <string>
#include <list>
#include <stack>
#include <queue>
#include <iostream>
#include <sstream>
#include <iomanip>

class Actor;
class AntHill;

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

	std::list<Actor*>* (*world())[64][64] { return &s_map; }

	bool s_odd() const { return odd; }

	void insertAGrasshopper(bool s_odd, int x, int y);

	void insertAnt(bool s_odd, int x, int y, int colony, Compiler* program);

	void insertPh(bool s_odd, int x, int y, int colony);

	void insertFood(bool s_odd, int x, int y, int energy);

	void insertDead(int x, int y);

	void updateAH(int colony);

private:
	int s_ticks;
	bool odd;
	std::list<Actor*>*  s_map[64][64];
	std::queue<int> map_x;
	std::queue<int> map_y;
	std::queue<int> dead_x;
	std::queue<int> dead_y;
	double ah[4];
	std::vector<std::string> fileNames;
};

#endif // STUDENTWORLD_H_

#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld()
{cleanUp();}

int StudentWorld::init()
{
	s_ticks = 0;
	odd = false;
	Field f;
	std::string fieldFile = getFieldFilename();
	std::string error;
	if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
	{
		setError(fieldFile + " " + error);
		return GWSTATUS_LEVEL_ERROR;
	}
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 64; y++)
		{
			s_map[x][y] = new std::list<Actor*>;
			Field::FieldItem z = f.getContentsOf(x, y);
			switch (z)
			{
			case Field::FieldItem::rock:
				s_map[x][y]->push_front(new Pebble(x, y, &s_map));
				break;
			case Field::FieldItem::grasshopper:
				s_map[x][y]->push_front(new bGrasshopper(x, y, &s_map));
				map_x.push(x);
				map_y.push(y);
				break;
			default:
				break;
			}
		}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	s_ticks++;
	odd = !odd;
	if (odd)
	{
		for (int x = map_x.size(); x > 0; x--)
		{
			s_map[map_x.front()][map_y.front()]->front()->doSomething();
			Actor* a = s_map[map_x.front()][map_y.front()]->front();
			s_map[map_x.front()][map_y.front()]->pop_front();
			s_map[a->getX()][a->getY()]->push_back(a);
			if (a->isDead())
			{
				dead_x.push(a->getX());
				dead_y.push(a->getY());
			}
			else
			{
				map_x.push(a->getX());
				map_y.push(a->getY());
			}
			map_x.pop();
			map_y.pop();
		}

		for (int y = dead_x.size(); y > 0; y--)
		{
			std::stack<Actor*> store;
			for (int z = s_map[dead_x.front()][dead_y.front()]->size(); z > 0; z--)
			{
				Actor* b = s_map[dead_x.front()][dead_y.front()]->back();
				if (b->isDead())
				{
					delete b;
					s_map[dead_x.front()][dead_y.front()]->pop_back();
					break;
				}
				else
				{
					store.push(b);
					s_map[dead_x.front()][dead_y.front()]->pop_back();
				}
			}
			while (store.size() > 0)
			{
				s_map[dead_x.front()][dead_y.front()]->push_back(store.top());
				store.pop();
			}
			dead_x.pop();
			dead_y.pop();
		}
	}
	else
	{
		for (int x = map_x.size(); x > 0; x--)
		{
			s_map[map_x.front()][map_y.front()]->back()->doSomething();
			Actor* a = s_map[map_x.front()][map_y.front()]->back();
			s_map[map_x.front()][map_y.front()]->pop_back();
			s_map[a->getX()][a->getY()]->push_front(a);
			if (a->isDead())
			{
				dead_x.push(a->getX());
				dead_y.push(a->getY());
			}
			else
			{
				map_x.push(a->getX());
				map_y.push(a->getY());
			}
			map_x.pop();
			map_y.pop();
		}

		for (int y = dead_x.size(); y > 0; y--)
		{
			std::stack<Actor*> store;
			for (int z = s_map[dead_x.front()][dead_y.front()]->size(); z > 0; z--)
			{
				Actor* b = s_map[dead_x.front()][dead_y.front()]->front();
				if (b->isDead())
				{
					delete b;
					s_map[dead_x.front()][dead_y.front()]->pop_front();
					break;
				}
				else
				{
					store.push(b);
					s_map[dead_x.front()][dead_y.front()]->pop_front();
				}
			}
			while (store.size() > 0)
			{
				s_map[dead_x.front()][dead_y.front()]->push_front(store.top());
				store.pop();
			}
			dead_x.pop();
			dead_y.pop();
		}
	}
	std::ostringstream caption;
	caption << "Ticks: " << 2000 - s_ticks;
	setGameStatText(caption.str());
	if (s_ticks >= 2000)
		return GWSTATUS_NO_WINNER;
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 64; y++)
		{
			while (!s_map[x][y]->empty())
			{
				Actor* a = s_map[x][y]->front();
				delete a;
				s_map[x][y]->pop_front();
			}
			delete s_map[x][y];
		}
}

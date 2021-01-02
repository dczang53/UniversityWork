#include "StudentWorld.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{
	Compiler* cp = new Compiler;
	fileNames = getFilenamesOfAntPrograms();
	std::string errorc;
	ah[0] = 0.;
	ah[1] = 0.;
	ah[2] = 0.;
	ah[3] = 0.;
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
			case Field::FieldItem::anthill0:
				if (fileNames.size() < 1)
					break;
				if (!cp->compile(fileNames[0], errorc))
				{
					setError(fileNames[0] + " " + errorc);
					return GWSTATUS_LEVEL_ERROR;
				}
				s_map[x][y]->push_back(new AntHill(this, x, y, 0, cp));
				map_x.push(x);
				map_y.push(y);
				break;
			case Field::FieldItem::anthill1:
				if (fileNames.size() < 2)
					break;
				if (!cp->compile(fileNames[1], errorc))
				{
					setError(fileNames[1] + " " + errorc);
					return GWSTATUS_LEVEL_ERROR;
				}
				s_map[x][y]->push_back(new AntHill(this, x, y, 1, cp));
				map_x.push(x);
				map_y.push(y);
				break;
			case Field::FieldItem::anthill2:
				if (fileNames.size() < 3)
					break;
				if (!cp->compile(fileNames[2], errorc))
				{
					setError(fileNames[2] + " " + errorc);
					return GWSTATUS_LEVEL_ERROR;
				}
				s_map[x][y]->push_back(new AntHill(this, x, y, 2, cp));
				map_x.push(x);
				map_y.push(y);
				break;
			case Field::FieldItem::anthill3:
				if (fileNames.size() < 4)
					break;
				if (!cp->compile(fileNames[3], errorc))
				{
					setError(fileNames[3] + " " + errorc);
					return GWSTATUS_LEVEL_ERROR;
				}
				s_map[x][y]->push_back(new AntHill(this, x, y, 3, cp));
				map_x.push(x);
				map_y.push(y);
				break;
			case Field::FieldItem::food:
				s_map[x][y]->push_back(new Food(this, x, y, 6000));
				break;
			case Field::FieldItem::water:
				s_map[x][y]->push_back(new WaterPool(this, x, y));
				break;
			case Field::FieldItem::poison:
				s_map[x][y]->push_back(new Poison(this, x, y));
				break;
			case Field::FieldItem::rock:
				s_map[x][y]->push_back(new Pebble(this, x, y));
				break;
			case Field::FieldItem::grasshopper:
				s_map[x][y]->push_back(new BabyGrasshopper(this, x, y));
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
			s_map[map_x.front()][map_y.front()]->back()->doSomething();
			Actor* a = s_map[map_x.front()][map_y.front()]->back();
			s_map[map_x.front()][map_y.front()]->pop_back();
			s_map[a->getX()][a->getY()]->push_front(a);
			map_x.push(a->getX());
			map_y.push(a->getY());
			map_x.pop();
			map_y.pop();
		}

		for (int x = 0; x < 64; x++)
		{
			for (int y = 0; y < 64; y++)
			{
				std::list<Actor*>::iterator n = s_map[x][y]->begin();
				while (n != s_map[x][y]->end())
				{
					(*n)->doSomething();
					n++;
				}
			}
		}

		for (int n = map_x.size(); n > 0; n--)
		{
			int x = 0;
			std::stack<Actor*> store;
			for (int z = s_map[map_x.front()][map_y.front()]->size(); z > 0; z--)
			{
				Actor* b = s_map[map_x.front()][map_y.front()]->front();
				if (b->isDead())
				{
					delete b;
					s_map[map_x.front()][map_y.front()]->pop_front();
					x++;
					break;
				}
				else
				{
					store.push(b);
					s_map[map_x.front()][map_y.front()]->pop_front();
				}
			}
			while (store.size() > 0)
			{
				s_map[map_x.front()][map_y.front()]->push_front(store.top());
				store.pop();
			}
			if (x != 0)
			{
				map_x.pop();
				map_y.pop();
			}
			else
			{
				map_x.push(map_x.front());
				map_y.push(map_y.front());
				map_x.pop();
				map_y.pop();
			}
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

	else
	{
		for (int x = map_x.size(); x > 0; x--)
		{
			s_map[map_x.front()][map_y.front()]->front()->doSomething();
			Actor* a = s_map[map_x.front()][map_y.front()]->front();
			s_map[map_x.front()][map_y.front()]->pop_front();
			s_map[a->getX()][a->getY()]->push_back(a);
			map_x.push(a->getX());
			map_y.push(a->getY());
			map_x.pop();
			map_y.pop();
		}
		
		for (int x = 0; x < 64; x++)
		{
			for (int y = 0; y < 64; y++)
			{
				std::list<Actor*>::iterator n = s_map[x][y]->begin();
				while (n != s_map[x][y]->end())
				{
					(*n)->doSomething();
					n++;
				}
			}
		}

		for (int n = map_x.size(); n > 0; n--)
		{
			int x = 0;
			std::stack<Actor*> store;
			for (int z = s_map[map_x.front()][map_y.front()]->size(); z > 0; z--)
			{
				Actor* b = s_map[map_x.front()][map_y.front()]->front();
				if (b->isDead())
				{
					delete b;
					s_map[map_x.front()][map_y.front()]->pop_front();
					x++;
					break;
				}
				else
				{
					store.push(b);
					s_map[map_x.front()][map_y.front()]->pop_front();
				}
			}
			while (store.size() > 0)
			{
				s_map[map_x.front()][map_y.front()]->push_front(store.top());
				store.pop();
			}
			if (x != 0)
			{
				map_x.pop();
				map_y.pop();
			}
			else
			{
				map_x.push(map_x.front());
				map_y.push(map_y.front());
				map_x.pop();
				map_y.pop();
			}
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
	std::ostringstream caption;
	caption << "Ticks:" << setw(5) << (2000 - s_ticks);
	for (int x = 0; x < fileNames.size(); x++)
	{
		if (x == 0)
			caption << " - ";
		if (x == 0 && ah[0] > ah[1] && ah[0] > ah[2] && ah[0] > ah[3])
		{
			caption << fileNames[x] << "*" << ": ";
		}
		else if (x == 1 && ah[1] > ah[0] && ah[1] > ah[2] && ah[1] > ah[3])
		{
			caption << fileNames[x] << "*" << ": ";
		}
		else if (x == 2 && ah[2] > ah[0] && ah[2] > ah[1] && ah[2] > ah[3])
		{
			caption << fileNames[x] << "*" << ": ";
		}
		else if (x == 3 && ah[3] > ah[0] && ah[3] > ah[1] && ah[3] > ah[2])
		{
			caption << fileNames[x] << "*" << ": ";
		}
		else
		{caption << fileNames[x] << ": ";}
		if (ah[x] > 9)
			caption << static_cast<int>(ah[x]) << "  ";
		else
			caption << "0" << static_cast<int>(ah[x]) << "  ";
	}
	setGameStatText(caption.str());
	if (s_ticks >= 2000)
	{
		if (fileNames.size() > 0 && ah[0] > ah[1] && ah[0] > ah[2] && ah[0] > ah[3])
		{
			setWinner(fileNames[0]);
			return GWSTATUS_PLAYER_WON;
		}
		else if (fileNames.size() > 1 && ah[1] > ah[0] && ah[1] > ah[2] && ah[1] > ah[3])
		{
			setWinner(fileNames[1]);
			return GWSTATUS_PLAYER_WON;
		}
		else if (fileNames.size() > 2 && ah[2] > ah[0] && ah[2] > ah[1] && ah[2] > ah[3])
		{
			setWinner(fileNames[2]);
			return GWSTATUS_PLAYER_WON;
		}
		else if (fileNames.size() > 3 && ah[3] > ah[0] && ah[3] > ah[1] && ah[3] > ah[2])
		{
			setWinner(fileNames[3]);
			return GWSTATUS_PLAYER_WON;
		}
		else
			return GWSTATUS_NO_WINNER;
	}
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

void StudentWorld::insertAGrasshopper(bool s_odd, int x, int y)
{
	if(s_odd)
		s_map[x][y]->push_front(new AdultGrasshopper(this, x, y));
	else
		s_map[x][y]->push_back(new AdultGrasshopper(this, x, y));
	map_x.push(x);
	map_y.push(y);
}

void StudentWorld::insertAnt(bool s_odd, int x, int y, int colony, Compiler* program)
{
	if (s_odd)
		s_map[x][y]->push_front(new Ant(this, x, y, colony, program, colony + 11));
	else
		s_map[x][y]->push_back(new Ant(this, x, y, colony, program, colony + 11));
	map_x.push(x);
	map_y.push(y);
}

void StudentWorld::insertPh(bool s_odd, int x, int y, int colony)
{
	if (s_odd)
		s_map[x][y]->push_front(new Pheromone(this, x, y, colony));
	else
		s_map[x][y]->push_back(new Pheromone(this, x, y, colony));
	map_x.push(x);
	map_y.push(y);
}

void StudentWorld::insertFood(bool s_odd, int x, int y, int energy)
{
	std::list<Actor*>::iterator i = s_map[x][y]->begin();
	while (i != s_map[x][y]->end())
	{
		if ((*i)->isEdible())
		{break;}
		i++;
	}
	if (i != s_map[x][y]->end())
	{
		(*i)->updateEnergy(energy);
	}
	else
	{
		if (s_odd)
			s_map[x][y]->push_back(new Food(this, x, y, energy));
		else
			s_map[x][y]->push_front(new Food(this, x, y, energy));
	}
}

void StudentWorld::insertDead(int x, int y)	//for special actors
{
	dead_x.push(x);
	dead_y.push(y);
}

void StudentWorld::updateAH(int colony)
{
	ah[colony] = ah[colony] + 1.;
	switch (colony)
	{
	case 0:
		if (ah[0] > 5 && ah[0] > ah[1] && ah[0] > ah[2] && ah[0] > ah[3])
		{
			ah[0] += 0.01;
			ah[1] = static_cast<int>(ah[1]);
			ah[2] = static_cast<int>(ah[2]);
			ah[3] = static_cast<int>(ah[3]);
		}
		break;
	case 1:
		if (ah[1] > 5 && ah[1] > ah[0] && ah[1] > ah[2] && ah[1] > ah[3])
		{
			ah[1] += 0.01;
			ah[0] = static_cast<int>(ah[0]);
			ah[2] = static_cast<int>(ah[2]);
			ah[3] = static_cast<int>(ah[3]);
		}
		break;
	case 2:
		if (ah[2] > 5 && ah[2] > ah[0] && ah[2] > ah[1] && ah[2] > ah[3])
		{
			ah[2] += 0.01;
			ah[0] = static_cast<int>(ah[0]);
			ah[1] = static_cast<int>(ah[1]);
			ah[3] = static_cast<int>(ah[3]);
		}
		break;
	case 3:
		if (ah[3] > 5 && ah[3] > ah[0] && ah[3] > ah[1] && ah[3] > ah[2])
		{
			ah[3] += 0.01;
			ah[0] = static_cast<int>(ah[0]);
			ah[1] = static_cast<int>(ah[1]);
			ah[2] = static_cast<int>(ah[2]);
		}
		break;
	default:
		break;
	}
}

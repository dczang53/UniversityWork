#include "Actor.h"
#include <cmath>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* world, int startX, int startY, Direction startDir, int imageID, int depth)
	:GraphObject(imageID, startX, startY, startDir, depth), w(world)
{}

/////

Pebble::Pebble(StudentWorld* sw, int startX, int startY)
	: Actor(sw, startX, startY, right, IID_ROCK, 1)
{}

/////

EnergyHolder::EnergyHolder(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID, int depth)
	: Actor(sw, startX, startY, startDir, imageID, 1), e_energy(energy), e_dead(false)
{}

void EnergyHolder::updateEnergy(int amt)
{
	e_energy += amt;
	if (e_energy <= 0)
		e_dead = true;
}

/////

Food::Food(StudentWorld* sw, int startX, int startY, int energy)
	:EnergyHolder(sw, startX, startY, right, energy, IID_FOOD, 2)
{}

void Food::setDead()
{
	if (energy() <= 0)
	{
		getWorld();
	}
}

/////

AntHill::AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program)
	:EnergyHolder(sw, startX, startY, right, 8999, IID_ANT_HILL, 2), ah_colony(colony), ah_compiler(program)
{}

void AntHill::doSomething()
{
	updateEnergy(-1);
	if (isDead())
		getWorld()->insertDead(getX(), getY());
	std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
	int consumed = 0;
	while ((i != (*(getWorld()->world()))[getX()][getY()]->end()) && consumed < 10000)
	{
		if ((*i)->isEdible())
		{
			if ((*i)->energy() <= (10000 - consumed))
			{
				updateEnergy((*i)->energy());
				(*i)->updateEnergy(-1 * ((*i)->energy()));
				getWorld()->insertDead((*i)->getX(), (*i)->getY());
			}
			else
			{
				updateEnergy(10000 - consumed);
				(*i)->updateEnergy(consumed - 10000);
			}
		}
		i++;
	}
	if (energy() >= 2000)
	{
		giveBirth();
		return;
	}
}

void AntHill::giveBirth()
{
	updateEnergy(-1500);
	getWorld()->insertAnt(getWorld()->s_odd(), getX(), getY(), ah_colony, ah_compiler);
	getWorld()->updateAH(ah_colony);
}

/////

Pheromone::Pheromone(StudentWorld* sw, int startX, int startY, int colony)
	:EnergyHolder( sw, startX, startY, right, 256, colony + 11, 2), p_colony(colony)
{}

void Pheromone::doSomething()
{
	updateEnergy(-1);
	if (isDead())
		getWorld()->insertDead(getX(), getY());
}

/////

TriggerableActor::TriggerableActor(StudentWorld* sw, int x, int y, int imageID)
	: Actor(sw, x, y, right, imageID, 2)
{}

/////

WaterPool::WaterPool(StudentWorld* sw, int x, int y)
	: TriggerableActor(sw, x, y, IID_WATER_POOL)
{}

void WaterPool::doSomething()
{
	std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
	while (i != (*(getWorld()->world()))[getX()][getY()]->end())
	{
		(*i)->getStunned();
		i++;
	}
}



/////

Poison::Poison(StudentWorld* sw, int x, int y)
	:TriggerableActor(sw, x, y, IID_POISON)
{}

void Poison::doSomething()
{
	std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
	while (i != (*(getWorld()->world()))[getX()][getY()]->end())
	{
		(*i)->getPoisoned();
		i++;
	}
}

/////

Insect::Insect(StudentWorld* world, int startX, int startY, int energy, int imageID)
	:EnergyHolder(world, startX, startY, static_cast<GraphObject::Direction>(randInt(1, 4)), energy, imageID, 1), i_sleep(0), i_moved(false), i_stunned(false)
{}


bool Insect::moveForwardIfPossible()
{
	switch (getDirection())
	{
	case up:
		if ((*(getWorld()->world()))[getX()][getY() + 1]->empty() || (!(*(getWorld()->world()))[getX()][getY() + 1]->front()->blocksMovement() && !(*(getWorld()->world()))[getX()][getY() + 1]->back()->blocksMovement()))
		{
			moveTo(getX(), getY() + 1);
			return true;
		}
		break;
	case right:
		if ((*(getWorld()->world()))[getX() + 1][getY()]->empty() || (!(*(getWorld()->world()))[getX() + 1][getY()]->front()->blocksMovement() && !(*(getWorld()->world()))[getX() + 1][getY()]->back()->blocksMovement()))
		{
			moveTo(getX() + 1, getY());
			return true;
		}
		break;
	case down:
		if ((*(getWorld()->world()))[getX()][getY() - 1]->empty() || (!(*(getWorld()->world()))[getX()][getY() - 1]->front()->blocksMovement() && !(*(getWorld()->world()))[getX()][getY() - 1]->back()->blocksMovement()))
		{
			moveTo(getX(), getY() - 1);
			return true;
		}
		break;
	case left:
		if ((*(getWorld()->world()))[getX() - 1][getY()]->empty() || (!(*(getWorld()->world()))[getX() - 1][getY()]->front()->blocksMovement() && !(*(getWorld()->world()))[getX() - 1][getY()]->back()->blocksMovement()))
		{
			moveTo(getX() - 1, getY());
			return true;
		}
		break;
	default:
		return false;
	}
	return false;
}

void Insect::getStunned()
{
	if (!i_stunned)
	{
		i_sleep += 2;
		setStunned();
	}
}


void Insect::bite(int colony)
{
	int c = 0;
	std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
	while (i != (*(getWorld()->world()))[getX()][getY()]->end())
	{
		if ((*i)->canBite() && (*i) != this && (*i)->isEnemy(colony))
			c++;
		i++;
	}
	if (c == 0)
		return;
	std::list<Actor*>::iterator j = (*(getWorld()->world()))[getX()][getY()]->begin();
	int b = randInt(1, c);
	c = 0;
	while (c < (b - 1))
	{
		if ((*j)->canBite() && (*j) != this && (*j)->isEnemy(colony))
			c++;
		j++;
	}
	if (this->isAdult())
		(*j)->updateEnergy(-50);
	else
		(*j)->updateEnergy(-15);
	(*j)->wasBitten();
	if ((*j)->isAdult() && !(*j)->alreadyBitBack())
	{
		c = randInt(0, 1);
		if (c == 1)
			(*j)->bite(999);
	}
	if ((*j)->isDead())
		getWorld()->insertDead((*j)->getX(), (*j)->getY());
}


/////

Ant::Ant(StudentWorld* sw, int startX, int startY, int colony, Compiler* program, int imageID)
	:Insect(sw, startX, startY, 1500, imageID), a_colony(colony), a_compiler(program), a_counter(0), a_cmd(0), blocked(false), bitten(false), pickedUp(0), lastRand(0)
{}

void Ant::doSomething()
{
	updateEnergy(-1);
	a_cmd = 0;
	if (!moved())
	{
		updateEnergy(-1);
		setMoved();
		if (isDead())
		{
			getWorld()->insertFood(getWorld()->s_odd(), getX(), getY(), 100);
			return;
		}
		if (sleep() > 0)
		{
			increaseSleepTicks(-1);
			return;
		}
		Compiler::Command cmd;
		for (;a_cmd < 10;)
		{
			if (!a_compiler->getCommand(a_counter, cmd))
			{
				setDead();
				return;
			}
			switch (cmd.opcode)		//see p34
			{
			case Compiler::Opcode::emitPheromone:
				getWorld()->insertPh(getWorld()->s_odd(), getX(), getY(), a_colony);
				a_counter++;
				return;
				break;
			case Compiler::Opcode::faceRandomDirection:
				setDirection(static_cast<GraphObject::Direction>(randInt(1, 4)));
				a_counter++;
				return;
				break;
			case Compiler::Opcode::rotateClockwise:
				if (getDirection() == up)
					setDirection(right);
				else if (getDirection() == right)
					setDirection(down);
				else if (getDirection() == down)
					setDirection(left);
				else if (getDirection() == left)
					setDirection(up);
				a_counter++;
				return;
				break;
			case Compiler::Opcode::rotateCounterClockwise:
				if (getDirection() == left)
					setDirection(down);
				else if (getDirection() == down)
					setDirection(right);
				else if (getDirection() == right)
					setDirection(up);
				else if (getDirection() == up)
					setDirection(left);
				a_counter++;
				return;
				break;
			case Compiler::Opcode::moveForward:
				if (!moveForwardIfPossible())
				{blocked = true;}
				else
				{
					blocked = false;
					bitten = false;
				}
				a_counter++;
				return;
				break;
			case Compiler::Opcode::bite:
				bite(a_colony);
				a_counter++;
				return;
				break;
			case Compiler::Opcode::pickupFood:
				pickUp();
				a_counter++;
				return;
				break;
			case Compiler::Opcode::dropFood:
				drop();
				a_counter++;
				return;
				break;
			case Compiler::Opcode::eatFood:
				eat();
				a_counter++;
				return;
				break;
			case Compiler::Opcode::generateRandomNumber:
				lastRand = randInt(0, (stoi(cmd.operand1) - 1));
				a_cmd++;
				a_counter++;
				break;
			case Compiler::Opcode::goto_command:
				a_counter = stoi(cmd.operand1);
				a_cmd++;
				break;
			case Compiler::Opcode::if_command:
				switch (stoi(cmd.operand1))
				{
				case 0:
					if (dangerInFront(getDirection()))
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 1:
					if (pInFront(getDirection()))
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 2:
					if (bitten)
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 3:
					if (pickedUp > 0)
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 4:
					if (energy() <= 25)
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 5:
					if (onAnthill(a_colony))
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 6:
					if (onFood())
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 7:
					if (withEnemy())
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 8:
					if (blocked)
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				case 9:
					if (lastRand == 0)
					{
						a_counter = stoi(cmd.operand2);
						a_cmd++;
						break;
					}
					a_counter++;
					a_cmd++;
					break;
				default:
					break;
				}
				break;
			default:
				a_counter++;
				break;
			}
		}

	}
	else
	{
		setMoved();
	}
}

void Ant::pickUp()
{
	for (std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin(); i != (*(getWorld()->world()))[getX()][getY()]->end(); i++)
	{
		if ((*i)->isEdible())
		{
			if ((*i)->energy() > 400)
			{
				if ((1800 - pickedUp) < 400)
				{
					(*i)->updateEnergy(-1 * (1800 - pickedUp));
					pickedUp = 1800;
					break;
				}
				else
				{
					pickedUp += 400;
					(*i)->updateEnergy(-400);
					break;
				}
			}
			if ((*i)->energy() < 400)
			{
				if ((1800 - pickedUp) < (*i)->energy())
				{
					(*i)->updateEnergy(-1 * (1800 - pickedUp));
					pickedUp = 1800;
					break;
				}
				else
				{
					pickedUp += (*i)->energy();
					(*i)->updateEnergy(-1 * ((*i)->energy()));
					getWorld()->insertDead((*i)->getX(), (*i)->getY());
					break;
				}
			}
		}
	}
}

void Ant::drop()
{
	if (onAnthill(a_colony))
	{
		std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY()]->end())
		{
			if ((*i)->isAntHill(a_colony))
				break;
			i++;
		}
		(*i)->updateEnergy(pickedUp);
		pickedUp = 0;
	}
	else
	{
		getWorld()->insertFood(getWorld()->s_odd(), getX(), getY(), pickedUp);
		pickedUp = 0;
	}
}

void Ant::eat()
{
	if (pickedUp < 100)
	{
		updateEnergy(pickedUp);
		pickedUp = 0;
	}
	else
	{
		updateEnergy(100);
		pickedUp -= 100;
	}
}

bool Ant::dangerInFront(Direction dir)
{
	std::list<Actor*>::iterator i;
	switch (dir)
	{
	case up:
		i = (*(getWorld()->world()))[getX()][getY() + 1]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isDangerous(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	case right:
		i = (*(getWorld()->world()))[getX() + 1][getY()]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isDangerous(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	case down:
		i = (*(getWorld()->world()))[getX()][getY() - 1]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isDangerous(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	case left:
		i = (*(getWorld()->world()))[getX() - 1][getY()]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isDangerous(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	default:
		return false;
	}
}

bool Ant::pInFront(Direction dir)
{
	std::list<Actor*>::iterator i;
	switch (dir)
	{
	case up:
		i = (*(getWorld()->world()))[getX()][getY() + 1]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isPheromone(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	case right:
		i = (*(getWorld()->world()))[getX() + 1][getY()]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isPheromone(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	case down:
		i = (*(getWorld()->world()))[getX()][getY() - 1]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isPheromone(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	case left:
		i = (*(getWorld()->world()))[getX() - 1][getY()]->begin();
		while (i != (*(getWorld()->world()))[getX()][getY() + 1]->end())
		{
			if ((*i)->isPheromone(a_colony))
				return true;
			i++;
		}
		return false;
		break;
	default:
		return false;
	}
}

bool Ant::onAnthill(int colony)
{
	std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
	while (i != (*(getWorld()->world()))[getX()][getY()]->end())
	{
		if ((*i)->isAntHill(colony))
			return true;
		i++;
	}
	return false;
}

bool Ant::onFood()
{
	std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
	while (i != (*(getWorld()->world()))[getX()][getY()]->end())
	{
		if ((*i)->isEdible())
			return true;
		i++;
	}
	return false;
}

bool Ant::withEnemy()
{
	std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin();
	while (i != (*(getWorld()->world()))[getX()][getY()]->end())
	{
		if ((*i)->isEnemy(a_colony))
			return true;
		i++;
	}
	return false;
}

/////

Grasshopper::Grasshopper(StudentWorld* sw, int startX, int startY, int energy, int imageID, bool gn)
	:Insect(sw, startX, startY, energy, imageID), g_grown(gn), g_dest(randInt(2, 10)), g_jumped(false), g_wbite(false)
{}

void Grasshopper::doSomething()
{
	if (alreadyBitBack())
	{setBitBack();}
	if (!moved())
	{
		updateEnergy(-1);
		setMoved();
		if (isDead())
		{
			getWorld()->insertFood(getWorld()->s_odd(), getX(), getY(), 100);
			return;
		}
		if (sleep() > 0)
		{
			increaseSleepTicks(-1);
			return;
		}
		if (isAdult())
		{
			int x = randInt(1, 3);
			if (x == 2)
			{
				g_wbite = true;
				return;
			}
			int y = randInt(1, 10);
			
			if (y == 10)
				for (;;)
				{
					int r = randInt(-10, 10);
					double c = randInt(0, 31415);
					c = c / 10000.;
					int a = (r*cos(c)) + getX();
					int b = (r*sin(c)) + getY();
					if ((a > 0) && (a < 64) && (b > 0) && (b < 64))
						if ((*(getWorld()->world()))[a][b]->empty() || (!(*(getWorld()->world()))[a][b]->front()->blocksMovement() && !(*(getWorld()->world()))[a][b]->back()->blocksMovement()))
						{
							moveTo(a, b);
							increaseSleepTicks(2);
							if (stunned())
								setStunned();
							return;
						}
				}
		}

		if (energy() >= 1600 && !g_grown)
		{
			setDead();
			getWorld()->insertAGrasshopper(getWorld()->s_odd(), getX(), getY());
			return;
		}

		for (std::list<Actor*>::iterator i = (*(getWorld()->world()))[getX()][getY()]->begin(); i != (*(getWorld()->world()))[getX()][getY()]->end(); i++)
		{
			if ((*i)->isEdible())
			{
				if ((*i)->energy() <= 200)
				{
					updateEnergy((*i)->energy());
					(*i)->updateEnergy(-1 * ((*i)->energy()));
					getWorld()->insertDead((*i)->getX(), (*i)->getY());
					increaseSleepTicks(randInt(0, 1));
					break;
				}
				else
				{
					updateEnergy(200);
					(*i)->updateEnergy(-200);
					increaseSleepTicks(randInt(0, 1));
					break;
				}
			}
		}
		if (sleep() > 0)
		{
			increaseSleepTicks(1);
			return;
		}
		if (g_dest <= 0)
		{
			setDirection(static_cast<GraphObject::Direction>(randInt(1, 4)));
			g_dest = randInt(2, 10);
		}
		if (!moveForwardIfPossible())
		{
			g_dest = 0;
			increaseSleepTicks(2);
			return;
		}
		g_dest--;
		increaseSleepTicks(2);
		if (stunned())
			setStunned();
	}
	else
	{
		if (g_wbite)
			bite(999);
		setMoved();
	}
}

/////

BabyGrasshopper::BabyGrasshopper(StudentWorld* sw, int startX, int startY)
	:Grasshopper(sw, startX, startY, 500, IID_BABY_GRASSHOPPER, false)
{}

/////

AdultGrasshopper::AdultGrasshopper(StudentWorld* sw, int startX, int startY)
	: Grasshopper(sw, startX, startY, 1600, IID_ADULT_GRASSHOPPER, true), bitBack(false)
{}

/*
colony: USCAnt // first line specifies the ant's name

// This program controls a single ant and causes it to move
// around the field and do things.
// This ant moves around randomly, picks up food if it
// happens to stumble upon it, eats when it gets hungry,
// and will drop food on its anthill if it happens to
// stumble back on its anthill while holding food.

// here's the ant's programming instructions, written
// in our 'Bugs' language

start:
faceRandomDirection
moveForward
if i_am_standing_on_food then goto on_food
if i_am_hungry then goto eat_food
if i_am_standing_on_my_anthill then goto on_hill
goto start 	// jump back to the "start:" line

on_food:
pickUpFood
goto start 	// jump back to the "start:" line

eat_food:
eatFood          // assumes our ant has already picked up food
goto start 	// jump back to the "start:" line

on_hill:
dropFood    // feed the anthill's queen so she
// can produce more ants for the colony
goto start  // jump back to the "start:" line

*/
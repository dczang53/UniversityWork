#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "Compiler.h"

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int startX, int startY, Direction startDir, int imageID, int depth);

	// Action to perform each tick.
	virtual void doSomething() = 0;

	// Is this actor dead?
	virtual bool isDead() const { return false; }

	// Does this actor block movement?
	virtual bool blocksMovement() const { return false; }

	virtual bool canBite() { return false; }

	virtual void wasBitten() {}

	// Cause this actor to be be poisoned.
	virtual void getPoisoned() {}

	// Cause this actor to be be stunned.
	virtual void getStunned() {}

	// Can this actor be picked up to be eaten?
	virtual bool isEdible() const { return false; }

	// Is this actor detected by an ant as a pheromone?
	virtual bool isPheromone(int colony) const { return false; }

	virtual void updateEnergy(int amt) {}
	virtual int energy() const { return 0; }

	// Is this actor an enemy of an ant of the indicated colony?
	virtual bool isEnemy(int colony) const { return false; }

	// Is this actor detected as dangerous by an ant of the indicated colony?
	virtual bool isDangerous(int colony) const { return true; }

	// Is this actor the anthill of the indicated colony?
	virtual bool isAntHill(int colony) const { return false; }

	virtual void bite(int colony) {}

	virtual bool alreadyBitBack() { return true; }

	virtual void setBitBack() {}

	virtual bool isAdult() { return false; }

	// Get this actor's world.
	StudentWorld* getWorld() const { return w; }

private:
	StudentWorld* w;
};

class Pebble : public Actor
{
public:
	Pebble(StudentWorld* sw, int startX, int startY);
	virtual void doSomething() {}
	virtual bool blocksMovement() const { return true; }
	virtual bool isDangerous(int colony) const { return false; }
};

class EnergyHolder : public Actor
{
public:
	EnergyHolder(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID, int depth);
	virtual bool isDead() const { return e_dead; }
	virtual void setDead() { e_dead = true; }

	// Adjust this actor's amount of energy upward or downward.
	virtual void updateEnergy(int amt);

	int energy() const { return e_energy; }

private:
	int e_energy;
	bool e_dead;
};


class Food : public EnergyHolder
{
public:
	Food(StudentWorld* sw, int startX, int startY, int energy);
	virtual void doSomething() {}
	virtual bool isEdible() const { return true; }
	virtual void getPoisoned() {}
	virtual void getStunned() {}
	virtual bool isDangerous(int colony) const { return false; }
	void setDead();
};



class AntHill : public EnergyHolder
{
public:
	AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program);
	virtual void doSomething();
	virtual bool isMyHill(int colony) const { return colony == ah_colony; }
	virtual bool isDangerous(int colony) const { return false; }
	virtual bool isAntHill(int colony) const { return (ah_colony == colony); }
	void giveBirth();
private:
	int ah_colony;
	Compiler* ah_compiler;
};



class Pheromone : public EnergyHolder
{
public:
	Pheromone(StudentWorld* sw, int startX, int startY, int colony);
	virtual void doSomething();
	virtual bool isDangerous(int colony) const { return false; }
	virtual bool isPheromone(int colony) const { return (colony == p_colony); }

	// Increase the strength (i.e., energy) of this pheromone.
	void increaseStrength(int a) { updateEnergy(a); }

private:
	int p_colony;
};


class TriggerableActor : public Actor
{
public:
	TriggerableActor(StudentWorld* sw, int x, int y, int imageID);
	virtual bool isDangerous(int colony) const { return false; }
};

class WaterPool : public TriggerableActor
{
public:
	WaterPool(StudentWorld* sw, int x, int y);
	virtual void doSomething();
};

class Poison : public TriggerableActor
{
public:
	Poison(StudentWorld* sw, int x, int y);
	virtual void doSomething();
};

class Insect : public EnergyHolder
{
public:
	Insect(StudentWorld* world, int startX, int startY, int energy, int imageID);
	virtual void getPoisoned() { updateEnergy(-150); }
	virtual void getStunned();
	bool stunned() { return i_stunned; }
	void setStunned() { i_stunned = !i_stunned; }
	virtual bool becomesFoodUponDeath() const { return isDead(); }

	virtual bool moveForwardIfPossible();

	void increaseSleepTicks(int amt) { i_sleep += amt; }

	int sleep() const { return i_sleep; }

	bool moved() const { return i_moved; }

	void setMoved() { i_moved = !i_moved; }

	virtual void bite(int colony);

private:
	int i_sleep;
	bool i_moved;
	bool i_stunned;
};


class Ant : public Insect
{
public:
	Ant(StudentWorld* sw, int startX, int startY, int colony, Compiler* program, int imageID);
	virtual void doSomething();
	void pickUp();
	void drop();
	void eat();
	bool dangerInFront(Direction dir);
	bool pInFront(Direction dir);
	bool onAnthill(int colony);
	bool onFood();
	bool withEnemy();
	virtual bool isEnemy(int colony) const { return !(colony == a_colony); }
	virtual bool isDangerous(int colony) const { return !(colony == a_colony); }
	virtual void wasBitten() { bitten = true; }
private:
	int a_colony;
	Compiler* a_compiler;
	int a_counter;
	int a_cmd;
	bool blocked;
	bool bitten;
	int pickedUp;
	int lastRand;
};


class Grasshopper : public Insect
{
public:
	Grasshopper(StudentWorld* sw, int startX, int startY, int energy, int imageID, bool gn = false);
	virtual void doSomething();
	virtual bool isEnemy(int colony) const { return true; }
private:
	bool g_grown;
	int g_dest;
	bool g_jumped;
	bool g_wbite;
};

class BabyGrasshopper : public Grasshopper
{
public:
	BabyGrasshopper(StudentWorld* sw, int startX, int startY);
	virtual bool canBite() { return true; }
	virtual void bite() {}
};


class AdultGrasshopper : public Grasshopper
{
public:
	AdultGrasshopper(StudentWorld* sw, int startX, int startY);
	virtual bool isAdult() { return true; }
	virtual bool canBite() { return true; }
	virtual bool alreadyBitBack() { return bitBack; }
	virtual void setBitBack() { bitBack = !bitBack; }
	virtual void getPoisoned() {}
	virtual void getStunned() {}
private:
	bool bitBack;
};


#endif // ACTOR_H_

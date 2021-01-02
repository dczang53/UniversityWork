#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include <string>
using namespace std;

class Arena;

class Game
{
public:
	// Constructor/destructor
	Game(int rows, int cols, int nRats);
	~Game();

	// Mutators
	void play();

private:
	Arena* m_arena;

	// Helper functions
	string takePlayerTurn();
};

#endif //GAME_INCLUDED
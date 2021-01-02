// rats.cpp

// Portions you are to complete are marked with a TODO: comment.
// We've provided some incorrect return statements (so indicated) just
// to allow this skeleton program to compile and run, albeit incorrectly.
// The first thing you probably want to do is implement the utterly trivial
// functions (marked TRIVIAL).  Then get Arena::display going.  That gives
// you more flexibility in the order you tackle the rest of the functionality.
// As you finish implementing each TODO: item, remove its TODO: comment.

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
#include <cctype>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;            // max number of rows in the arena
const int MAXCOLS = 20;            // max number of columns in the arena
const int MAXRATS = 100;           // max number of rats allowed

const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;
const int NUMDIRS = 4;

const int EMPTY = 0;
const int HAS_POISON = 1;

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
			  // type name, since it's mentioned in the Rat declaration.

class Rat
{
public:
	// Constructor
	Rat(Arena* ap, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;
	bool isDead() const;

	// Mutators
	void move();

private:
	Arena* m_arena;
	int    m_row;
	int    m_col;
	int m_health;
	bool m_move;
};

class Player
{
public:
	// Constructor
	Player(Arena* ap, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;
	bool isDead() const;

	// Mutators
	string dropPoisonPellet();
	string move(int dir);
	void   setDead();

private:
	Arena* m_arena;
	int    m_row;
	int    m_col;
	bool   m_dead;
};

class Arena
{
public:
	// Constructor/destructor
	Arena(int nRows, int nCols);
	~Arena();

	// Accessors
	int     rows() const;
	int     cols() const;
	Player* player() const;
	int     ratCount() const;
	int     getCellStatus(int r, int c) const;
	int     numberOfRatsAt(int r, int c) const;
	void    display(string msg) const;

	// Mutators
	void setCellStatus(int r, int c, int status);
	bool addRat(int r, int c);
	bool addPlayer(int r, int c);
	void moveRats();

private:
	int     m_grid[MAXROWS][MAXCOLS];
	int     m_rows;
	int     m_cols;
	Player* m_player;
	Rat*    m_rats[MAXRATS];
	int     m_nRats;
	int     m_turns;

	// Helper functions
	void checkPos(int r, int c) const;
	bool isPosInBounds(int r, int c) const;
};

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

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int randInt(int min, int max);
bool decodeDirection(char ch, int& dir);
bool attemptMove(const Arena& a, int dir, int& r, int& c);
bool recommendMove(const Arena& a, int r, int c, int& bestDir);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
//  Rat implementation
///////////////////////////////////////////////////////////////////////////

Rat::Rat(Arena* ap, int r, int c)
{
	if (ap == nullptr)
	{
		cout << "***** A rat must be created in some Arena!" << endl;
		exit(1);
	}
	if (r < 1 || r > ap->rows() || c < 1 || c > ap->cols())
	{
		cout << "***** Rat created with invalid coordinates (" << r << ","
			<< c << ")!" << endl;
		exit(1);
	}
	m_arena = ap;
	m_row = r;
	m_col = c;
	m_health = 2;
	m_move = true;
}

int Rat::row() const
{
	return m_row;
}

int Rat::col() const
{
	return m_col;
}

bool Rat::isDead() const
{
	if (m_health == 0)
		return true;
	else
		return false;  
}

void Rat::move()
{
	if (m_health == 1)
		m_move = !m_move;

	if (m_move)
	{
		int m = randInt(0, 3);

		if (m == NORTH && m_row != 1)
			m_row--;
		if (m == EAST && m_col != m_arena->cols())
			m_col++;
		if (m == SOUTH && m_row != m_arena->rows())
			m_row++;
		if (m == WEST && m_col != 1)
			m_col--;
	}
	else
		return;

	if (m_arena->getCellStatus(m_row, m_col) == HAS_POISON)
	{
		m_health--;
		m_arena->setCellStatus(m_row, m_col, EMPTY);
	}
}

///////////////////////////////////////////////////////////////////////////
//  Player implementation
///////////////////////////////////////////////////////////////////////////

Player::Player(Arena* ap, int r, int c)
{
	if (ap == nullptr)
	{
		cout << "***** The player must be created in some Arena!" << endl;
		exit(1);
	}
	if (r < 1 || r > ap->rows() || c < 1 || c > ap->cols())
	{
		cout << "**** Player created with invalid coordinates (" << r
			<< "," << c << ")!" << endl;
		exit(1);
	}
	m_arena = ap;
	m_row = r;
	m_col = c;
	m_dead = false;
}

int Player::row() const
{
	return m_row;
}

int Player::col() const
{
	return m_col;
}

string Player::dropPoisonPellet()
{
	if (m_arena->getCellStatus(m_row, m_col) == HAS_POISON)
		return "There's already a poison pellet at this spot.";
	m_arena->setCellStatus(m_row, m_col, HAS_POISON);
	return "A poison pellet has been dropped.";
}

string Player::move(int dir)
{
	if (attemptMove(*m_arena, dir, m_row, m_col))
	{
		if (m_arena->numberOfRatsAt(row(), col()) != 0)
			{
				setDead();
				return "Player walked into a rat and died.";
			}
		if (dir == NORTH)
			return "Player moved north.";
		if (dir == EAST)
			return "Player moved east.";
		if (dir == SOUTH)
			return "Player moved south.";
		if (dir == WEST)
			return "Player moved west.";
	}
	
	return "Player couldn't move; player stands.";
}

bool Player::isDead() const
{
	return m_dead;
}

void Player::setDead()
{
	m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Arena implementation
///////////////////////////////////////////////////////////////////////////

Arena::Arena(int nRows, int nCols)
{
	if (nRows <= 0 || nCols <= 0 || nRows > MAXROWS || nCols > MAXCOLS)
	{
		cout << "***** Arena created with invalid size " << nRows << " by "
			<< nCols << "!" << endl;
		exit(1);
	}
	m_rows = nRows;
	m_cols = nCols;
	m_player = nullptr;
	m_nRats = 0;
	m_turns = 0;
	for (int x = 0; x < MAXRATS; x++)
		m_rats[x] = nullptr;
	for (int r = 1; r <= m_rows; r++)
		for (int c = 1; c <= m_cols; c++)
			setCellStatus(r, c, EMPTY);
}

Arena::~Arena()
{
	delete m_player;
	for (int x = 0; x < m_nRats; x++)
		delete m_rats[x];
}

int Arena::rows() const
{
	return m_rows;
}

int Arena::cols() const
{
	return m_cols;
}

Player* Arena::player() const
{
	return m_player;
}

int Arena::ratCount() const
{
	return m_nRats;
}

int Arena::getCellStatus(int r, int c) const
{
	//checkPos(r, c);
	return m_grid[r - 1][c - 1];
}

int Arena::numberOfRatsAt(int r, int c) const
{
	int b = 0;
	for (int x = 0; x < m_nRats; x++)
		if (m_rats[x]->row() == r && m_rats[x]->col() == c)
			b++;
	return b;
}

void Arena::display(string msg) const
{
	char displayGrid[MAXROWS][MAXCOLS];
	int r, c;

	// Fill displayGrid with dots (empty) and stars (poison pellets)
	for (r = 1; r <= rows(); r++)
		for (c = 1; c <= cols(); c++)
			displayGrid[r - 1][c - 1] = (getCellStatus(r, c) == EMPTY ? '.' : '*');

	// Indicate each rat's position	
	string digits = "0123456789";
	for (r = 1; r <= rows(); r++)
		for (c = 1; c <= cols(); c++)
		{
			if (numberOfRatsAt(r, c) == 1)
				displayGrid[r - 1][c - 1] = 'R';
			if (numberOfRatsAt(r, c) >= 2 && numberOfRatsAt(r, c) <= 9)
				displayGrid[r - 1][c - 1] = digits[numberOfRatsAt(r, c)];
			if (numberOfRatsAt(r, c) > 9)
				displayGrid[r - 1][c - 1] = '9';
		}

	// Indicate player's position
	if (m_player != nullptr)
		displayGrid[m_player->row() - 1][m_player->col() - 1] = (m_player->isDead() ? 'X' : '@');

	// Draw the grid
	clearScreen();
	for (r = 1; r <= rows(); r++)
	{
		for (c = 1; c <= cols(); c++)
			cout << displayGrid[r - 1][c - 1];
		cout << endl;
	}
	cout << endl;

	// Write message, rat, and player info
	if (msg != "")
		cout << msg << endl;
	cout << "There are " << ratCount() << " rats remaining." << endl;
	if (m_player == nullptr)
		cout << "There is no player!" << endl;
	else if (m_player->isDead())
		cout << "The player is dead." << endl;
	cout << m_turns << " turns have been taken." << endl;
}

void Arena::setCellStatus(int r, int c, int status)
{
	checkPos(r, c);
	m_grid[r - 1][c - 1] = status;
}

bool Arena::addRat(int r, int c)
{
	if (!isPosInBounds(r, c))
		return false;

	// Don't add a rat on a spot with a poison pellet
	if (getCellStatus(r, c) != EMPTY)
		return false;

	// Don't add a rat on a spot with a player
	if (m_player != nullptr  &&  m_player->row() == r  &&  m_player->col() == c)
		return false;

	// If there are MAXRATS existing rats, return false.
	//Otherwise,
	// dynamically allocate a new rat at coordinates (r,c).  Save the
	// pointer to the newly allocated rat and return true.
	if (m_nRats == MAXRATS)
		return false;
	else
	{
		m_rats[m_nRats] = new Rat(this, r, c);
		m_nRats++;
		return true;
	}
}

bool Arena::addPlayer(int r, int c)
{
	if (!isPosInBounds(r, c))
		return false;

	// Don't add a player if one already exists
	if (m_player != nullptr)
		return false;

	// Don't add a player on a spot with a rat
	if (numberOfRatsAt(r, c) > 0)
		return false;

	m_player = new Player(this, r, c);
	return true;
}

void Arena::moveRats()
{	
	for (int x = 0; x < m_nRats; x++)
	{
		m_rats[x]->move();

		if (m_player->row() == m_rats[x]->row() && m_player->col() == m_rats[x]->col())
			m_player->setDead();

		if (m_rats[x]->isDead())
		{
			delete m_rats[x];
			for (int y = x; y < (m_nRats - 1); y++)
			{m_rats[y] = m_rats[y + 1];}
			m_rats[m_nRats - 1] = nullptr;
			m_nRats--;
		}
	}

	// Another turn has been taken
	m_turns++;
}

bool Arena::isPosInBounds(int r, int c) const
{
	return (r >= 1 && r <= m_rows  &&  c >= 1 && c <= m_cols);
}

void Arena::checkPos(int r, int c) const
{
	if (r < 1 || r > m_rows || c < 1 || c > m_cols)
	{
		cout << "***** " << "Invalid arena position (" << r << ","
			<< c << ")" << endl;
		exit(1);
	}
}

///////////////////////////////////////////////////////////////////////////
//  Game implementation
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nRats)
{
	if (nRats < 0)
	{
		cout << "***** Cannot create Game with negative number of rats!" << endl;
		exit(1);
	}
	if (nRats > MAXRATS)
	{
		cout << "***** Trying to create Game with " << nRats
			<< " rats; only " << MAXRATS << " are allowed!" << endl;
		exit(1);
	}
	if (rows == 1 && cols == 1 && nRats > 0)
	{
		cout << "***** Cannot create Game with nowhere to place the rats!" << endl;
		exit(1);
	}

	// Create arena
	m_arena = new Arena(rows, cols);

	// Add player
	int rPlayer;
	int cPlayer;
	do
	{
		rPlayer = randInt(1, rows);
		cPlayer = randInt(1, cols);
	} while (m_arena->getCellStatus(rPlayer, cPlayer) != EMPTY);
	m_arena->addPlayer(rPlayer, cPlayer);

	// Populate with rats
	while (nRats > 0)
	{
		int r = randInt(1, rows);
		int c = randInt(1, cols);
		if (r == rPlayer && c == cPlayer)
			continue;
		m_arena->addRat(r, c);
		nRats--;
	}
}

Game::~Game()
{
	delete m_arena;
}

string Game::takePlayerTurn()
{
	for (;;)
	{
		cout << "Your move (n/e/s/w/x or nothing): ";
		string playerMove;
		getline(cin, playerMove);

		Player* player = m_arena->player();
		int dir;

		if (playerMove.size() == 0)
		{
			if (recommendMove(*m_arena, player->row(), player->col(), dir))
				return player->move(dir);
			else
				return player->dropPoisonPellet();
		}
		else if (playerMove.size() == 1)
		{
			if (tolower(playerMove[0]) == 'x')
				return player->dropPoisonPellet();
			else if (decodeDirection(playerMove[0], dir))
				return player->move(dir);
		}
		cout << "Player move must be nothing, or 1 character n/e/s/w/x." << endl;
	}
}

void Game::play()
{
	m_arena->display("");
	while (!m_arena->player()->isDead() && m_arena->ratCount() > 0)
	{
		string msg = takePlayerTurn();
		Player* player = m_arena->player();
		if (player->isDead())
		{
			cout << msg << endl;
			break;
		}
		m_arena->moveRats();
		m_arena->display(msg);
	}
	if (m_arena->player()->isDead())
		cout << "You lose." << endl;
	else
		cout << "You win." << endl;
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementation
///////////////////////////////////////////////////////////////////////////

// Return a uniformly distributed random int from min to max, inclusive
int randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

bool decodeDirection(char ch, int& dir)
{
	switch (tolower(ch))
	{
	default:  return false;
	case 'n': dir = NORTH; break;
	case 'e': dir = EAST;  break;
	case 's': dir = SOUTH; break;
	case 'w': dir = WEST;  break;
	}
	return true;
}

// Return false without changing anything if moving one step from (r,c)
// in the indicated direction would run off the edge of the arena.
// Otherwise, update r and c to the position resulting from the move and
// return true.
bool attemptMove(const Arena& a, int dir, int& r, int& c)
{
	if (dir == NORTH)
	{
		if (r == 1)
			return false;
		else
			r--;
	}
	if (dir == EAST)
	{
		if (c == a.cols())
			return false;
		else
			c++;
	}
	if (dir == SOUTH)
	{
		if (r == a.rows())
			return false;
		else
			r++;
	}
	if (dir == WEST)
	{
		if (c == 1)
			return false;
		else
			c--;
	}
	return true;

}

// Recommend a move for a player at (r,c):  A false return means the
// recommendation is that the player should drop a poison pellet and not
// move; otherwise, this function sets bestDir to the recommended
// direction to move and returns true.
bool recommendMove(const Arena& a, int r, int c, int& bestDir)
{
				   // Your replacement implementation should do something intelligent.
				   // You don't have to be any smarter than the following, although
				   // you can if you want to be:  If staying put runs the risk of a
				   // rat possibly moving onto the player's location when the rats
				   // move, yet moving in a particular direction puts the player in a
				   // position that is safe when the rats move, then the chosen
				   // action is to move to a safer location.  Similarly, if staying put
				   // is safe, but moving in certain directions puts the player in danger,
				   // then the chosen action should not be to move in one of the
				   // dangerous directions; instead, the player should stay put or move to
				   // another safe position.  In general, a position that may be moved to
				   // by many rats is more dangerous than one that may be moved to by
				   // few.
				   //
				   // Unless you want to, you do not have to take into account that a
				   // rat might be poisoned and thus sometimes less dangerous than one
				   // that is not.  That requires a more sophisticated analysis that
				   // we're not asking you to do.
	int m = 0;
	int n = 0;
	int e = 0;
	int s = 0;
	int w = 0;

	int g = r;
	int h = c;
	int i = r;
	int j = c;

	if (attemptMove(a, NORTH, g, h))
		m += 10 * (a.numberOfRatsAt(g, h));
	g = r;
	h = c;
	if (attemptMove(a, EAST, g, h))
		m += 10 * (a.numberOfRatsAt(g, h));
	g = r;
	h = c;
	if (attemptMove(a, SOUTH, g, h))
		m += 10 * (a.numberOfRatsAt(g, h));
	g = r;
	h = c;
	if (attemptMove(a, WEST, g, h))
		m += 10 * (a.numberOfRatsAt(g, h));
	g = r;
	h = c;
	if (a.getCellStatus(r, c) == HAS_POISON)
		m += 1;

	if (attemptMove(a, NORTH, g, h) && a.numberOfRatsAt(g, h) == 0)
	{
		i = g;
		j = h;
		if (attemptMove(a, NORTH, g, h))
			n += 10 * (a.numberOfRatsAt(g, h));
		else
			n += 1;
		g = i;
		h = j;
		if (attemptMove(a, EAST, g, h))
			n += 10 * (a.numberOfRatsAt(g, h));
		else
			n += 1;
		g = i;
		h = j;
		if (attemptMove(a, WEST, g, h))
			n += 10 * (a.numberOfRatsAt(g, h));
		else
			n += 1;
		g = i;
		h = j;
		if (a.getCellStatus(g, h) == HAS_POISON)
			n += 1;
	}
	else
		n = 20000;

	g = r;
	h = c;
	i = r;
	j = c;

	if (attemptMove(a, EAST, g, h) && a.numberOfRatsAt(g, h) == 0)
	{
		i = g;
		j = h;
		if (attemptMove(a, NORTH, g, h))
			e += 10 * (a.numberOfRatsAt(g, h));
		else
			e += 1;
		g = i;
		h = j;
		if (attemptMove(a, EAST, g, h))
			e += 10 * (a.numberOfRatsAt(g, h));
		else
			e += 1;
		g = i;
		h = j;
		if (attemptMove(a, SOUTH, g, h))
			e += 10 * (a.numberOfRatsAt(g, h));
		else
			e += 1;
		g = i;
		h = j;
		if (a.getCellStatus(g, h) == HAS_POISON)
			e += 1;
	}
	else
		e = 20000;

	g = r;
	h = c;
	i = r;
	j = c;
	
	if (attemptMove(a, SOUTH, g, h) && a.numberOfRatsAt(g, h) == 0)
	{
		i = g;
		j = h;
		if (attemptMove(a, EAST, g, h))
			s += 10 * (a.numberOfRatsAt(g, h));
		else
			s += 1;
		g = i;
		h = j;
		if (attemptMove(a, SOUTH, g, h))
			s += 10 * (a.numberOfRatsAt(g, h));
		else
			s += 1;
		g = i;
		h = j;
		if (attemptMove(a, WEST, g, h))
			s += 10 * (a.numberOfRatsAt(g, h));
		else
			s += 1;
		g = i;
		h = j;
		if (a.getCellStatus(g, h) == HAS_POISON)
			s += 1;
	}
	else
		s = 20000;

	g = r;
	h = c;
	i = r;
	j = c;

	if (attemptMove(a, WEST, g, h) && a.numberOfRatsAt(g, h) == 0)
	{
		i = g;
		j = h;
		if (attemptMove(a, NORTH, g, h))
			w += 10 * (a.numberOfRatsAt(g, h));
		else
			w += 1;
		g = i;
		h = j;
		if (attemptMove(a, SOUTH, g, h))
			w += 10 * (a.numberOfRatsAt(g, h));
		else
			w += 1;
		g = i;
		h = j;
		if (attemptMove(a, WEST, g, h))
			w += 10 * (a.numberOfRatsAt(g, h));
		else
			w += 1;
		g = i;
		h = j;
		if (a.getCellStatus(g, h) == HAS_POISON)
			w += 1;
	}
	else
		w = 20000;

	g = r;
	h = c;
	i = r;
	j = c;

	if (m <= n && m <= e && m <= s && m <= w)
	{
		if (m == n && a.getCellStatus(r, c) == HAS_POISON)
		{
			bestDir = NORTH;
			return true;
		}
		if (m == e && a.getCellStatus(r, c) == HAS_POISON)
		{
			bestDir = EAST;
			return true;
		}
		if (m == s && a.getCellStatus(r, c) == HAS_POISON)
		{
			bestDir = SOUTH;
			return true;
		}
		if (m == w && a.getCellStatus(r, c) == HAS_POISON)
		{
			bestDir = WEST;
			return true;
		}
		return false;
	}

	if (n < e && n < s && n < w)
	{
		bestDir = NORTH;
		return true;
	}

	if (e < n && e < s && e < w)
	{
		bestDir = EAST;
		return true;
	}

	if (s < n && s < e && s < w)
	{
		bestDir = SOUTH;
		return true;
	}

	if (w < n && w < e && w < s)
	{
		bestDir = WEST;
		return true;
	}

	if (n == e && n == s && n == w)
	{
		int x = randInt(1, 4);
		switch (x)
		{
		default: return false;
		case 1: bestDir = NORTH; break;
		case 2: bestDir = EAST; break;
		case 3: bestDir = SOUTH; break;
		case 4: bestDir = WEST; break;
		}
		return true;
	}

	if (n == e && n == s && n < w)
	{
		int x = randInt(1, 3);
		switch (x)
		{
		default: return false;
		case 1: bestDir = NORTH; break;
		case 2: bestDir = EAST; break;
		case 3: bestDir = SOUTH; break;
		}
		return true;
	}

	if (n == e && n == w && n < s)
	{
		int x = randInt(1, 3);
		switch (x)
		{
		default: return false;
		case 1: bestDir = NORTH; break;
		case 2: bestDir = EAST; break;
		case 3: bestDir = WEST; break;
		}
		return true;
	}

	if (n == s && n == w && n < e)
	{
		int x = randInt(1, 3);
		switch (x)
		{
		default: return false;
		case 1: bestDir = NORTH; break;
		case 2: bestDir = SOUTH; break;
		case 3: bestDir = WEST; break;
		}
		return true;
	}

	if (e == s && e == w && e < n)
	{
		int x = randInt(1, 3);
		switch (x)
		{
		default: return false;
		case 1: bestDir = EAST; break;
		case 2: bestDir = SOUTH; break;
		case 3: bestDir = WEST; break;
		}
		return true;
	}

	if (n == e && n < s && n < w)
	{
		int x = randInt(1, 2);
		switch (x)
		{
		default: return false;
		case 1: bestDir = NORTH; break;
		case 2: bestDir = EAST; break;
		}
		return true;
	}

	if (n == s && n < e && n < w)
	{
		int x = randInt(1, 2);
		switch (x)
		{
		default: return false;
		case 1: bestDir = NORTH; break;
		case 2: bestDir = SOUTH; break;
		}
		return true;
	}

	if (n == w && n < e && n < s)
	{
		int x = randInt(1, 2);
		switch (x)
		{
		default: return false;
		case 1: bestDir = NORTH; break;
		case 2: bestDir = WEST; break;
		}
		return true;
	}

	if (e == s && e < n && e < w)
	{
		int x = randInt(1, 2);
		switch (x)
		{
		default: return false;
		case 1: bestDir = EAST; break;
		case 2: bestDir = SOUTH; break;
		}
		return true;
	}

	if (e == w && e < n && e < s)
	{
		int x = randInt(1, 2);
		switch (x)
		{
		default: return false;
		case 1: bestDir = EAST; break;
		case 2: bestDir = WEST; break;
		}
		return true;
	}

	if (s == w && s < n && s < e)
	{
		int x = randInt(1, 2);
		switch (x)
		{
		default: return false;
		case 1: bestDir = SOUTH; break;
		case 2: bestDir = WEST; break;
		}
		return true;
	}
}

///////////////////////////////////////////////////////////////////////////
// main()
///////////////////////////////////////////////////////////////////////////

int main()
{
	// Create a game
	// Use this instead to create a mini-game:   Game g(3, 5, 2);
	Game g(10, 12, 40);

	// Play the game
	g.play();
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANY CODE BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++ UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _MSC_VER  //  Microsoft Visual C++

#pragma warning(disable : 4005)
#include <windows.h>

void clearScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	COORD upperLeft = { 0, 0 };
	DWORD dwCharsWritten;
	FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
		&dwCharsWritten);
	SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
	static const char* term = getenv("TERM");
	if (term == nullptr || strcmp(term, "dumb") == 0)
		cout << endl;
	else
	{
		static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
		cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
	}
}

#endif
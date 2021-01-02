#include <string>
#include <stack>
#include <iostream>
using namespace std;

class Coord
{
public:
	Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
	int r() const { return m_r; }
	int c() const { return m_c; }
private:
	int m_r;
	int m_c;
};

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
	stack<Coord> test;
	test.push(Coord(sr, sc));
	maze[sr][sc] = 'X';
	while (!test.empty())
	{
		Coord pos = test.top();
		test.pop();
		if (pos.r() == er && pos.c() == ec)
			return true;
		if (pos.r() >= 1 && maze[pos.r() - 1][pos.c()] == '.')
		{
			test.push(Coord(pos.r(), pos.c()));
			test.push(Coord(pos.r() - 1, pos.c()));
			maze[pos.r() - 1][pos.c()] = 'X';
		}
		else if ((pos.c() < (nCols - 1)) && maze[pos.r()][pos.c() + 1] == '.')
		{
			test.push(Coord(pos.r(), pos.c()));
			test.push(Coord(pos.r(), pos.c() + 1));
			maze[pos.r()][pos.c() + 1] = 'X';
		}
		else if ((pos.r() < (nRows - 1)) && maze[pos.r() + 1][pos.c()] == '.')
		{
			test.push(Coord(pos.r(), pos.c()));
			test.push(Coord(pos.r() + 1, pos.c()));
			maze[pos.r() + 1][pos.c()] = 'X';
		}
		else if (pos.c() >= 1 && maze[pos.r()][pos.c() - 1] == '.')
		{
			test.push(Coord(pos.r(), pos.c()));
			test.push(Coord(pos.r(), pos.c() - 1));
			maze[pos.r()][pos.c() - 1] = 'X';
		}
	}
	return false;
}

int main()
{
	string maze[10] = {
		"XXXXXXXXXX",
		"X........X",
		"XX.X.XXXXX",
		"X..X.X...X",
		"X..X...X.X",
		"XXXX.XXX.X",
		"X.X....XXX",
		"X..XX.XX.X",
		"X...X....X",
		"XXXXXXXXXX"
	};

	if (pathExists(maze, 10, 10, 6, 4, 1, 1))
		cout << "Solvable!" << endl;
	else
		cout << "Out of luck!" << endl;
}
#include <iostream>
#include <string>
using namespace std;

#include "globals.h"
#include "History.h"

string h_string = ".ABCDEFGHIJKLMNOPQRSTUVWXY";

History::History(int nRows, int nCols)
{
	h_rows = nRows;
	h_cols = nCols;
	for (int x = 0; x < nRows; x++)
		for (int y = 0; y < nCols; y++)
		{h_grid[x][y] = EMPTY; }
}

bool History::record(int r, int c)
{
	if (r <= 0 || c <= 0 || r > h_rows || c > h_cols)
	{return false;}
	else
	{
		h_grid[r - 1][c - 1]++;
		return true;
	}
}

void History::display() const
{
	clearScreen();
	for (int x = 0; x < h_rows; x++)
	{
		for (int y = 0; y < h_cols; y++)
		{
			if (h_grid[x][y] <= 25)
				cout << h_string[h_grid[x][y]];
			else
				cout << "Z";
		}
		cout << endl;
	}
	cout << endl;
}
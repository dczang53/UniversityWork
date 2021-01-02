#ifndef HISTORY_INCLUDED	
#define HISTORY_INCLUDED

#include "globals.h"

class History
{
public:
	History(int nRows, int nCols);
	bool record(int r, int c);
	void display() const;
private:
	int     h_grid[MAXROWS][MAXCOLS];
	int     h_rows;
	int     h_cols;
};

#endif //HISTORY_INCLUDED
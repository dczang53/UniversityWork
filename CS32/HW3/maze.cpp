bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
	if (sr == er && sc == ec && maze[sr][sc] == '.')
		return true;
	bool a = false, b = false, c = false, d = false;
	maze[sr][sc] = 'X';
	if (sr > 0 && maze[sr - 1][sc] == '.')
		a = pathExists(maze, nRows, nCols, sr - 1, sc, er, ec);
	if (sc < nCols - 1 && maze[sr][sc + 1] == '.')
		b = pathExists(maze, nRows, nCols, sr, sc + 1, er, ec);
	if (sr < nRows - 1 && maze[sr + 1][sc] == '.')
		c = pathExists(maze, nRows, nCols, sr + 1, sc, er, ec);
	if (sc > 0 && maze[sr][sc - 1] == '.')
		d = pathExists(maze, nRows, nCols, sr, sc - 1, er, ec);
	if (a || b || c || d)
		return true;
	else
		return false;

}
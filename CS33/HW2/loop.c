int loop(int x, int n)
{
	int result = 0;
	int mask;
	for (mask = 1; mask != 0; mask = (mask << (n & 0xFF))) {
		result |= (mask & x);
	}
	return result;
}


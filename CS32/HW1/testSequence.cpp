#include "Sequence.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	Sequence s;
	assert(s.empty());
	assert(s.find(42) == -1);
	s.insert(3);
	assert(s.size() == 1 && s.find(3) == 0);
	s.insert(0, 42);
	assert(s.size() == 2 && s.find(42) == 0 && s.find(3) == 1);
	s.set(1, 50);
	assert(s.size() == 2 && s.find(42) == 0 && s.find(3) == -1 && s.find(50) == 1);
	s.insert(46);
	for (int x = 0; x < s.size(); x++)
	{
		unsigned long n;
		s.get(x, n);
		cerr << n << endl;
	}
	cerr << "Passed all tests" << endl;
	Sequence t;
	t = s;
	for (int x = 0; x < t.size(); x++)
	{
		unsigned long n;
		t.get(x, n);
		cerr << n << endl;
	}
}
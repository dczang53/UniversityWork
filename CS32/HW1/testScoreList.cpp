#include "Sequence.h"
#include "ScoreList.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	ScoreList s;
	assert(s.size() == 0);
	assert(!s.remove(3));
	s.add(135);
	assert(s.size() == 0);
	s.add(1);
	assert(s.size() == 1 && s.minimum() == 1 && s.maximum() == 1);
	s.add(50);
	assert(s.size() == 2 && s.minimum() == 1 && s.maximum() == 50);
	s.add(99);
	s.remove(50);
	assert(s.size() == 2 && s.minimum() == 1 && s.maximum() == 99);
	cerr << "ALL TESTS PASSED" << endl;
	s.remove(1);
	s.add(100);
	
	/*
	for (int x = 0; x < s.ref()->size(); x++)
	{
		unsigned long n;
		s.ref()->get(x, n);
		cerr << n << endl;
	}
	ScoreList t = s;
	for (int x = 0; x < t.ref()->size(); x++)
	{
		unsigned long n;
		t.ref()->get(x, n);
		cerr << n << endl;
	}
	*/
}
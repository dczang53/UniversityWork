#include "newSequence.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	/*
	Sequence s;
	assert(s.empty());
	assert(s.find("vm") == -1);
	s.insert("hello");
	assert(s.size() == 1 && s.find("hello") == 0);
	s.insert(0, "3");
	assert(s.size() == 2 && s.find("3") == 0 && s.find("hello") == 1);
	s.set(1, "world");
	assert(s.size() == 2 && s.find("3") == 0 && s.find("hello") == -1 && s.find("world") == 1);
	s.insert("zip");
	for (int x = 0; x < s.size(); x++)
	{
		string n;
		s.get(x, n);
		cerr << n << endl;
	}
	cerr << "Passed all tests" << endl;
	Sequence t;
	t = s;
	for (int x = 0; x < t.size(); x++)
	{
		string n;
		t.get(x, n);
		cerr << n << endl;
	}
	*/

	Sequence a(1000);   // a can hold at most 1000 items
	Sequence b(5);      // b can hold at most 5 items
	Sequence c;         // c can hold at most DEFAULT_MAX_ITEMS items
	ItemType v = 9;

	// No failures inserting 5 items into b
	for (int k = 0; k < 5; k++)
		assert(b.insert(k, v));

	// Failure if we try to insert a sixth item into b
	assert(!b.insert(5, v));

	// When two Sequences' contents are swapped, their capacities are
	// swapped as well:
	a.swap(b);
	assert(!a.insert(5, v));
	for (int k = 0; k < 1000; k++)
		assert(b.insert(k, v));
	cerr << "SUCCESS" << endl;
}
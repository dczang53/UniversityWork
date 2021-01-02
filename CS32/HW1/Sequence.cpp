#include <string>

#include "Sequence.h"

	Sequence::Sequence()
	{
		s_size = 0;
	}// Create an empty sequence (i.e., one whose size() is 0).

	bool Sequence::empty() const
	{
		if (s_size == 0)
			return true;
		else
			return false;
	}// Return true if the sequence is empty, otherwise false.

	int Sequence::size() const
	{
		return s_size;
	}// Return the number of items in the sequence.

	bool Sequence::insert(int pos, const ItemType& value)
	{
		if (!(pos >= 0) || !(pos <= size()) || size() >= DEFAULT_MAX_ITEMS)
			return false;
		else
		{
			s_size++;
			for (int x = size() - 1; x > pos; x--)
				arr[x] = arr[x - 1];
			arr[pos] = value;
			return true;
		}
	}
	// Insert value into the sequence so that it becomes the item at
	// position pos.  The original item at position pos and those that
	// follow it end up at positions one higher than they were at before.
	// Return true if 0 <= pos <= size() and the value could be
	// inserted.  (It might not be, if the sequence has a fixed capacity,
	// (e.g., because it's implemented using a fixed-size array) and is
	// full.)  Otherwise, leave the sequence unchanged and return false.
	// Notice that if pos is equal to size(), the value is inserted at the
	// end.

	int Sequence::insert(const ItemType& value)
	{
		if (size() == DEFAULT_MAX_ITEMS)
			return -1;
		for (int x = 0; x < size(); x++)
		{
			if (value <= arr[x])
			{
				s_size++;
				for (int y = size() - 1; y > x; y--)
					arr[y] = arr[y - 1];
				arr[x] = value;
				return x;
			}
		}
		s_size++;
		arr[size() - 1] = value;
		return (size() - 1);
	}
	// Let p be the smallest integer such that value <= the item at
	// position p in the sequence; if no such item exists (i.e.,
	// value > all items in the sequence), let p be size().  Insert
	// value into the sequence so that it becomes the item at position
	// p.  The original item at position p and those that follow it end
	// up at positions one higher than before.  Return p if the value
	// was actually inserted.  Return -1 if the value was not inserted
	// (perhaps because the sequence has a fixed capacity and is full).

	bool Sequence::erase(int pos)
	{
		if (pos >= 0 && pos < size())
		{
			for (int x = pos; x < size() - 1; x++)
			{
				arr[x] = arr[x + 1];
			}
			s_size--;
			return true;
		}
		else
			return false;
	}
	// If 0 <= pos < size(), remove the item at position pos from
	// the sequence (so that all items that followed this item end up at
	// positions one lower than they were at before), and return true.
	// Otherwise, leave the sequence unchanged and return false.

	int Sequence::remove(const ItemType& value)
	{
		int c = 0;
		for (int x = 0; x < size(); x++)
		{
			if (arr[x] == value)
			{
				for (int y = x; y < size() - 1; y++)
				{
					arr[y] = arr[y + 1];
				}
				s_size--;
				c++;
			}
		}
		return c;
	}
	// Erase all items from the sequence that == value.  Return the
	// number of items removed (which will be 0 if no item == value).

	bool Sequence::get(int pos, ItemType& value) const
	{
		if (pos >= 0 && pos < size())
		{
			value = arr[pos];
			return true;
		}
		else
			return false;
	}
	// If 0 <= pos < size(), copy into value the item at position pos
	// in the sequence and return true.  Otherwise, leave value unchanged
	// and return false.

	bool Sequence::set(int pos, const ItemType& value)
	{
		if (pos >= 0 && pos < size())
		{
			arr[pos] = value;
			return true;
		}
		else
			return false;
	}
	// If 0 <= pos < size(), replace the item at position pos in the
	// sequence with value and return true.  Otherwise, leave the sequence
	// unchanged and return false.

	int Sequence::find(const ItemType& value) const
	{
		for (int x = 0; x < size(); x++)
		{
			if (arr[x] == value)
				return x;
		}
		return -1;
	}
	// Let p be the smallest integer such that value == the item at
	// position p in the sequence; if no such item exists, let p be -1.
	// Return p.

	void Sequence::swap(Sequence& other)
	{
		int a = size();
		s_size = other.size();
		other.s_size = a;
		ItemType b[DEFAULT_MAX_ITEMS];
		for (int x = 0; x < size(); x++)
			b[x] = arr[x];
		for (int y = 0; y < DEFAULT_MAX_ITEMS; y++)
			arr[y] = other.arr[y];
		for (int z = 0; z < DEFAULT_MAX_ITEMS; z++)
			other.arr[z] = b[z];
	}
	// Exchange the contents of this sequence with the other one.
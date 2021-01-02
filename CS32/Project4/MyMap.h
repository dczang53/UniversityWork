// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#ifndef MYMAP
#define MYMAP

#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap()
	 : root(nullptr), asc(0)
	{}
	~MyMap()
	{clear();}
	void clear()
	{
		delete root;
		root = nullptr;
		asc = 0;
	}
	int size() const
	{return asc;}
	void associate(const KeyType& key, const ValueType& value)
	{
		Node* p = root;
		while (p != nullptr)
		{
			if (key == p->n_key)
			{
				p->n_value = value;
				return;
			}
			else if (key < p->n_key)
			{
				if (p->n_less == nullptr)
				{
					p->n_less = new Node(key, value);
					asc++;
					return;
				}
				p = p->n_less;
			}
			else
			{
				if (p->n_greater == nullptr)
				{
					p->n_greater = new Node(key, value);
					asc++;
					return;
				}
				p = p->n_greater;
			}
		}
		if (root == nullptr)
		{
			root = new Node(key, value);
			asc++;
		}
	}

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const
	{
		Node* p = root;
		while (p != nullptr)
		{
			if (key == p->n_key)
				return &(p->n_value);
			else if (key < p->n_key)
				p = p->n_less;
			else
				p = p->n_greater;
		}
		return nullptr;
	}

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node
	{
		Node(KeyType a, ValueType b)
			: n_key(a), n_value(b), n_less(nullptr), n_greater(nullptr)
		{}
		~Node()
		{
			delete n_less;
			delete n_greater;
		}
		KeyType n_key;
		ValueType n_value;
		Node* n_less;
		Node* n_greater;
	};
	Node* root;
	int asc;
};

#endif //MYMAP
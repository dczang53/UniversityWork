#include "Sequence.h"

Sequence::Sequence()
{
	s_dummy = new Node;							//creates a "dummy node"
	s_size = 0;									//sets size to 0
}

Sequence::Sequence(const Sequence& scopy)
{
	s_size = scopy.size();						//set size to that of the copy
	s_dummy = new Node;							//create a new dummy node
	Node* ptra = s_dummy;						//create two place holding variables for the dummy nodes
	Node* ptrb = scopy.s_dummy;
	Node* nnode;								//placeholding variable for creating new nodes
	for (int x = 0; x < size(); x++)			//for each element in the copy...
	{
		ptrb = ptrb->next();					//access the xth term of the copy
		nnode = new Node;						//create a new node
		nnode->svalue(ptrb->nvalue());			//copy the value at position x to the new node
		nnode->snext(s_dummy);					//set the variables in the right places for the list
		nnode->sprev(ptra);
		ptra->snext(nnode);
		s_dummy->sprev(nnode);
		ptra = ptra->next();					//set the "next" placeholder to new node
	}
}

Sequence& Sequence::operator=(const Sequence& scopy)
{
	Node* sub;									//placeholder for creating/deleting new nodes
	Node* lnode = s_dummy->prev();				//create a variable set to the last variable
	if (scopy.size() > size())							//if there are less elements in the original sequence than the copy
	{
		for (s_size; s_size < scopy.size(); s_size++)	//for each element in the copy...
		{
			sub = new Node;								//create a new node
			sub->snext(s_dummy);						//set it as the last node
			sub->sprev(lnode);
			lnode->snext(sub);
			s_dummy->sprev(sub);
			lnode = sub;
		}
	}
	if (scopy.size() < size())							//if there are more elements in the original than the copy
	{
		for (s_size; s_size > scopy.size(); s_size--)	//for each extraneous node
		{
			sub = s_dummy->prev();						//delete the last node and set the second last as the last
			sub->prev()->snext(s_dummy);
			s_dummy->sprev(sub->prev());
			delete sub;
		}
	}
	Node* ptra = s_dummy;
	Node* ptrb = scopy.s_dummy;
	for (int n = 0; n < size(); n++)					//for each element...
	{
		ptra = ptra->next();							//set the copy's value in the nth position to the nth node of the sequence
		ptrb = ptrb->next();
		ptra->svalue(ptrb->nvalue());
	}
	if (empty())										//if the node is empty...
	{
		s_dummy->snext(nullptr);						//set the two pointers to mullptrs
		s_dummy->sprev(nullptr);
	}
	return *this;
}

Sequence::~Sequence()
{
	Node* ptra = s_dummy;								//create a variable to access the dummy variable
	Node* ptrb = ptra->prev();							//create a variable to access the last variable (if there is one)
	for (s_size; s_size > 0; s_size--)					//for each element in the sequence...
	{
		ptra = ptrb;
		ptrb = ptra->prev();							//record the next subsequent last node
		delete ptra;									//delete the last node
	}
	delete s_dummy;										//(when there are no more elements) delete the dummy node
}

bool Sequence::empty() const
{
	if (s_size == 0)									//if size counter is 0
		return true;									//return true
	else
		return false;									//else...return false
}

int Sequence::size() const
{
	return s_size;										//access the s_size private member
}

bool Sequence::insert(int pos, const ItemType& value)
{
	if (pos < 0 || pos > size())						//check if pos is legitimate
		return false;
	else
	{
		Node* ptr = s_dummy;							//(next 4 lines) access the value right before pos
		for (int x = 0; x < pos; x++)
		{
			ptr = ptr->next();
		}
		Node* nnode = new Node;							//create a new node
		nnode->svalue(value);							//set the value of the node to "value"
		if (pos == size())								//(set the node as the last element)
		{
			nnode->snext(s_dummy);
			nnode->sprev(ptr);
			ptr->snext(nnode);
			s_dummy->sprev(nnode);
		}
		if (pos < size())								//(set the node in pos p)
		{
			nnode->snext(ptr->next());
			nnode->sprev(ptr);
			ptr->next()->sprev(nnode);
			ptr->snext(nnode);
		}
		s_size++;										//increment the element counter
		return true;
	}

}

int Sequence::insert(const ItemType& value)
{
	Node* ptr = s_dummy;								//set a pointer to the dummy node
	int p;												//declare p
	for (p = 0; p < size(); p++)						//set p to 0; for each element
	{
		ptr = ptr->next();
		if (value <= ptr->nvalue())						//check each element for one that have a value >= "value"
			break;										//break out of loop and set p to that element's position
	}
	Node* nnode = new Node;								//create a new node
	nnode->svalue(value);								//set the value to "value"
	if (p == size())									//(same as the previous function)
	{
		nnode->snext(s_dummy);
		nnode->sprev(ptr);
		ptr->snext(nnode);
		s_dummy->sprev(nnode);
	}
	if (p < size())
	{
		nnode->snext(ptr);
		nnode->sprev(ptr->prev());
		ptr->prev()->snext(nnode);
		ptr->sprev(nnode);
	}
	s_size++;
	return p;
}

bool Sequence::erase(int pos)
{
	if (pos < 0 || pos >= size())						//check if pos is legitimate
		return false;
	else
	{
		Node* ptr = s_dummy;							//(next 4 lines) access the xth value
		for (int x = 0; x <= pos; x++)
		{
			ptr = ptr->next();
		}
		ptr->prev()->snext(ptr->next());				//manipulate the pointers so that the element in position pos is not pointed to
		ptr->next()->sprev(ptr->prev());
		delete ptr;										//delete the element at position p
		s_size--;										//decrement the size
		if (empty())									//if the sequence is empty, set the pointers to nullptr
		{
			s_dummy->snext(nullptr);
			s_dummy->sprev(nullptr);
		}
		return true;
	}
}

int Sequence::remove(const ItemType& value)
{
	int c = 0;											//declare a counter and initialize it to 0
	Node* ptr = s_dummy;								//create a pointer to point to the current node
	Node* dtpr;											//pointer for delete function
	for (int x = 0; x < size(); x++)					//for each element in the sequence...
	{
		ptr = ptr->next();								//access the xth element
		if (ptr->nvalue() == value)						//if the value pf the node is equal to the parameter, delete it
		{
			ptr->prev()->snext(ptr->next());
			ptr->next()->sprev(ptr->prev());
			dtpr = ptr;
			ptr = ptr->prev();
			delete dtpr;
			s_size--;
			x--;
			c++;
		}
	}
	if (empty())										//if the sequence is empty, set the pointers to nullptr
	{
		s_dummy->snext(nullptr);
		s_dummy->sprev(nullptr);
	}
	return c;
}

bool Sequence::get(int pos, ItemType& value) const
{
	if (pos < 0 || pos >= size())						//check if pos is legitimate
		return false;
	else
	{
		Node* ptr = s_dummy;							//(next 4 lines) access the xth value
		for (int x = 0; x <= pos; x++)
		{
			ptr = ptr->next();
		}
		value = ptr->nvalue();							//copy the value into the variable parameter
		return true;
	}
}

bool Sequence::set(int pos, const ItemType& value)
{
	if (pos < 0 || pos >= size())						//check if pos is legitimate
		return false;
	else
	{
		Node* ptr = s_dummy;							//(next 4 lines) access the xth value
		for (int x = 0; x <= pos; x++)
		{
			ptr = ptr->next();
		}
		ptr->svalue(value);								//change the value of the xth value to the given parameter
		return true;
	}
}

int Sequence::find(const ItemType& value) const
{
	Node* ptr = s_dummy;								//create a pointer pointing to the current element
	for (int x = 0; x < size(); x++)					//for each element
	{
		ptr = ptr->next();
		if (ptr->nvalue() == value)						//check if the value is the same as the parameter (return the position if it is)
		{
			return x;
		}
	}
	return -1;											//if no matches are found, return -1
}

void Sequence::swap(Sequence& other)
{
	Node* nsub = s_dummy;								//create a variable to record the address of the original dummy variable
	s_dummy = other.s_dummy;							//set the address of the original sequence to the other sequence
	other.s_dummy = nsub;								//set the address of the other sequence to the original
	int ssub = s_size;									//(same for size)
	s_size = other.s_size;
	other.s_size = ssub;
}



Sequence::Node::Node()
{
	n_next = nullptr;									//when a node is constructed, set its pointers to nullptr
	n_prev = nullptr;
}

ItemType Sequence::Node::nvalue() const
{
	return n_value;										//return the node's value
}

Sequence::Node* Sequence::Node::next() const
{
	return n_next;										//return the pointer to the next node
}

Sequence::Node* Sequence::Node::prev() const
{
	return n_prev;										//return the pointer to the previous node
}

void Sequence::Node::svalue(ItemType val)
{
	n_value = val;										//set the value of the node to the parameter
}

void Sequence::Node::snext(Node* n)
{
	n_next = n;											//set the pointer to the next node to the given parameter
}

void Sequence::Node::sprev(Node* p)
{
	n_prev = p;											//set the pointer to the previous node to the given parameter
}


int subsequence(const Sequence& seq1, const Sequence& seq2)
{
	if (seq2.size() == 0 || ((seq1.size() - seq2.size()) < 0))	//check if the seq2 is not empty and seq1 has more elements than seq2
		return -1;
	ItemType a;													//create two variables for the get function
	ItemType b;
	int match = 0;												//element match counter
	for (int x = 0; x < seq1.size(); x++)						//for each element in seq1...
	{
		seq1.get(x, a);
		seq2.get(match, b);
		if (a == b)												//access and compare the xth term of seq1 and the next matching term of seq2
			match++;
		if (a != b)												//if the terms match, increment the counter; if not, decrement the counter back to 0 and check the same element of seq1 again
		{
			if (match != 0)
				x--;
			match = 0;
		}
		if (match == seq2.size())								//if all elements in seq2 have been matched, return the first matching position
			return (x - seq2.size() + 1);
	}
	return -1;													//if no matches have been found, return -1
}

void interleave(const Sequence& seq1, const Sequence& seq2, Sequence& result)
{
	if (&seq1 == &result || &seq2 == &result)					//check for aliasing
		return;
	int pos;													//access the largest sequence size
	if (seq1.size() >= seq2.size())
		pos = seq1.size();
	else
		pos = seq2.size();

	Sequence combined;											//declare a new empty sequence
	ItemType a;
	int size = 0;												//declare a counter to access the last term of the sequence above
	for (int x = 0; x < pos; x++)								//for each element...(if applicable)
	{
		if (x < seq1.size())
		{
			seq1.get(x, a);										//append the xth value of seq1 to the end of the sequence
			combined.insert(size, a);
			size++;
		}
		if (x < seq2.size())									//append the xth value of seq2 to the end of the array
		{
			seq2.get(x, a);
			combined.insert(size, a);
			size++;
		}
	}
	result.swap(combined);										//swap the array created in this function with "result"
}
#ifndef SEQUENCE_INCLUDED
#define SEQUENCE_INCLUDED

#include <string>
typedef std::string ItemType;

class Sequence
{
public:
	Sequence();
	Sequence(const Sequence& scopy);
	Sequence& operator=(const Sequence& scopy);
	~Sequence();

	bool empty() const;
	int size() const;
	bool insert(int pos, const ItemType& value);
	int insert(const ItemType& value);
	bool erase(int pos);
	int remove(const ItemType& value);
	bool get(int pos, ItemType& value) const;
	bool set(int pos, const ItemType& value);
	int find(const ItemType& value) const;
	void swap(Sequence& other);
private:
	class Node
	{
	public:
		Node();

		ItemType nvalue() const;
		Node* next() const;
		Node* prev() const;

		void svalue(ItemType val);
		void snext(Node* n);
		void sprev(Node* p);

	private:
		ItemType n_value;
		Node* n_next;
		Node* n_prev;
	};
	Node* s_dummy;
	int s_size;
};

int subsequence(const Sequence& seq1, const Sequence& seq2);

void interleave(const Sequence& seq1, const Sequence& seq2, Sequence& result);

#endif	//SEQUENCE_INCLUDED
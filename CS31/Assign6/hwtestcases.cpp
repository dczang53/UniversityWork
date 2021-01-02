#include <cstring>

#include <iostream>
#include <string>
#include <cctype>
using namespace std;

void findMax(int arr[], int n, int*& pToMax)	//apparently pointers are default pass-by-value
{
	if (n <= 0)
		return;      // no items, no maximum!

	pToMax = arr;

	for (int i = 1; i < n; i++)
	{
		if (arr[i] > *pToMax)
			pToMax = arr + i;
	}
}																	//1B

void computeCube(int n, int* ncubed)
{
	*ncubed = n * n * n;
}																	//1C

// return true if two C strings are equal
bool strequal(const char str1[], const char str2[])
{
	while (*str1 != 0 && *str2 != 0)
	{
		if (*str1 != *str2)  // compare corresponding characters
			return false;
		str1++;            // advance to the next character
		str2++;
	}
	return *str1 == *str2;   // both ended at same time?			//1D
}

int* getPtrToArray(int& m)
{
	int anArray[100];
	for (int j = 0; j < 100; j++)
		anArray[j] = 100 - j;
	m = 100;
	return anArray;
}

void f()
{
	int junk[100];
	for (int k = 0; k < 100; k++)
		junk[k] = 123400000 + k;
}																	//1E

double mean(const double* scores, int numScores)
{
	double tot = 0;
	int x = 0;
	while (x != numScores)
	{
		tot += *(scores + x);
		x++;
	}
	return tot / numScores;
}																	//3A

/*const char* findTheChar(const char* str, char chr)
{
	for (int k = 0; *(str + k) != 0; k++)
		if (*(str + k) == chr)
			return (str + k);

	return nullptr;
}*/																	//3B

const char* findTheChar(const char* str, char chr)
{
	while (str != '\0')
	{
		if (*str == chr)
		{return str;}
		str++;
	}
	return nullptr;
}																	//3C

int* maxwell(int* a, int* b)
{
	if (*a > *b)
		return a;
	else
		return b;
}

void swap1(int* a, int* b)
{
	int* temp = a;
	a = b;
	b = temp;
}

void swap2(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}																	//4

void removeS(char* msg)
{
	char* c = msg;
	while (*c != 0)
	{
		if (*c == 'S' || *c == 's')
		{
			while (*c != 0)
			{
				*c = *(c + 1);
				c++;
			}
			c = msg;
		}
		c++;
	}
}																	//5

int main()
{
	/*int arr[3] = { 5, 10, 15 };
	int* ptr = arr;

	*ptr = 30;          // set arr[0] to 30
	*(ptr + 1) = 20;	// set arr[1] to 20
	ptr += 2;
	ptr[0] = 10;        // set arr[2] to 10

	while (ptr >= arr)
	{
		cout << *(arr + (2 - (ptr - arr))) << endl;    // print values
		ptr--;
	}*/																				//1A

	/*int nums[4] = { 5, 3, 15, 6 };
	int* ptr;

	findMax(nums, 4, ptr);
	cout << "The maximum is at address " << ptr << endl;
	cout << "It's at position " << ptr - nums << endl;
	cout << "Its value is " << *ptr << endl;*/										//1B

	/*int x;
	int* ptr = &x;					//pointer wasn't initialized to any variable address
	computeCube(5, ptr);
	cout << "Five cubed is " << *ptr << endl;*/										//1C

	/*char a[15] = "Zhao";
	char b[15] = "Zhao";

	if (strequal(a, b))
		cout << "They're the same person!\n";*/										//1D

	/*int n;
	int* ptr = getPtrToArray(n);
	f();
	for (int i = 0; i < 3; i++)
		cout << ptr[i] << ' ';			//NOT IN SCOPE
	for (int i = n - 3; i < n; i++)
		cout << ptr[i] << ' ';
	cout << endl;*/																	//1E

	/*double* cat;
	double mouse[5];
	cat = &mouse[4];
	*cat = 25;
	*(mouse + 3) = 42;
	cat -= 3;
	cat[1] = 54;
	cat[0] = 17;
	bool b = (*cat == cat[1]);
	//*&b = (*cat == cat[1]);
	bool d = (cat == mouse);
	cout << mouse[0] << endl << mouse[1] << endl << mouse[2] << endl << mouse[3] << endl << mouse[4] << endl;
	cout << b << endl;
	cout << d << endl;*/															//2

	/*double string[] = { 2, 4, 6, 8 };
	cout << mean(string, 4) << endl;*/

	char string[] = "HELLOYOUPIECEOFSHIT";
	cout << findTheChar(string, 'S') << endl;

	/*int array[6] = { 5, 3, 4, 17, 22, 19 };

	int* ptr = maxwell(array, &array[2]);
	*ptr = -1;
	ptr += 2;
	ptr[1] = 9;
	*(array + 1) = 79;

	cout << &array[5] - ptr << endl;

	swap1(&array[0], &array[1]);	//doesn't work...WTF...
	swap2(array, &array[2]);

	for (int i = 0; i < 6; i++)
		cout << array[i] << endl;*/													//4

	/* 4
	declare and initialize array of 6 int
	declare a pointer ptr to ant int and initialize it to the location of the larger of element 0 or 2 of the array [element 0]
	set the element ptr points to to -1 [element 0 is now -1]
	set the pointer two elements ahead [position 2]
	set the element after the ptr element to 9 [element 3 becomes 9]
	set the next element after the first to 79 [element 1 becomes 79]
	cout the difference in elements between the last element and the second [cout 3]
	swap the pointer locations of the first two elements [apparently this doesn't work]
	swap the elements of the "first" and third elements [elements 0 and 2 switch]
	cout all elements in order [cout 4, -1, 79, 9, 22, 19]
	5 3 4 17 22 19
	-1 3 4 17 22 19
	-1 3 4 9 22 19
	-1 79 4 9 22 19
	cout 3
	-1 79 4 9 22 19
	4 79 -1 9 22 19
	*/																				//4

	/*char msg[50] = "She'll be a massless princess.";
	removeS(msg);
	cout << msg;  // prints   he'll be a male prince.*/								//5

}
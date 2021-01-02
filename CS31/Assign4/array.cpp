#include <iostream>
#include <string>
using namespace std;

int appendToAll(string a[], int n, string value);
int lookup(const string a[], int n, string target);
int positionOfMax(const string a[], int n);
int rotateLeft(string a[], int n, int pos);
int countRuns(const string a[], int n);
int flip(string a[], int n);
int differ(const string a1[], int n1, const string a2[], int n2);
int subsequence(const string a1[], int n1, const string a2[], int n2);
int lookupAny(const string a1[], int n1, const string a2[], int n2);
int separate(string a[], int n, string separator);

int main()
{
	const int c = 9;
	//const int k = 2;
	string a[c] = {"tim", "ajamu", "mike", "fred", "donald", "tim", "jim", "timothy", "jimmy"};
	//string h[k] = {"tim", "jim"};
	string value;
	getline(cin, value);
	//int n;
	//cin >> n;
	//int p;
	//cin >> p;
	cout << separate(a, 0, value) << endl;
	for (int x = 0; x < 9; x++)
	{
		cout << a[x] << endl;
	}
}

int appendToAll(string a[], int n, string value)
{
	if (n < 0)													//checks if array size is at least 0
	{return -1;}
	else
	{															//for loop for every array element
		for(int x = 0; x < n; x++)
		{a[x] += value;}										//appends string "value" to each element
		return n;												//returns number of elements in array
	}
}

int lookup(const string a[], int n, string target)
{
	if (n < 0)													//checks if array size is at least 0
	{return -1;}
	for (int x = 0; x < n; x++)									//for loop for each array element
	{
		if (a[x] == target)										//checks if particular element is equal to "target"
		{return x;}												//if so, return position number (only for first matching element)
	}
	return -1;													//if no matches were found (and the loop actually finishes), return -1 for no match
}

int positionOfMax(const string a[], int n)
{
	if (n <= 0)													//checks if size is positive and non-zero
	{return -1;}
	string s = a[0];											//"assumes" greatest string of the array is the first
	int b = 0;													//"records" position 0 (for now, before any comparizon)
	for (int x = 1; x < n; x++)									//for loop for each array element
	{
		if (a[x] > s)											//checks each element if it is greater than the current max
		{														//if so, record the new max string and position
			s = a[x];
			b = x;
		}
	}
	return b;													//after the loop ends, return the recorded position
}

int rotateLeft(string a[], int n, int pos)
{
	if (n < 0)													//checks if number of elements in not less than 0
	{return -1;}
	if (pos < 0 || pos >= n)									//makes sure that "pos" is a legitimate value (in the given range of 0 to n - 1)
	{return -1;}
	string s = a[pos];											//"store away" the string in position "pos"
	for (int x = pos; x < (n - 1); x++)							//for loop for each array element at and after position "pos"
	{a[x] = a[x + 1];}											//each string in each position will take on the value of that after itself (except for the last string)
	a[n - 1] = s;												//the last string takes on the value stored as string "s"
	return pos;													//return "pos"
}

int countRuns(const string a[], int n)
{
	if (n < 0)													//checks if the array size is not negative
	{return -1;}
	if (n == 0)													//if the array is "empty", return 0 for 0 runs
	{return 0;}
	string s = a[0];											//store the first string in string "s"
	int b = 1;													//declare and assign the value 1 to counter "b"
	for (int x = 1; x < n; x++)									//for loop for each array element (after the first)
	{
		if (a[x] != s)											//checks if the value at position x is equal to the stored string "s"
		{
			b++;												//if not, increment the counter by 1
			s = a[x];											//and store the new value as string "s"
		}
	}
	return b;													//after the loop finishes, return the value of the counter
}

int flip(string a[], int n)
{
	if (n < 0)													//check if the array size is nonnegative
	{return -1;}
	string s;													//create string "s" to store strings in the upcoming loop
	for (int x = 0; x < (n / 2); x++)							//for loop for the first half of the array
	{
		s = a[x];												//swap values for each array element pair (first and last, second and second last, etc..)
		a[x] = a[n - x - 1];									//by storing the first element in string "s",
		a[n - x - 1] = s;										//assigning the latter element's value in the first, and then assigning the first value (stored in s) to the last string element
	}
	return n;													//return the array size
}

int differ(const string a1[], int n1, const string a2[], int n2)
{
	if (n1 < 0 || n2 < 0)										//checks if either array size is negative
	{return -1;}
	for (int x = 0; x < n1; x++)								//for loop for each element of array "a1"
	{
		if (x == n2)											//if x has reached n2 length and has no differing elements up to now, return n2
		{return n2;}
		if (a1[x] != a2[x])										//if the elements in position x of arrays a1 and a2 are different, return the position x
		{return x;}
	}
	return n1;													//return n1 (given that no mismatches where found and the elements in "a1" [and possibly "a2"] have run out)
}

int subsequence(const string a1[], int n1, const string a2[], int n2)
{
	if (n1 < 0 || n2 < 0)										//checks if the lengths of either of the two arrays are negative
	{return -1;}
	if (n2 == 0)												//if subsequence if empty, it is present in any array
	{return 0;}
	int b = 0;													//declare and initialize int "b" to 0 (the counter for matching subsequence elements)
	for (int x = 0; x < n1; x++)
	{
		if (a1[x] != a2[b])										//if the corresponding elements don't match, reset the counter to 0
		{b = 0;}
		if (a1[x] == a2[b])										//if the corresponding elements match, increment the counter by 1
		{b++;}
		if (b == n2)											//when the counter of matching elements reaches the length of the subsequence, return the position where the subsequence begins
		{return (x + 1 - b);}
	}
	return -1;													//if the loops ends without finding a subsequence, return -1 for no match
}

int lookupAny(const string a1[], int n1, const string a2[], int n2)
{
	if (n1 <= 0 || n2 <= 0)										//checks if the sizes of "a1" and "a2" are greater than 0
	{return -1;}
	for (int x = 0; x < n1; x++)								//for loop for each element in array "a1"
	{
		for (int y = 0; y < n2; y++)							//for loop for each element in array "a2"
		{
			if (a1[x] == a2[y])									//checks if the element in position x of "a1" matches any of the elements in "a2"
			{return x;}
		}
	}
	return -1;													//if the loop is completed without findaing any matches, return -1
}

int separate(string a[], int n, string separator)
{
	if (n < 0)													//checks if the size of "a2" is not negative
	{return -1;}
	int b = 0;													//declare and initialize "rotateleft counter" "b" to 0
	int c = 0;													//declare and initialize "2nd rotateleft counter" "c" to 0
	string s;													//declare empty "storage string" "s"
	for (int x = 0; x < n; x++)									//for loop for each element in array "a"
	{
		if (a[x] >= separator)									//checks if specific string is greater than/equal to string "separator"
		{
			b++;												//increment "b" by 1
			s = a[x];											//store the specific string in "s"
			for (int y = x; y < (n - 1); y++)					//for loop for each element in array "a" at and after position x to n - 2
			{a[y] = a[y + 1];}									//shift each element one spot to the left
			a[n - 1] = s;										//assign the value of the stored string to the last string in the array
			x -= 1;												//after the shift, decrement the counter by 1
		}
		if ((x + b) >= (n - 1))									//if the value of the "total counter" (number of rotateLefts and position number) becomes greater than/equal to one less than the total length, break out of the loop (can just use equal to, but greater than/equal to has greater safety)
		{break;}
	}
	for (int z = (n - b); z < n; z++)							//second for loop for the elements greater than/equal to "separator" in the latter portion of the modified array
	{
		c++;													//increment "c" by 1
		if (a[z] > separator)									//checks if specific string is greater than "separator"
		{
			s = a[z];											//store the current string in "s"
			for (int q = z; q < (n - 1); q++)					//for loop for the current and following positions (besides the last)
			{a[q] = a[q + 1];}									//shift each value one over to the left
			a[n - 1] = s;										//store the stored value in "s" to the last string
			z -= 1;												//after the shift, decrement the counter by 1
		}
		if (c >= b)												//if the loop has repeated for the same number of (or greater than [won't happen]) "b", break out of the loop
		{break;}
	}
	return (n - b);											//return the first position in which the string is greater than "separator"
}
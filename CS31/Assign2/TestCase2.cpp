#include <iostream>
using namespace std;

int main()
{
	int side;

	cout << "Enter a number: ";
	cin >> side;

	for (int i = 0; i < side; i++)
	{
		int n = 0;
			while (n <= i)
			{
				cout << "#";
				n++;
			}
		cout << "\n";
	}
}
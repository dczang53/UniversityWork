#include <iostream>
using namespace std;

int main()
{
	int side;

	cout << "Enter a number: ";
	cin >> side;

	int i = 0;

	if (side <= 0)
		return 0;
	else
	{
		do
		{
			int n = 0;
			while (n <= i)
			{
				cout << "#";
				n++;
			}
			cout << "\n";
			i++;
		} while (i < side);
	}
}
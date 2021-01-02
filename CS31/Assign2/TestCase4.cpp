#include <iostream>
using namespace std;

int main() {
	cout << "Enter the code: ";
	int codeSection;
	cin >> codeSection;

	switch (codeSection)
	{
		case 281:
			cout << "bigamy" << endl;
			return 0;
		case 321:
		case 322:
			cout << "selling illegal lottery tickets" << endl;
			return 0;
		case 383:
			cout << "selling rancid butter" << endl;
			return 0;
		case 598:
			cout << "wounding a bird in a public cemetery" << endl;
			return 0;
		default:
			cout << "some other crime" << endl;
	}

}
#include <iostream>
#include <string>
#include <cctype>
using namespace std;

bool hasCorrectSyntax(string pollData);
int countVotes(string pollData, char party, int& voteCount);

int main()
{
	string pollData;
	char party;
	int votes = 0;

	getline(cin, pollData);
	cin >> party;

	cout << countVotes(pollData, party, votes) << endl;
	cout << votes;
}



bool hasCorrectSyntax(string pollData)												//checks if string has correct syntax
{
	pollData += '0';																//adds a zero to the end of the string (for checking letter sequence lengths later in the function)
	int alph = 0;																	//letter sequence length initialized at zero
	int numLength = 1;																//number sequence length initialized at one (bc we're checking these sequence lengths after the second term)
	const string codes = "AL.AK.AZ.AR.CA.CO.CT.DE.DC.FL.GA.HI.ID.IL.IN.IA.KS.KY.LA.ME.MD.MA.MI.MN.MS.MO.MT.NE.NV.NH.NJ.NM.NY.NC.ND.OH.OK.OR.PA.RI.SC.SD.TN.TX.UT.VT.VA.WA.WV.WI.WY";
																					//"storage" of viable state names
	for (int b = 0; b < pollData.size(); b++)										//capitalizes all letters in the input pollData string
	{pollData[b] = toupper(pollData[b]);}

	if (pollData.size() < 5 && pollData.size() != 1)								//the minimum size for the modified string is 5 characters (unless it's empty)
	{return false;}
	if (pollData.size() == 1)
	{return true;}

	if (!isalpha(pollData[0]) || !isalpha(pollData[1]) || !isdigit(pollData[2]))	//tests to see if the first two characters are numbers
	{return false;}
	if (codes.find(pollData.substr(0,2)) == string::npos)							//tests to see if the first two letter substring is a valide state name
	{return false;}
	
	if (!isalpha(pollData[pollData.size() - 2]))
	{return false;}

	else
	{
		for (int x = 3; x < pollData.size(); x++)									//{tests for all characters starting with the 4th)
		{
			if (!isdigit(pollData[x]) && !isalpha(pollData[x]))						//checks to see if each character is either a letter or a digit
			{return false;}

			if (isalpha(pollData[x]))												//(if character is a letter...)
			{
				if (numLength != 1 && numLength != 2 && isdigit(pollData[x - 1]))	//conditional checks if each number is only one or two digits
				{return false;}
				numLength = 0;														//number length counter is reset to zero after hitting a letter
				alph++;																//letter string length counter is incremented by 1 when letter is hit
			}

			if (isdigit(pollData[x]))												//(if character is a digit...)
			{
				if (alph != 3 && alph != 1 && alph != 0)							//checks to see if each letter string (besides the first) is 3 letters long (and not the last letter string) or 1 letter long (only for the last letter string)
				{return false;}
				if (alph == 1 && x != (pollData.size() - 1))						
				{return false;}
				if (alph == 3 && x == (pollData.size() - 1))
				{return false;}

				if (alph == 3)														//checks to see if each state code in the letter string is valid
				{
					if (codes.find(pollData.substr(x - 2, 2)) == string::npos)
					{return false;}
				}

				numLength++;														//munber length counter increments by 1 after hitting a number
				alph = 0;															//letter string length counter is reset to 0 after hitting a number
			}
		}
		return true;																//if all the tests above are satisfied, the string is valid
	}
}



int countVotes(string pollData, char party, int& voteCount)
{
	string digits = "0123456789";													//creates a string for the conversion of characters to integers
	int currentCount = 0;															//current number value
	int totalCount = 0;																//total number of votes for the input party

	if (!hasCorrectSyntax(pollData))												//returns 1 for invalid poll string
	{return 1;}
	if (!isalpha(party))															//returns 3 for invalid party character input
	{return 3;}
	else
	{
		party = toupper(party);														//convert the party input to an uppercase letter

		for (int k = 2; k < pollData.size(); k++)									//(starts checking numbers starting from the third character)
		{
			if (isdigit(pollData[k]))												//(if the character is a digit...)
			{
				for (int m = 0; m <= 9; m++)										//converts the character into an integer
				{
					if (pollData[k] == digits[m])
					{currentCount = (10 * currentCount) + m;}
				}
			}

			if (isalpha(pollData[k]))												//(if the character is a letter)
			{
				pollData[k] = toupper(pollData[k]);									//converts letter into uppercase
				if (currentCount == 0 && isdigit(pollData[k - 1]))					//if the number string is zero, return 2
				{return 2;}
				if (pollData[k] == party)											//if the party character matches with the input, add the current count into the total
				{totalCount += currentCount;}
				currentCount = 0;													//reset the number counter to zero upon hitting the letter, after the current count is(isn't) stored
			}
		}
		voteCount = totalCount;														//if no errors occur, assign the total to the variable for party votes
		return 0;																	//returns 0 for the function if no errors occur
	}
}

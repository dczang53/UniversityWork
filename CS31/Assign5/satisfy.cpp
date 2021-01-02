#include <cstring>
#include <iostream>
#include <cctype>
#include <cassert>
using namespace std;

const int MAX_WORD_LENGTH = 20;

int normalizeRules(char word1[][MAX_WORD_LENGTH + 1], char word2[][MAX_WORD_LENGTH + 1], int distance[], int nRules);
int calculateSatisfaction(const char word1[][MAX_WORD_LENGTH + 1], const char word2[][MAX_WORD_LENGTH + 1], const int distance[], int nRules, const char document[]);

int main()
{
	char a[][MAX_WORD_LENGTH + 1] = {"mad", "deranged", "", "Hello", "NEFARIOUS", "half-witted", "robot", "plot", "have"};
	char b[][MAX_WORD_LENGTH + 1] = {"scientist", "robot", "", "hello", "PLOT", "assistant", "deranged", "Nefarious", "mad"};
	int c[] = {2, 4, 1, 3, 2, 1, 13, 5, 9};
	char cstring[] = "ma34D hello SciEntiSt Robot     deranged plOT he-llo MAD   HELLO34 hello 508# hello nefARious 23 Have";
	cout << normalizeRules(a, b, c, 9) << endl;
	cout << a[0] << endl << a[1] << endl << a[2] << endl << a[3] << endl << a[4] << endl << a[5] << endl << a[6] << endl << a[7] << endl << a[8] << endl;
	cout << endl << endl << endl;
	cout << b[0] << endl << b[1] << endl << b[2] << endl << b[3] << endl << b[4] << endl << b[5] << endl << b[6] << endl << b[7] << endl << b[8] << endl;
	cout << endl << endl << endl;
	cout << c[0] << endl << c[1] << endl << c[2] << endl << c[3] << endl << c[4] << endl << c[5] << endl << c[6] << endl << c[7] << endl << c[8] << endl;
	cout << endl << endl << endl;
	cout << calculateSatisfaction(a, b, c, 4, cstring) << endl;

	//cout << "HELLO THERE" << endl;

	/*const int TEST1_NRULES = 4;
	char test1w1[TEST1_NRULES][MAX_WORD_LENGTH + 1] = {
		"mad",       "deranged", "nefarious", "have"
	};
	char test1w2[TEST1_NRULES][MAX_WORD_LENGTH + 1] = {
		"scientist", "robot",    "plot",      "mad"
	};
	int test1dist[TEST1_NRULES] = {
		2,           4,          1,           13
	};
	assert(calculateSatisfaction(test1w1, test1w2, test1dist, TEST1_NRULES,
		"The mad UCLA scientist unleashed a deranged evil giant robot.") == 2);
	assert(calculateSatisfaction(test1w1, test1w2, test1dist, TEST1_NRULES,
		"The mad UCLA scientist unleashed    a deranged robot.") == 2);
	assert(calculateSatisfaction(test1w1, test1w2, test1dist, TEST1_NRULES,
		"**** 2016 ****") == 0);
	assert(calculateSatisfaction(test1w1, test1w2, test1dist, TEST1_NRULES,
		"  That plot: NEFARIOUS!") == 1);
	assert(calculateSatisfaction(test1w1, test1w2, test1dist, TEST1_NRULES,
		"deranged deranged robot deranged robot robot") == 1);
	assert(calculateSatisfaction(test1w1, test1w2, test1dist, TEST1_NRULES,
		"That scientist said two mad scientists suffer from deranged-robot fever.") == 0);
	cout << "All tests succeeded" << endl;*/
}

int normalizeRules(char word1[][MAX_WORD_LENGTH + 1], char word2[][MAX_WORD_LENGTH + 1], int distance[], int nRules)
{
	if (nRules < 0)													//sets nRules to 0 if it is negative (just in case)
	{nRules = 0;}
	int z = 0;														//error counter for error indication (0 if no error, non-0 if error in rule set)
	for (int q = 0; q < nRules; q++)								//for-loop for each rule set
	{
		if (strlen(word1[q]) == 0)									//checks if the c-string is empty (and increment the counter if it is)
		{z++;}
		for (int w = 0; w < strlen(word1[q]); w++)					//for-loop to check if each character in word1 is a letter (if not, increment the error counter) and lowercase every letter
		{
			if (!isalpha(word1[q][w]))
			{z++;}
			word1[q][w] = tolower(word1[q][w]);
		}
		if (strlen(word2[q]) == 0)									//for-loop to check if each character in word2 is a letter (if not, increment the error counter) and lowercase every letter
		{z++;}
		for (int e = 0; e < strlen(word2[q]); e++)
		{
			if (!isalpha(word2[q][e]))
			{z++;}
			word2[q][e] = tolower(word2[q][e]);
		}
		if (distance[q] <= 0)											//checks if the distance for the rule set is non-0 (increment the error counter if not)
		{z++;}
		if (z != 0)													//if there are any errors in the rule set, delete the whole set and shift each array to the left one unit (overwriting the rule set with the error(s))
		{
			for (int r = q; r < (nRules - 1); r++)
			{strcpy(word1[r], word1[r + 1]);}
			strcpy(word1[nRules - 1], "");
			for (int t = q; t < (nRules - 1); t++)
			{strcpy(word2[t], word2[t + 1]);}
			strcpy(word2[nRules - 1], "");
			for (int y = q; y < (nRules - 1); y++)
			{distance[y] = distance[y + 1];}
			distance[nRules - 1] = 0;
			nRules--;												//decrement the total number of rules by 1
			q--;													//decrement the counter for the loop by one
		}
		z = 0;														//reset the error counter to 0 at the end of each loop
	}
	for (int x = 0; x < (nRules - 1); x++)							//for-loop for the remaining rule sets (besides the last)
	{
		for (int c = (x + 1); c < nRules; c++)						//nested for-loop to check every rule set after the designated rule set (x))
		{
			if ((strcmp(word1[x], word1[c]) == 0 && strcmp(word2[x], word2[c]) == 0) || (strcmp(word1[x], word2[c]) == 0 && strcmp(word2[x], word1[c]) == 0))
			{
				if (distance[x] < distance[c])						//first checks if a pair of rule sets have the same words, then set the first distance value as the larger number (if it isn't)
				{distance[x] = distance[c];}
				for (int u = c; u < (nRules - 1); u++)				//basically, from line 78 to line 86, delete rule set that occurs second
				{strcpy(word1[u], word1[u + 1]);}
				strcpy(word1[nRules - 1], "");
				for (int i = c; i < (nRules - 1); i++)
				{strcpy(word2[i], word2[i + 1]);}
				strcpy(word2[nRules - 1], "");
				for (int o = c; o < (nRules - 1); o++)
				{distance[o] = distance[o + 1];}
				distance[nRules - 1] = 0;
				nRules--;											//decrement the total number of rules by 1
				c--;												//decrement the counter of the loop by 1
			}
		}
	}
	return nRules;													//after all of the modifications, return the number of rule sets remaining
}

int calculateSatisfaction(const char word1[][MAX_WORD_LENGTH + 1], const char word2[][MAX_WORD_LENGTH + 1], const int distance[], int nRules, const char document[])
{
	if (nRules < 0)														//sets nRules to 0 if it is negative (just in case)
	{nRules = 0;}
																		//declare and initialize various counters
	int satisfy = 0;													//(counter for number of rule sets satisfied)
	int cmatcha = 0;													//(counter for number of characters that match those of word1)
	int cmatchb = 0;													//(counter for number of characters that match those of word2 )
	int space = 0;														//(counter for number of words traversed after first match is found)
	int wmatch = 0;														//(counter for number and type of words found so far)

	char documentCopy[201];												//create a copy of the input c-string
	strcpy(documentCopy, document);

	for (int q = 0; q < strlen(documentCopy); q++)						//for-loop for each character for the copy c-string
	{
		documentCopy[q] = tolower(documentCopy[q]);						//convert each letter to its lowercase counterpart
		if (!isalpha(documentCopy[q]) && (documentCopy[q] != ' '))		//if the specified element in the copy c-string is not a letter or a space character, remove it
		{
			for (int w = q; w < strlen(documentCopy); w++)
			{documentCopy[w] = documentCopy[w + 1];}					//removal by overwriting the values one unit to the left
			q--;														//decrement the loop counter by 1
		}
	}
	for (int e = 0; e < strlen(documentCopy); e++)						//for-loop for each character in the modified c-string
	{
		if (documentCopy[e] == ' ' && documentCopy[e + 1] == ' ')		//removes space characters so that there are only single spaces between words
		{
			for (int r = e; r < strlen(documentCopy); r++)
			{documentCopy[r] = documentCopy[r + 1];}					//removal by overwriting the values one unit to the left
			e--;														//decrement the loop counter by 1
		}
	}

	for (int t = 0; t < nRules; t++)									//for-loop for each rule set
	{
		for (int y = 0; y < strlen(documentCopy); y++)					//for-loop for each character in the modified c-string
		{
			if (y == 0)													//every time a new rule set is being analyzed (analyzing the first character), reset all of the following counters to 0
			{
				cmatcha = 0;
				cmatchb = 0;
				space = 0;
				wmatch = 0;
			}

			if ((y == 0) || ((y != 0) && documentCopy[y - 1] == ' '))	//if-conditional for the start of each word (checks will only be done if the loop reaches the start of a word)
			{
				if (wmatch == 1)										//if-conditional if a match for word1 has already benn found
				{
					space++;											//increments the "words traversed" counter by 1

					for (int i = 0; i < strlen(word2[t]); i++)			//for-loop to identify match for word2
					{
						if ((y + i) >= strlen(documentCopy))			//breaks out of counting loop before a potential out-of-bound value for the c-string is called
						{break;}
						if (documentCopy[y + i] == word2[t][i])			//char-by-char match count
						{cmatchb++;}
					}
					if (cmatchb == strlen(word2[t]) && (documentCopy[y + strlen(word2[t])] == ' ' || documentCopy[y + strlen(word2[t])] == '\0'))
					{wmatch += 2;}										//sets total for "word match" counter to 3 for both words found
					cmatchb = 0;										//reset the "word2 char match" counter back to 0

					for (int u = 0; u < strlen(word1[t]); u++)			//for-loop to identify match for word1
					{
						if ((y + u) >= strlen(documentCopy))			//breaks out of counting loop before a potential out-of-bound value for the c-string is called
						{break;}
						if (documentCopy[y + u] == word1[t][u])			//char-by-char match count
						{cmatcha++;}
					}
					if (cmatcha == strlen(word1[t]) && (documentCopy[y + strlen(word1[t])] == ' ' || documentCopy[y + strlen(word1[t])] == '\0'))
					{space = 0;}										//resets "words traversed" counter back to zero if second match for same word is found
					cmatcha = 0;										//reset the "word1 char match" counter back to 0
				}

				if (wmatch == 2)										//basically a mirror image of the above conditional (if (wmatch == 1)), except word1 and word2 are reversed (see above)
				{
					space++;

					for (int u = 0; u < strlen(word1[t]); u++)
					{
						if ((y + u) >= strlen(documentCopy))
						{break;}
						if (documentCopy[y + u] == word1[t][u])
						{cmatcha++;}
					}
					if (cmatcha == strlen(word1[t]) && (documentCopy[y + strlen(word1[t])] == ' ' || documentCopy[y + strlen(word1[t])] == '\0'))
					{wmatch += 1;}
					cmatcha = 0;

					for (int i = 0; i < strlen(word2[t]); i++)
					{
						if ((y + i) >= strlen(documentCopy))
						{break;}
						if (documentCopy[y + i] == word2[t][i])
						{cmatchb++;}
					}
					if (cmatchb == strlen(word2[t]) && (documentCopy[y + strlen(word2[t])] == ' ' || documentCopy[y + strlen(word2[t])] == '\0'))
					{space = 0;}
					cmatchb = 0;
				}

				if (wmatch == 0)										//if-conditional if no words have been found to match yet
				{
					for (int u = 0; u < strlen(word1[t]); u++)			//for-loop to identify match for word1
					{
						if ((y + u) >= strlen(documentCopy))
						{break;}
						if (documentCopy[y + u] == word1[t][u])
						{cmatcha++;}
					}
					for (int i = 0; i < strlen(word2[t]); i++)			//for-loop to identify match for word2
					{
						if ((y + i) >= strlen(documentCopy))
						{break;}
						if (documentCopy[y + i] == word2[t][i])
						{cmatchb++;}
					}
					if (cmatcha == strlen(word1[t]) && (documentCopy[y + strlen(word1[t])] == ' ' || documentCopy[y + strlen(word1[t])] == '\0'))
					{wmatch = 1;}										//if word has been found to match word1, increment the "word match" counter by 1
					if (cmatchb == strlen(word2[t]) && (documentCopy[y + strlen(word2[t])] == ' ' || documentCopy[y + strlen(word2[t])] == '\0'))
					{wmatch = 2;}										//if word has been found to match word2, increment the "word match" counter by 2
					cmatcha = 0;										//reset the "word1 char match" and "word2 char match" counters to 0
					cmatchb = 0;
				}

				if (wmatch == 3)										//if "word match" counter is 3, increment the "satisfy" counter and break out of the loop
				{
					satisfy++;
					break;
				}

				if (space == distance[t])								//if "space" counter reaches the limit specified in the rule set, reset "word match" and "space" counters to 0
				{
					space = 0;
					wmatch = 0;
				}
			}
		}
	}
	return satisfy;														//return number of rule sets satisfies
}
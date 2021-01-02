#include "ScoreList.h"

ScoreList::ScoreList()
	: Scores()
{}       // Create an empty score list.

bool ScoreList::add(unsigned long score)
{
	if (score >= 0 && score <= 100 && Scores.insert(Scores.size(), score))
		return true;
	else
		return false;
}
// If the score is valid (a value from 0 to 100) and the score list
// has room for it, add it to the score list and return true.
// Otherwise, leave the score list unchanged and return false.

bool ScoreList::remove(unsigned long score)
{
	if (Scores.erase(Scores.find(score)))
		return true;
	else
		return false;

}
// Remove one instance of the specified score from the score list.
// Return true if a score was removed; otherwise false.

int ScoreList::size() const
{
	return Scores.size();
}  // Return the number of scores in the list.

unsigned long ScoreList::minimum() const
{
	unsigned long y;
	unsigned long z;
	if (!Scores.get(0, y))
		return NO_SCORE;
	for (int x = 1; x < Scores.size(); x++)
	{
		Scores.get(x, z);
		if (z < y)
			y = z;
	}
	return y;
}
// Return the lowest score in the score list.  If the list is
// empty, return NO_SCORE.

unsigned long ScoreList::maximum() const
{
	unsigned long y;
	unsigned long z;
	if (!Scores.get(0, y))
		return NO_SCORE;
	for (int x = 1; x < Scores.size(); x++)
	{
		Scores.get(x, z);
		if (z > y)
			y = z;
	}
	return y;
}
// Return the highest score in the score list.  If the list is
// empty, return NO_SCORE.
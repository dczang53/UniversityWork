#include "provided.h"
#include "support.h"
#include <queue>
using namespace std;

bool operator>(GeoCoord a, GeoCoord b)
{
	if (a.latitude > b.latitude)
		return true;
	else if (a.latitude == b.latitude && a.longitude > b.longitude)
		return true;
	else
		return false;
}

bool operator<(GeoCoord a,  GeoCoord b)
{
	if (a.latitude < b.latitude)
		return true;
	else if (a.latitude == b.latitude && a.longitude < b.longitude)
		return true;
	else
		return false;
}

bool operator==(GeoCoord a, GeoCoord b)
{
	if (a.latitude == b.latitude && a.longitude == b.longitude)
		return true;
	else
		return false;
}

bool operator!=(GeoCoord a, GeoCoord b)
{
	if (a.latitude != b.latitude || a.longitude != b.longitude)
		return true;
	else
		return false;
}

PathNode::PathNode()
{}

PathNode::PathNode(GeoCoord gca, GeoCoord gcb)	//starting node
 : elapsed(0), parent(nullptr), street(gca, gca), first(true)
{estimate = distanceEarthMiles(gca, gcb);}

PathNode::PathNode(string n, PathNode* p, GeoCoord gca, GeoCoord gcb)
	: name(n), parent(p), street(p->street.end, gca), first(false)
{
	elapsed = (parent->elapsed + distanceEarthMiles(parent->street.end, gca));
	estimate = elapsed + distanceEarthMiles(gca, gcb);
}

bool operator>(string a, string b)
{
	int n;
	if (a.size() < b.size())
		n = a.size();
	else
		n = b.size();
	for (int x = 0; x < n; x++)
	{
		if (tolower(a[x]) > tolower(b[x]))
			return true;
		else if (tolower(a[x]) < tolower(b[x]))
			return false;
	}
	if (a.size() > b.size())
		return true;
	else
		return false;

}
bool operator<(string a, string b)
{
	int n;
	if (a.size() < b.size())
		n = a.size();
	else
		n = b.size();
	for (int x = 0; x < n; x++)
	{
		if (tolower(a[x]) < tolower(b[x]))
			return true;
		else if (tolower(a[x]) > tolower(b[x]))
			return false;
	}
	if (a.size() < b.size())
		return true;
	else
		return false;
}
bool operator==(string a, string b)
{
	if (a.size() != b.size())
		return false;
	for (int x = 0; x < a.size(); x++)
	{
		if (tolower(a[x]) != tolower(b[x]))
			return false;
	}
	return true;
}
bool operator!=(string a, string b)
{
	int n;
	if (a.size() < b.size())
		n = a.size();
	else
		n = b.size();
	for (int x = 0; x < n; x++)
	{
		if (tolower(a[x]) != tolower(b[x]))
			return true;
	}
	if (a.size() != b.size())
		return true;
	else
		return false;
}
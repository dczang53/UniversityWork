#ifndef SUPPORT
#define SUPPORT

#include <string>
#include <cstring>
#include <queue>
using namespace std;

bool operator>(GeoCoord a, GeoCoord b);
bool operator<(GeoCoord a, GeoCoord b);
bool operator==(GeoCoord a, GeoCoord b);
bool operator!=(GeoCoord a, GeoCoord b);
bool operator>(string a, string b);
bool operator<(string a, string b);
bool operator==(string a, string b);
bool operator!=(string a, string b);

struct PathNode
{
	PathNode();
	PathNode(GeoCoord gca, GeoCoord gcb);	//for origin
	PathNode(string n, PathNode* p, GeoCoord gca, GeoCoord gcb);	//for adding paths/branches
	double elapsed;
	double estimate;
	string name;
	PathNode* parent;
	GeoSegment street;
	bool first;
};

#endif //SUPPORT

#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <string>
#include <vector>
#include <stack>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	AttractionMapper AMap;
	SegmentMapper SMap;
};

NavigatorImpl::NavigatorImpl()
{}

NavigatorImpl::~NavigatorImpl()
{}

bool NavigatorImpl::loadMapData(string mapFile)
{
	MapLoader NM;
	if (!NM.load(mapFile))
		return false;
	AMap.init(NM);
	SMap.init(NM);
	return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	GeoCoord origin;
	if (!AMap.getGeoCoord(start, origin))
		return NAV_BAD_SOURCE;
	GeoCoord destination;
	if (!AMap.getGeoCoord(end, destination))
		return NAV_BAD_DESTINATION;

	struct opgreater
	{
		bool operator()(PathNode* a, PathNode* b)
		{
			return (a->estimate > b->estimate);
		}
	};
	priority_queue<PathNode*, vector<PathNode*>, opgreater> pathTree;
	vector<PathNode*> vpp;
	MyMap<GeoCoord, double> repeated;
	vpp.push_back(new PathNode(origin, destination));
	pathTree.push(vpp.back());
	repeated.associate(origin, 0);
	bool tobreak = false;
	while (pathTree.size() > 0)
	{
		PathNode* p = pathTree.top();
		pathTree.pop();
		PathNode* q;
		vector<StreetSegment> v = SMap.getSegments(p->street.end);
		for (int x = 0; x < v.size(); x++)
		{
			for (int y = 0; y < v[x].attractions.size(); y++)
			{
				if (v[x].attractions[y].geocoordinates == destination)
				{
					vpp.push_back(new PathNode(v[x].streetName, p, destination, destination));
					tobreak = true;
					break;
				}
			}
			if (tobreak)
				break;
			if (p->first)
			{
				q = new PathNode(v[x].streetName, p, v[x].segment.start, destination);
				vpp.push_back(q);
				pathTree.push(q);
				q = new PathNode(v[x].streetName, p, v[x].segment.end, destination);
				vpp.push_back(q);
				pathTree.push(q);
			}
			else if (v[x].segment.start == p->street.end)
			{
				if ((repeated.find(v[x].segment.end)) != nullptr)
				{tobreak = true;}
				if (!tobreak && (p->street.start != v[x].segment.end))
				{
					q = new PathNode(v[x].streetName, p, v[x].segment.end, destination);
					vpp.push_back(q);
					pathTree.push(q);
					repeated.associate(v[x].segment.end, 0);
				}
				tobreak = false;
			}
			else if (v[x].segment.end == p->street.end)
			{
				if ((repeated.find(v[x].segment.start)) != nullptr)
				{tobreak = true;}
				if (!tobreak && (p->street.start != v[x].segment.start))
				{
					q = new PathNode(v[x].streetName, p, v[x].segment.start, destination);
					vpp.push_back(q);
					pathTree.push(q);
					repeated.associate(v[x].segment.end, 0);
				}
				tobreak = false;
			}
		}
		if (tobreak)
			break;
	}
	if (pathTree.size() == 0 && !tobreak)
		return NAV_NO_ROUTE;
	PathNode* f = vpp.back();
	double retAngle;
	string retDir;
	vector<NavSegment> retvalue;
	stack<NavSegment> retbuffer;
	while ((f != nullptr) && !(f->first))
	{
		double dist = distanceEarthMiles(f->street.start, f->street.end);
		retAngle = angleOfLine(f->street);
		if (retAngle >= 0 && retAngle <= 22.5)
			retDir = "east";
		else if (retAngle > 22.5 && retAngle <= 67.5)
			retDir = "northeast";
		else if (retAngle > 67.5 && retAngle <= 112.5)
			retDir = "north";
		else if (retAngle > 112.5 && retAngle <= 157.5)
			retDir = "northwest";
		else if (retAngle > 157.5 && retAngle <= 202.5)
			retDir = "west";
		else if (retAngle > 202.5 && retAngle <= 247.5)
			retDir = "southwest";
		else if (retAngle > 247.5 && retAngle <= 292.5)
			retDir = "south";
		else if (retAngle > 292.5 && retAngle <= 337.5)
			retDir = "southeast";
		else if (retAngle > 337.5 && retAngle < 360)
			retDir = "east";
		retbuffer.push(NavSegment(retDir, f->name, dist, f->street));
		if ((f->parent != nullptr) && !(f->parent->first) && (f->name != f->parent->name))
		{
			retAngle = angleBetween2Lines(f->parent->street, f->street);
			if (retAngle < 180)
				retDir = "left";
			else
				retDir = "right";
			retbuffer.push(NavSegment(retDir, f->parent->name));
		}
		f = f->parent;
	}
	for (int z = 0; z < vpp.size(); z++)
	{delete vpp[z];}
	while (!retbuffer.empty())
	{
		retvalue.push_back(retbuffer.top());
		retbuffer.pop();
	}
	directions = retvalue;
	return NAV_SUCCESS;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}

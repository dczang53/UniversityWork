#include "provided.h"
#include "MyMap.h"
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> AtMap;
};

AttractionMapperImpl::AttractionMapperImpl()
{}

AttractionMapperImpl::~AttractionMapperImpl()
{}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	StreetSegment buffer;
	for (int x = 0; x < ml.getNumSegments(); x++)
	{
		ml.getSegment(x, buffer);
		for (int c = 0; c < buffer.attractions.size(); c++)
		{
			AtMap.associate(buffer.attractions[c].name, buffer.attractions[c].geocoordinates);
		}
	}
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	const GeoCoord* g = AtMap.find(attraction);
	if (g != nullptr)
	{
		gc = *(g);
		return true;
	}
	else
		return false;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}

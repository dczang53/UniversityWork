#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <vector>
using namespace std;



class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<GeoCoord, vector<StreetSegment>> SMap;
};

SegmentMapperImpl::SegmentMapperImpl()
{}

SegmentMapperImpl::~SegmentMapperImpl()
{}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	StreetSegment buffer;
	vector<StreetSegment> sbuffer;
	vector<StreetSegment>* spbuffer;
	for (int x = 0; x < ml.getNumSegments(); x++)
	{
		ml.getSegment(x, buffer);
		spbuffer = SMap.find(buffer.segment.start);
		if (spbuffer == nullptr)
		{
			sbuffer.push_back(buffer);
			SMap.associate(buffer.segment.start, sbuffer);
			sbuffer.pop_back();
		}
		else
			spbuffer->push_back(buffer);
		spbuffer = SMap.find(buffer.segment.end);
		if (spbuffer == nullptr)
		{
			sbuffer.push_back(buffer);
			SMap.associate(buffer.segment.end, sbuffer);
			sbuffer.pop_back();
		}
		else
			spbuffer->push_back(buffer);

		for (int c = 0; c < buffer.attractions.size(); c++)
		{
			spbuffer = SMap.find(buffer.attractions[c].geocoordinates);
			if (spbuffer == nullptr)
			{
				sbuffer.push_back(buffer);
				SMap.associate(buffer.attractions[c].geocoordinates, sbuffer);
				sbuffer.pop_back();
			}
			else
				spbuffer->push_back(buffer);
		}
	}

}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	vector<StreetSegment> segments = *(SMap.find(gc));
	return segments;

}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}

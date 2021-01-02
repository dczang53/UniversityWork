#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	vector<StreetSegment> m_stuff;
};

MapLoaderImpl::MapLoaderImpl()
{}

MapLoaderImpl::~MapLoaderImpl()
{}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream infile(mapFile);
	if (!infile)
		return false;
	char n;
	int c;
	string s[5];


	while (getline(infile, s[0]))
	{
		if (!infile.get(n))
			return true;
		for (int x = 1; x < 5; x++)
		{
			while (isdigit(n) || n == '.' || n == '-')
			{
				s[x] += n;
				infile.get(n);
			}
			while (n == ' ' || n == ',')
			{
				infile.get(n);
			}
		}
		if (!(infile >> c))
			return true;
		infile.ignore(100, '\n');
		StreetSegment street;
		street.streetName = s[0];
		street.segment = GeoSegment(GeoCoord(s[1], s[2]), GeoCoord(s[3], s[4]));
		m_stuff.push_back(street);
		s[0] = "";
		s[1] = "";
		s[2] = "";
		s[3] = "";
		s[4] = "";
		for (; c > 0; c--)
		{
			if (!getline(infile, s[3]))
				return true;
			int y = 0;
			while (s[3][y] != '|' && (y < s[3].size()))
			{
				s[0] += s[3][y];
				y++;
			}
			y++;
			if (y >= s[3].size())
				return true;
			while (y < s[3].size() && (s[3][y] == ' '))
			{
				y++;
			}
			while (y < s[3].size() && (isdigit(s[3][y]) || s[3][y] == '.' || s[3][y] == '-'))
			{
				s[1] += s[3][y];
				y++;
			}
			if (y >= s[3].size())
				return true;
			while (y < s[3].size() && (s[3][y] == ' ' || s[3][y] == ','))
			{
				y++;
			}
			if (y >= s[3].size())
				return true;
			while (y < s[3].size() && (isdigit(s[3][y]) || s[3][y] == '.' || s[3][y] == '-'))
			{
				s[2] += s[3][y];
				y++;
			}
			if (y > s[3].size())
				return true;
			Attraction a;
			if (s[0].size() > 0 && s[0][s[0].size() - 1] == ' ')
				s[0].pop_back();
			a.name = s[0];
			GeoCoord b(s[1], s[2]);
			a.geocoordinates = b;
			m_stuff.back().attractions.push_back(a);
			s[0] = "";
			s[1] = "";
			s[2] = "";
			s[3] = "";
		}
	}
	
	return true;
	
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_stuff.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum >= m_stuff.size() || segNum < 0)
		return false;
	seg = m_stuff[segNum];
	return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}


/*
Gayley Avenue
34.0669742, -118.4478277 34.0668392,-118.4476837
0
Gayley Avenue
34.0602175, -118.4464952 34.0597400,-118.4460477
3
Iso Fusion Café|34.0600264, -118.4460993
Native Foods Café|34.0599185, -118.4460044
Novel Cafe Westwood|34.0600033, -118.4465424
Gayley Avenue
34.0597400, -118.4460477 34.0594307,-118.4457798
1
Gayley Center|34.0595207, -118.4461079
Gayley Avenue
34.0594307, -118.4457798 34.0593392,-118.4457005
0
Gayley Avenue
34.0668392, -118.4476837 34.0666361,-118.4475188
0
Gayley Avenue
34.0666361, -118.4475188 34.0664405,-118.4474729
0
Gayley Avenue
34.0664405, -118.4474729 34.0662845,-118.4474763
0
Gayley Avenue
34.0662845, -118.4474763 34.0661429,-118.4475222
0
Gayley Avenue
34.0661429, -118.4475222 34.0656290,-118.4477200
1
Parking 1|34.0657660, -118.4474487
Gayley Avenue
34.0593392, -118.4457005 34.0592176,-118.4456464
0
Gayley Avenue
34.0592176, -118.4456464 34.0590860,-118.4455843
0
Gayley Avenue
34.0590860, -118.4455843 34.0589282,-118.4455418
0
Gayley Avenue
34.0589282, -118.4455418 34.0586527,-118.4455042
0
Gayley Avenue
34.0586527, -118.4455042 34.0585535,-118.4455063
0
Gayley Avenue
34.0585535, -118.4455063 34.0583655,-118.4455102
0
...
Gayley Avenue
34.0619693, -118.4479670 34.0607070,-118.4469116
28
1000 Gayley Avenue|34.0619411, -118.4475609
1001 Gayley
*/

/*
8.4471593
Westwood Copies | 34.0615188, -118.4481217
Westwood Sporting Goods | 34.0607599, -118.4471582
Whole Foods Market | 34.0612088, -118.4470689
Gayley Avenue
34.0607070, -118.4469116 34.0602175, -118.4464952
5
El Pollo Loco | 34.0605728, -118.4470117
Elysee | 34.0602814, -118.4467380
Mio Babbo's|34.0607578, -118.4467400
The Gayley Building | 34.0607929, -118.4465048
Westwood Flower Garden | 34.0604296, -118.4468857
Georgina Avenue
34.0443593, -118.4955823 34.0445712, -118.4952853
0
Georgina Avenue
34.0445712, -118.4952853 34.0446620, -118.4950578
0
Georgina Avenue
34.0446620, -118.4950578 34.0447281, -118.4948523
0
Georgina Avenue
34.0447281, -118.4948523 34.0447676, -118.4946040
0
Georgina Avenue
34.0447676, -118.4946040 34.0448457, -118.4943790
0
Georgina Avenue
34.0448457, -118.4943790 34.0449331, -118.4941276
0
Georgina Avenue
34.0449331, -118.4941276 34.0451373, -118.4938469
0
Georgina Avenue
34.0451373, -118.4938469 34.0454840, -118.4934065
0
Georgina Avenue
34.0454840, -118.4934065 34.0461524, -118.4925496
0
Georgina Avenue
34.0461524, -118.4925496 34.0463824, -118.4921976
0
Georgina Avenue
34.0463824, -118.4921976 34.0465175, -118.4918886
0
Georgina Avenue
34.0465175, -118.4918886 34.0466100, -118.4914852
0
Georgina Avenue
34.0466100, -118.4914852 34.0465886, -118.4907900
0
Georgina Avenue
34.0465886, -118.4907900 34.0465906, -118.4906596
0
Georgina Avenue
34.0465906, -118.4906596 34.0465957, -118.4903265
0
Georgina Avenue
34.0465957, -118.4903265 34.0466409, -118.4900895
0
Georgina Avenue
34.0466409, -118.4900895 34.0466963, -118.4899691
0
Georgina Avenue
34.0404270, -118.4992792 34.0411322, -118.4984104
0
Georgina Avenue
34.0411322, -118.4984104 34.0418242, -118.4975577
0
Georgina Avenue
34.0418242, -118.4975577 34.0425011, -118.4967245
0
Georgina Avenue
34.0425011, -118.4967245 34.0431928, -118.4958717
0
Georgina Avenue
34.0431928, -118.4958717 34.0438832, -118.4950204
0
Getty Center Drive
34.0784189, -118.4751579 34.0784486, -118.4751884
0
Getty Center Drive
34.0784486, -118.4751884 34.0786930, -118.4753869
0
Getty Center Drive
34.07
*/

//19641
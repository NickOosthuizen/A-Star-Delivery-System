#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ExpandableHashMap.h"
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
	ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_streetMap;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
	ifstream inf(mapFile);	//read in mapfile
	if (!inf)			//return false if could not be read in
		return false;
	string line;
	
	while (getline(inf, line))
	{
		istringstream iss(line);
		
		string name;
		int numCoords;

		name = iss.str();	//if there is an empty line at the end of the file, break
		if (name == "")
			break;

		if (!getline(inf, line))	
			return false;

		istringstream iss2(line);	//number following should be how many line segments follow the name

		iss2 >> numCoords;

		for (int i = 0; i < numCoords; i++)	//for the number of line segments, convert the strings into streetsegments
		{
			if (!getline(inf, line))
				return false;
			istringstream iss3(line);
			string latitude1;
			string longitude1;
			string latitude2;
			string longitude2;
			iss3 >> latitude1 >> longitude1 >> latitude2 >> longitude2;
			GeoCoord start(latitude1, longitude1);
			GeoCoord end(latitude2, longitude2);
			StreetSegment s(start, end, name);
			vector<StreetSegment>* sVector = m_streetMap.find(start);
			if (sVector != nullptr)	//if a geocoord already has associated streetsegments, add the segment to it
			{
				sVector->push_back(s);
			}
			else                           //if not, create a new vector of streetsegments
			{
				vector<StreetSegment> newSegment;
				newSegment.push_back(s);
				m_streetMap.associate(start, newSegment);
			}
			sVector = m_streetMap.find(end);		//do the same with the reversed segments
			StreetSegment r(end, start, name);
			if (sVector != nullptr)
			{
				sVector->push_back(r);
			}
			else
			{
				vector<StreetSegment> newSegment;
				newSegment.push_back(r);
				m_streetMap.associate(end, newSegment);
			}

		}
	}
	return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
	const vector<StreetSegment>* segVector = m_streetMap.find(gc);	//find vector asssociated with coord
	if (segVector == nullptr)
		return false;
	segs.erase(segs.begin(), segs.end());
	for (int i = 0; i < segVector->size(); i++)		//place associated segments into segs
	{
		segs.push_back((*segVector)[i]);
	}
	return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}

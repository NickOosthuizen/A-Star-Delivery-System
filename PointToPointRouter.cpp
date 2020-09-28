#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <queue>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
	const StreetMap* m_streetMap;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
	m_streetMap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}


//A* Star Implementation of Route Finding
DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
	route.clear();		//clear route
	totalDistanceTravelled = 0;
	if (start == end)	//if start is end, already at delivery location
		return DELIVERY_SUCCESS;
	vector<StreetSegment> containsStEnd;
	if (!(m_streetMap->getSegmentsThatStartWith(start, containsStEnd)) || !(m_streetMap->getSegmentsThatStartWith(end, containsStEnd)))	//if start or end is not in map, it is a bad coord
		return BAD_COORD;
	
	//f-value, location
	priority_queue < pair<double, GeoCoord>, vector<pair<double, GeoCoord>>, greater<pair<double, GeoCoord>> > openLocations;
	//current location : previous location 
	ExpandableHashMap<GeoCoord, GeoCoord> routeMap;
	//location : cost from start
	ExpandableHashMap<GeoCoord, double> totalCosts;

	// g is the total distance to get to the location
	double g = 0;
	// f = g + distance to end
	double f = 0;
	GeoCoord currentLoc;
	pair<double, GeoCoord> priorityPair;
	priorityPair.first = f;
	priorityPair.second = start;
	totalCosts.associate(start, g);
	openLocations.push(priorityPair);

	vector<StreetSegment> nextSegments;
	GeoCoord nextLoc;
	double* oldGVal;

	while (!openLocations.empty())
	{
		currentLoc = openLocations.top().second;
		openLocations.pop();
		if (currentLoc == end)	//if end found, retrieve route to get there
		{
			GeoCoord previous;
			GeoCoord currentCoord = currentLoc;
			while (previous != start)
			{
				previous = *(routeMap.find(currentCoord));
				StreetSegment s;
				s.start = previous;
				s.end = currentCoord;
				vector<StreetSegment> nameFinder;
				m_streetMap->getSegmentsThatStartWith(previous, nameFinder);
				for (int i = 0; i < nameFinder.size(); i++)
				{
					if (nameFinder[i].end == currentCoord)
					{
						s.name = nameFinder[i].name;
						break;
					}
				}
				route.push_front(s);
				totalDistanceTravelled += distanceEarthMiles(s.start, s.end);
				currentCoord = previous;
			}
			return DELIVERY_SUCCESS;
		}
		
		//get all connected street segments
		m_streetMap->getSegmentsThatStartWith(currentLoc, nextSegments);
		for (int i = 0; i < nextSegments.size(); i++)
		{
			nextLoc = nextSegments[i].end;

			g = *(totalCosts.find(currentLoc)) + distanceEarthMiles(currentLoc, nextLoc);

			oldGVal = totalCosts.find(nextLoc);
			//if this location has not yet been visited or is better than the previous route, process it
			if (oldGVal == nullptr || (g < *oldGVal))
			{
				routeMap.associate(nextLoc, currentLoc);
				//record the g value to get to Loc
				totalCosts.associate(nextLoc, g);
				f = g + distanceEarthMiles(nextLoc, end);
				priorityPair.first = f;
				priorityPair.second = nextLoc;
				openLocations.push(priorityPair);
			}
		}
	}
    return NO_ROUTE;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}


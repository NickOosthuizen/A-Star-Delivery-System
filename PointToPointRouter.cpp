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
	
	pair<double, GeoCoord> startPoint;
	startPoint.first = distanceEarthMiles(start, end);
	startPoint.second = start;
	priority_queue < pair<double, GeoCoord>, vector<pair<double, GeoCoord>>, greater<pair<double, GeoCoord>> > routeQueue;	//uses a priority queue based on smallest double value
	routeQueue.push(startPoint);	//add first value to queue

	ExpandableHashMap<GeoCoord, GeoCoord> routeMap;
	while (!routeQueue.empty())
	{
		GeoCoord current = routeQueue.top().second;
		routeQueue.pop();
		if (current == end)	//if end found, retrieve route to get there
		{
			GeoCoord previous;
			GeoCoord currentCoord = current;
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

		vector<StreetSegment> connecters;
		m_streetMap->getSegmentsThatStartWith(current, connecters);	
		for (int i = 0; i < connecters.size(); i++)
		{
			if (routeMap.find(connecters[i].end) == nullptr)	//if point has already been visited, do not return to it
			{
				routeMap.associate(connecters[i].end, current);	//associate geocoord ends with beginnings
				pair<double, GeoCoord> newStreetCoord;
				newStreetCoord.first = distanceEarthMiles(connecters[i].end, end);
				newStreetCoord.second = connecters[i].end;
				routeQueue.push(newStreetCoord);	//push new coord onto the queue
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


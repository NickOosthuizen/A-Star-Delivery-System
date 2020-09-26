#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
	PointToPointRouter* m_pathFinder;
	string angleDir(double angle) const;
	DeliveryOptimizer* m_optimizer;

};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
	m_pathFinder = new PointToPointRouter(sm);
	m_optimizer = new DeliveryOptimizer(sm);

}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
	delete m_pathFinder;
	delete m_optimizer;
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
	commands.clear();
	totalDistanceTravelled = 0;

	double oldDist = 0;
	double newDist = 0;

	vector<DeliveryRequest> optimizedDeliveries = deliveries;

	m_optimizer->optimizeDeliveryOrder(depot, optimizedDeliveries, oldDist, newDist);

	list<StreetSegment> deliveryRoute;
	GeoCoord start = depot;
	double distance = 0;
	vector<list<StreetSegment>> totalRoute;
	for (int i = 0; i < deliveries.size(); i++)		//ensure that deliviers are valid, and push them onto a vector to be processed
	{
		DeliveryResult deliveryCheck;
		deliveryCheck = m_pathFinder->generatePointToPointRoute(start, optimizedDeliveries[i].location, deliveryRoute, distance);
		
		if (deliveryCheck != DELIVERY_SUCCESS)
			return deliveryCheck;
		totalDistanceTravelled += distance;
		totalRoute.push_back(deliveryRoute);
		start = deliveries[i].location;
	}
	m_pathFinder->generatePointToPointRoute(start, depot, deliveryRoute, distance);		//add route to return to depot
	totalDistanceTravelled += distance;
	totalRoute.push_back(deliveryRoute);
	

	for (int i = 0; i < totalRoute.size(); i++)
	{
		list<StreetSegment>::iterator it;	
		it = totalRoute[i].begin();
		double proceedDistance = 0;
		vector<StreetSegment> proceedList;
		list<StreetSegment>::iterator lastFinder;	//finds last element of the list
		lastFinder = totalRoute[i].end();
		lastFinder--;
		for (; it != totalRoute[i].end(); it++)
		{
			
			if (it == lastFinder)
			{
				proceedList.push_back(*it);				//need to proceed down last street segment
				proceedDistance += distanceEarthMiles(it->start, it->end);

				double angle = angleOfLine(proceedList[0]);
				DeliveryCommand proceed;
				const string dir = angleDir(angle);
				proceed.initAsProceedCommand(dir, proceedList[0].name, proceedDistance);
				commands.push_back(proceed);
				proceedDistance = 0;

				if (i != (totalRoute.size() - 1))	//ensure that the route is making a delivery, not returning to the depot
				{
					DeliveryCommand deliver;
					deliver.initAsDeliverCommand(deliveries[i].item);
					commands.push_back(deliver);
				}
				break;
			}
			if (proceedList.empty())	//if not proceeding, add segment to proceed
			{
				proceedList.push_back(*it);
				proceedDistance += distanceEarthMiles(it->start, it->end);
			}
			else if (it->name == proceedList.back().name)	//if on same road, combine commands
			{
				proceedList.push_back(*it);
				proceedDistance += distanceEarthMiles(it->start, it->end);
			}
			else if (it->name != proceedList.back().name)	//if on different road, need to turn
			{
				if (proceedDistance != 0)		//first complete any held proceed street segments
				{
					double angle = angleOfLine(proceedList[0]);
					DeliveryCommand proceed;
					const string dir = angleDir(angle);
					proceed.initAsProceedCommand(dir, proceedList[0].name, proceedDistance);
					commands.push_back(proceed);
					proceedDistance = 0;
					
				}
				double theta = angleBetween2Lines(proceedList.back(), *it);	
				proceedList.clear();
				if (theta < 1 || theta > 359)	//if angle is small enough, it is more like going straight
				{
					double angle = angleOfLine(*it);
					DeliveryCommand proceed;
					const string dir = angleDir(angle);
					proceed.initAsProceedCommand(dir, it->name, distanceEarthMiles(it->start, it->end));
					commands.push_back(proceed);
				}
				else                            //if it is larger, make turn
				{
					string leftOrRight;
					if (theta >= 1 && theta < 180)
						leftOrRight = "left";
					else
						leftOrRight = "right";
					DeliveryCommand turn;
					turn.initAsTurnCommand(leftOrRight, it->name);
					commands.push_back(turn);
				}
			}

		}
		
	}
	return DELIVERY_SUCCESS;
    
}

string DeliveryPlannerImpl::angleDir(double angle) const	//find correct angle direction
{
	if (angle >= 0 && angle < 22.5)
		return "east";
	else if (angle >= 22.5 && angle < 67.5)
		return "northeast";
	else if (angle >= 67.5 && angle < 112.5)
		return "north";
	else if (angle >= 112.5 && angle < 157.5)
		return "northwest";
	else if (angle >= 157.5 && angle < 202.5)
		return "west";
	else if (angle >= 202.5 && angle < 247.5)
		return "southwest";
	else if (angle >= 247.5 && angle < 292.5)
		return "south";
	else if (angle >= 292.5 && angle < 337.5)
		return "southeast";
	else
		return "east";
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}

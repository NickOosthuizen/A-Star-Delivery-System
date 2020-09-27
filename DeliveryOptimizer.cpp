#include "provided.h"
#include <vector>
using namespace std;


double getDistance(const vector<DeliveryRequest>& deliveries, const GeoCoord& depot);

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
	double acceptChance(double distance, double newDistance, double temp) const;

};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}


double DeliveryOptimizerImpl::acceptChance(double distance, double newDistance, double temp) const
{
	double chance;
	//chance will be accepted
	if (newDistance < distance)
		chance = 1;
	//when temp is higher, a larger number will be returned
	//this means the optimizer will have a higher chance of accepting a worse solution earlier on
	else
		chance = exp((distance - newDistance) / temp);
	return chance;
}

//Gets the distance of entire route
double getDistance(const vector<DeliveryRequest>& deliveries, const GeoCoord& depot)
{
	int distance = 0;
	GeoCoord start = depot;
	if (deliveries.size() < 1)
		return distance;
	
	for (int i = 0; i < deliveries.size(); i++)
	{
		distance += distanceEarthMiles(start, deliveries[i].location);
		start = deliveries[i].location;
	}
	distance += distanceEarthMiles(start, depot);

	return distance;
}

//simulated annealing algorithm to optimize delivery order
void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
	double temp = 10000;
	double coolingRate = 0.003;

	oldCrowDistance = getDistance(deliveries, depot);
	vector<DeliveryRequest> currentRoute = deliveries;
	vector<DeliveryRequest> optimizedDeliveries = currentRoute;

	while (temp > 1)
	{
		vector<DeliveryRequest> newRoute = deliveries;

		//randomly switch 2 positions in the route
		int position1 = rand() % deliveries.size();
		int position2 = rand() % deliveries.size();

		DeliveryRequest tempDeliv = newRoute[position1];
		newRoute[position1] = newRoute[position2];
		newRoute[position2] = tempDeliv;
	
		int currentDistance = getDistance(currentRoute, depot);
		int newDistance = getDistance(newRoute, depot);

		//if the accept chance (between 0-1) is greater than than a 
		//rand chance (0-1) accept it as the current solution
		//exploring random choices decreases the chance of getting stuck in a local optimum
		if (acceptChance(currentDistance, newDistance, temp) > (rand() / RAND_MAX))
			currentRoute = newRoute;

		//if the current route is shorter than the best current route, take it as the route
		if (getDistance(currentRoute, depot) < getDistance(optimizedDeliveries, depot))
			optimizedDeliveries = currentRoute;

		temp *= 1 - coolingRate;
	}

	deliveries = optimizedDeliveries;
	newCrowDistance = getDistance(deliveries, depot);

}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}

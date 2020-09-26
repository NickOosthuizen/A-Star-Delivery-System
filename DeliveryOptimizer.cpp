#include "provided.h"
#include <vector>
using namespace std;

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
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
	oldCrowDistance = 0;
	GeoCoord start = depot;
	for (int i = 0; i < deliveries.size(); i++)
	{
		oldCrowDistance += distanceEarthMiles(start, deliveries[i].location);
		start = deliveries[i].location;
	}
	oldCrowDistance += distanceEarthMiles(start, depot);
	
	vector<DeliveryRequest> optimizedDeliveries;
	start = depot;
	
	for (int i = 0; i < deliveries.size(); )
	{
		DeliveryRequest optimalSpot(deliveries[i].item, deliveries[i].location);	//attempt ten random position in deliveries to try to find an optimal ordering
		int optimalSpotIndex = i;
		for (int j = 0; j < 10; j++)
		{
			int position = rand() % deliveries.size();
			double distance = distanceEarthMiles(start, optimalSpot.location);
			if (distanceEarthMiles(deliveries[position].location, start) < distance)
			{
				optimalSpot = deliveries[position];
				optimalSpotIndex = i;
			}
		}
		start = optimalSpot.location;
		optimizedDeliveries.push_back(optimalSpot);
		deliveries.erase(deliveries.begin() + optimalSpotIndex);
	}
	deliveries = optimizedDeliveries;

	start = depot;
	newCrowDistance = 0;
	for (int i = 0; i < deliveries.size(); i++)
	{
		newCrowDistance += distanceEarthMiles(start, deliveries[i].location);
		start = deliveries[i].location;
	}
	newCrowDistance += distanceEarthMiles(start, depot);

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

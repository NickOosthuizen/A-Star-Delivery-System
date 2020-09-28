# Routing System

This was my final project for my CS class for winter 2020. It is a 
program that takes in mapdata coordinates from a text file and a list
of deliveries from another text file and generates a route to each of
the delivery locations and back to the start. The provided.h files and 
main.cpp files were provided by my professor. I implemented DeliveryOptimizer.cpp,
PointToPointRouter.cpp, StreetMap.cpp, and DeliverPlanner.cpp except for a few 
wrapper/delegating functions. 

ExpandableHashMap.h: Generic HashMap class using templates to hold any type of data
StreetMap.cpp: Reads in mapdata file into a ExpandableHashMap
PointToPointRouter.cpp: Uses A* algorithm to generate route to given location
DeliveryOptimizer.cpp: Uses Simulated Anneling algorithm to optimize the order of deliveries
DeliverPlanner.cpp: Translates optimized routes of streetsegments into proceed, turn, and deliver text commands

## Usage:

executable mapdata.txt deliveries.txt



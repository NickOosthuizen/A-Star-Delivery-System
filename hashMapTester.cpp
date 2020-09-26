#include "ExpandableHashMap.h"
#include <string>
#include <iostream>
#include <functional>
using namespace std;

unsigned int hasher(const string& g)
{
	hash<string> str_hash;
	return str_hash(g);
}

int main()
{
	ExpandableHashMap<string, int> h;
	h.associate("hi", 5);
	h.associate("hi", 4);
	int* val = h.find("hi");
	cout << *val << endl;
	h.reset();
	cout << h.size() << endl;
	h.associate("so", 2);
	h.associate("ro", 3);
	h.associate("mo", 7);
	cout << *(h.find("mo")) << endl;
	cout << h.size() << endl;
	h.associate("to", 6);
	h.associate("no", 7);
	h.associate("do", 9);
	cout << h.size();
		
}
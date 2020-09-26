// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <vector>
#include <list>
#include "provided.h"


template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	struct m_association
	{
		KeyType m_key;
		ValueType m_value;
	};
	std::vector<std::list<m_association> * > * m_hashMap;
	unsigned int getBucketNum(const KeyType& key) const;
	void addItem(const KeyType& key, const ValueType& value);
	double m_maxLoadFactor;
	int m_numBuckets;
	double m_numItems;
	
};



template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	m_hashMap = new std::vector<std::list<m_association> *> (8, nullptr);
	m_maxLoadFactor = maximumLoadFactor;
	m_numBuckets = 8;
	m_numItems = 0;

}

template <typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
	reset();
	delete m_hashMap;
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	typename std::vector<std::list<m_association>*>::iterator it;
	for (it = m_hashMap->begin(); it != m_hashMap->end(); it++)
	{
		if (*it != nullptr)
		{ 
			delete *it;
			*it = nullptr;
		}
			
	}
	(*m_hashMap).resize(8, nullptr);
	m_numBuckets = 8;
	m_numItems = 0;
}

template <typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_numItems;
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::addItem(const KeyType& key, const ValueType& value)		//helper function for associating
{
	unsigned int location = getBucketNum(key);	//add new lists of associations if one is not present
	if ((*m_hashMap)[location] == nullptr)
	{
		(*m_hashMap)[location] = new std::list<m_association>;
	}
	m_association newAssociation;	//add association to the list
	newAssociation.m_key = key;
	newAssociation.m_value = value;
	(*m_hashMap)[location]->push_back(newAssociation);
	m_numItems++;
}

template <typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	ValueType* oldVal = find(key);	//replace value if key is already in map
	if (oldVal != nullptr)
	{
		*oldVal = value;
	}
	else
	{
		addItem(key, value);
	}
	if ((m_numItems / m_numBuckets) > m_maxLoadFactor)	//resize the map if the max load factor is exceeded
	{
		int prevNumBuckets = m_numBuckets;	//double number of buckets
		m_numBuckets *= 2;
		std::vector<std::list <m_association>* >* oldHashMap = m_hashMap;
		m_hashMap = new std::vector<std::list <m_association>* >(m_numBuckets, nullptr);	//generate new hash map 
		m_numItems = 0;
		for (int i = 0; i < prevNumBuckets; i++)	//fill new hash map with old hash map's values
		{
			if ((*oldHashMap)[i] != nullptr)
			{
				typename std::list<m_association>::iterator it;
				for (it = (*oldHashMap)[i]->begin(); it != (*oldHashMap)[i]->end(); it++)
				{
					associate(it->m_key, it->m_value);
				}
				delete (*oldHashMap)[i];	//delete old lists as they are added
			}
		}
		delete oldHashMap;	//delete old hash map
	}

}

template <typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	unsigned int location = getBucketNum(key);	//find expected bucket number of value and check if a value is already there
	if ((*m_hashMap)[location] == nullptr)
		return nullptr;
	typename std::list<m_association>::iterator it;
	for (it = (*m_hashMap)[location]->begin(); it != (*m_hashMap)[location]->end(); it++)
	{
		if (it->m_key == key)
			return &(it->m_value);
	}
	return nullptr;
}

template <typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNum(const KeyType& key) const
{
	unsigned int hasher(const KeyType & k);
	unsigned int bucketNum = hasher(key) % m_numBuckets;
	return bucketNum;
}


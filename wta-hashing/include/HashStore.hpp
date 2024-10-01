/*
 * hashStore.hpp
 *
 *  Created on: Oct 4, 2015
 *      Author: amir
 */

#ifndef INCLUDE_HASHSTORE_HPP_
#define INCLUDE_HASHSTORE_HPP_

#include <vector>
#include <unordered_map>


class HashStore{
public:
	virtual void record(std::vector <std::vector <int> >) = 0;
	virtual void lookup(std::vector <std::vector <int> >, std::vector <std::vector <int> > & votes) = 0;
};

class HashStoreSimple: public HashStore{

	std::vector<std::unordered_multimap <int,int> > theHashTables;

public:
	void record(std::vector <std::vector <int> >);
	void lookup(std::vector <std::vector <int> >, std::vector <std::vector <int> > & votes);

};

#endif /* INCLUDE_HASHSTORE_HPP_ */

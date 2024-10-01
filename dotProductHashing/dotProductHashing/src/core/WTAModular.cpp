/*
 * ModularWTA.cpp
 *
 *  Created on: Oct 4, 2015
 *      Author: amir
 */



#include "DotProductHasher.hpp"
#include "utility.h"

using namespace std;

template <typename Dtype>
WTAModular<Dtype>::WTAModular(unique_ptr<Hasher<Dtype> > & hasher_ ,unique_ptr<HashStore> & hashStore_,unique_ptr<Counter> & counter_,
		int numberToAccept_):
hasher(std::move(hasher_)),hashStore(std::move(hashStore_)),counter(std::move(counter_)), numberToAccept(numberToAccept_)
{

}

template <typename Dtype>
void WTAModular<Dtype>::makeNewHashTable(const FakeSpan<Dtype> & data){


	hasher->calculateNewHashPermutations();

	std::vector <std::vector <int> > allHashes;
	hasher->computeAllHashes(data,allHashes);

	hashStore->record(allHashes);
	//put hashes into table:

}


template <typename Dtype>
void WTAModular<Dtype>::lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches){


	std::vector <std::vector <int> > allHashes;
	hasher->computeAllHashes(data,allHashes);

	std::vector <std::vector<int> > hashMatches;
	hashStore->lookup (allHashes,hashMatches);

	std::vector<int> counts2;
	std::vector<int> equals;
	int id = 0;

	matches.resize(allHashes.size());
	for (auto votes: hashMatches){
		std::vector<int> counts;
		counter->count (votes,counts);
		getTopElemetIds(counts,numberToAccept,matches[id]);
		id ++;
	}
}

template class WTAModular<float>;

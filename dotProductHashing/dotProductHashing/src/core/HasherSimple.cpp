/*
 * SimpleHasher.cpp
 *
 *  Created on: Oct 4, 2015
 *      Author: amir
 */

#include <Hasher.hpp>

template <typename Dtype>
HasherSimple<Dtype>::HasherSimple(int nSectionsPerHash_,int sectionSize_,int nHashes_,
		int sizeOfEachVector_, int seed_):
nSectionsPerHash(nSectionsPerHash_), sectionSize(sectionSize_),nHashes(nHashes),sizeOfEachVector(sizeOfEachVector_)
{

}

template <typename Dtype>
void HasherSimple<Dtype>::calculateNewHashPermutations(){

}


template <typename Dtype>
void HasherSimple<Dtype>::computeAllHashes(const FakeSpan<Dtype> & data,std::vector < std::vector<int> > & hashes) {
	int n = data.size()/sizeOfEachVector;
	hashes.resize(n);
	for (int i = 0; i < n; i++){

	}
}


template class HasherSimple<float>;

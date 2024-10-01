/*
 * Hasher.hpp
 *
 *  Created on: Oct 4, 2015
 *      Author: amir
 */

#ifndef INCLUDE_HASHER_HPP_
#define INCLUDE_HASHER_HPP_

#include <vector>
#include <GSL-placeHolder.hpp>

template <typename Dtype>
class Hasher{
public:
	virtual void calculateNewHashPermutations() = 0;
	virtual void computeAllHashes(const FakeSpan<Dtype> & data,std::vector < std::vector<int> > & hashes) = 0;
	virtual ~Hasher(){};
};

template <typename Dtype>
class HasherSimple:public Hasher<Dtype>{
public:

	HasherSimple(int nSectionsPerHash_,int sectionSize_,int nHashes_,
			int sizeOfEachVector_, int seed_);
	virtual ~HasherSimple(){};

	void calculateNewHashPermutations() ;
	void computeAllHashes(const FakeSpan<Dtype> & data,std::vector < std::vector<int> > & hashes);

private:
	int nSectionsPerHash, sectionSize, nHashes, sizeOfEachVector;
};





#endif /* INCLUDE_HASHER_HPP_ */

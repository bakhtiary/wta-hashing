#ifndef DOT_PRODUCT_HASHER
#define DOT_PRODUCT_HASHER

#include <vector>
#include <boost/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <memory.h>
#include <GSL-placeHolder.hpp>
//#include "gtest.h"
#include <unordered_map>
#include "Hasher.hpp"
#include "HashStore.hpp"
#include "Counter.hpp"
#include "IDValPair.hpp"


template <typename Dtype>
class DotProductHasher{
public:
	virtual void makeNewHashTable(const FakeSpan<Dtype> & data) = 0;
	virtual void lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches) = 0;
	virtual ~DotProductHasher(){};
};

template <typename Dtype>
class WTAHasherSimple: public DotProductHasher<Dtype>{

public:
	WTAHasherSimple(int nSectionsPerHash,int sectionSize,int nHashes,int numberToAccept, int sizeOfEachVector_,int seed);
	virtual ~WTAHasherSimple(){};

	virtual void makeNewHashTable(const FakeSpan<Dtype> & data);
	virtual void lookUpHashForTheseVectors(const FakeSpan<Dtype> & data, std::vector <std::vector<int> > & matches);

	void computeHashes(const Dtype * data, std::vector < int > & hashes);

protected:

	void calculateNewHashPermutations();

	int sizeOfEachVector;
	int nHashes;
	int nSectionsPerHash;
	int sectionSize;
	int nPermutatedData;

	int * hashPermutations;
	int permutationArraySize;

	int nBitsPerSection;
	int nToAccept;

	std::vector<std::unordered_multimap <int,int> > theHashTables;


	boost::variate_generator<boost::mt19937,boost::uniform_int<> > var_uniform;

	friend class WTAHasherTest;

//	FRIEND_TEST(WTAHasherTest, RandomPermutation);
//	FRIEND_TEST(WTAHasherTest, ComputeHashes);
};

template <typename Dtype>
class WTACacheAware: public DotProductHasher<Dtype>{

public:
	WTACacheAware(int nSectionsPerHash,int sectionSize,int nHashes,int numberToAccept, int sizeOfEachVector_,int seed);
	virtual ~WTACacheAware(){}

	virtual void makeNewHashTable(const FakeSpan<Dtype> & data);
	virtual void lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches);

	void computeHashes(Dtype * data, std::vector < int > & hashes);

	void calculateNewHashPermutations();

private:

	void getWTAHashFaster(std::vector<int> &hashes,	const Dtype* data);
	void putInvertedHashesIntoManualTable(
			std::vector<int> * allCurHashesInverted);

	void lookupHashTableForTheseHashes(
			std::vector< std::vector<int> > &matchedWeights,  const std::vector<int> * allCurHashes, const int M_);

	boost::variate_generator<boost::mt19937,boost::uniform_int<> > var_uniform;

	int sizeOfEachVector;
	int nHashes;
	int nSectionsPerHash;
	int sectionSize;
	int nPermutatedData;

	int * hashPermutations;
	int permutationArraySize;

	int numberToAccept;
	int nBitsPerSection;

	int manRealHashWidth;
	int manHashWidth;

	int manHashStoreWidth;

	int numberOfVectorsInHashTable;


	std::vector <int> manualHashTable;
	std::vector <int> manualHashTableStore;
};

template <typename Dtype>
class WTACacheAwareSecondTry: public DotProductHasher<Dtype>{

public:
	WTACacheAwareSecondTry(int nSectionsPerHash,int sectionSize,int nHashes,int numberToAccept, int sizeOfEachVector_,int seed);
	virtual ~WTACacheAwareSecondTry(){}

	virtual void makeNewHashTable(const FakeSpan<Dtype> & data);
	virtual void lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches);

	void computeHashes(Dtype * data, std::vector < int > & hashes);

	void calculateNewHashPermutations();

private:

	void getWTAHashFaster(std::vector<int> &hashes,	const Dtype* data);
	void putInvertedHashesIntoManualTable(
			std::vector<int> * allCurHashesInverted);

	void lookupHashTableForTheseHashes(
			std::vector< std::vector<int> > &matchedWeights,  const std::vector<int> * allCurHashes, const int M_);

	boost::variate_generator<boost::mt19937,boost::uniform_int<> > var_uniform;

	int sizeOfEachVector;
	int nHashes;
	int nSectionsPerHash;
	int sectionSize;
	int nPermutatedData;

	int * hashPermutations;
	int permutationArraySize;

	int numberToAccept;
	int nBitsPerSection;

	uint64_t manRealHashWidth;
	uint64_t manHashWidth;

	uint64_t manHashStoreWidth;

	int numberOfVectorsInHashTable;


	std::vector <uint64_t> manualHashTable;
	std::vector <int> manualHashTableStore;
};



template <typename Dtype>
class WTAModular: public DotProductHasher<Dtype>{

public:
	WTAModular(std::unique_ptr<Hasher<Dtype> > &, std::unique_ptr< HashStore> &, std::unique_ptr< Counter > &,int numberToAccept);
	virtual ~WTAModular(){};
	virtual void makeNewHashTable(const FakeSpan<Dtype> & data);
	virtual void lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches);

private:
	std::unique_ptr<Hasher<Dtype>> hasher;
	std::unique_ptr<HashStore> hashStore;
	std::unique_ptr<Counter> counter;
	int numberToAccept;

};


template <typename Dtype>
class LEMSimple: public DotProductHasher<Dtype>{ // largest element multiplier
public:
	LEMSimple(int nElemToKeep,int nElemToMultiply,int numberToAccept,int sizeOfEachVector_);
	~LEMSimple(){};
	virtual void makeNewHashTable(const FakeSpan<Dtype> & data);
	virtual void lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches);


private:
	int nElemToKeep;
	int nElemToMultiply;

	int sizeOfEachVector;
	int numberToAccept;

	std::vector <std::vector<IDValPair<Dtype> > > store;

	friend class LEMSimpleTest;
//	FRIEND_TEST(LEMSimpleTest, testHashTableConstruction);
};

template <typename Dtype>
class ExactHighestProduct: public DotProductHasher<Dtype>{ // largest element multiplier
public:
	ExactHighestProduct(int numberToAccept,int sizeOfEachVector_);
	virtual void makeNewHashTable(const FakeSpan<Dtype> & data);
	virtual void lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches);


private:

	int sizeOfEachVector;
	int numberToAccept;

	std::vector <Dtype> store;

	friend class testExactHighestProduct;
};



#endif

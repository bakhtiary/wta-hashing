/*
 * CacheOptimizedWTA.cpp
 *
 *  Created on: Oct 1, 2015
 *      Author: amir
 */
#include "DotProductHasher.hpp"

#include <xmmintrin.h>

using namespace std;

const unsigned mapSize = 10200; //2063;
const unsigned idMask = 0x00ffffff;
const unsigned addOne = idMask + 1;
const unsigned countMask = 0xff000000; // TODO ~idMask?

bool compareCompactHashMatch2_3(unsigned int p1, unsigned int p2) {
	return ((p1 & countMask) > (p2 & countMask));
}


template<typename Dtype>
WTACacheAwareSecondTry<Dtype>::WTACacheAwareSecondTry(int nSectionsPerHash_,
		int sectionSize_, int nHashes_, int numberToAccept_,
		int sizeOfEachVector_,int seed) :
		nSectionsPerHash(nSectionsPerHash_), sectionSize(sectionSize_), nHashes(
				nHashes_), numberToAccept(numberToAccept_), sizeOfEachVector(sizeOfEachVector_),
				var_uniform(boost::mt19937(seed),boost::uniform_int<>(0,sizeOfEachVector_-1)),
				numberOfVectorsInHashTable(0)
				{


	nBitsPerSection = 0;
	{
		unsigned int tmp = sectionSize - 1;
		while (tmp > 0) {
			nBitsPerSection += 1;
			tmp = tmp >> 1;
		}
//		CHECK (nBitsPerSection*nSectionsPerHash < 32);// TODO maybe it was nicer if this was available
	}

	permutationArraySize = (nHashes
			/ (sizeOfEachVector / (nSectionsPerHash * sectionSize)) + 1)
			* sizeOfEachVector;
	//number of hashes that one iteration over the vector can compute
	//number of iterations needed to compute all the hashes.
	nPermutatedData = permutationArraySize;

//	nPermutatedData = nSectionsPerHash*sectionSize*nHashes;
//	permutationArraySize = (nPermutatedData/K_ + 1)*K_;
	hashPermutations = new int[permutationArraySize]; //TODO convert to vector

	for (int i = 0; i < permutationArraySize; i++) {
		hashPermutations[i] = i % sizeOfEachVector;
	}

//
//	for (int i = 0; i < nHashes; i++) {
//		theHashTables->push_back(new HashTableType());
//	}
//
//	////LOG(INFO) << "the hashtable size " << theHashTables->size();
//
//	this->hashTables = theHashTables;

	manRealHashWidth = (1 << (nBitsPerSection * nSectionsPerHash));
	manHashWidth = (1 << (nBitsPerSection * nSectionsPerHash)) + 1; // the last one is for the very last house!

	//  LOG(INFO) << "hashWidthes!" << manRealHashWidth << " " << manHashStoreWidth;

//	manualHashTable = new int[nHashes * manHashWidth];
//	manualHashTableStore = new int[nHashes * manHashStoreWidth];

}


void invertHashes2(int M_, int nHashes, vector<int> * allCurHashesInverted,
		vector<int> * allCurHashes) {
	////LOG(INFO)<< "inverting HashArray";
	int blocksize = 16;
	for (int j = 0; j < nHashes; j += blocksize) {
		int maxL = j + blocksize < nHashes ? j + blocksize : nHashes;
		for (int l = j; l < maxL; l++) {
			allCurHashesInverted[l].resize(M_);
			//allCurHashesInverted[l].insert(allCurHashesInverted[l].begin(), M_, 0);
		}
		for (int i = 0; i < M_; i += blocksize) {
			int maxK = i + blocksize < M_ ? i + blocksize : M_;
			for (int k = i; k < maxK; k++) {
				for (int l = j; l < maxL; l++) {
					allCurHashesInverted[l][k] = allCurHashes[k][l];
				}
			}
		}
	}
}


void invertMatrixArrayOfVectorForm2(int N_, int nHashes,
		vector<int> * allCurHashesInverted, vector<int> * allCurHashes) {
	////LOG(INFO)<< "inverting matrix";
	for (int i = 0; i < nHashes; i++) {
		allCurHashesInverted[i].resize(N_);
	}
	int blocksize = 16;
	for (int i = 0; i < N_; i += blocksize) {
		for (int j = 0; j < nHashes; j += blocksize) {
			int maxK = i + blocksize < N_ ? i + blocksize : N_;
			int maxL = j + blocksize < nHashes ? j + blocksize : nHashes;
			for (int k = i; k < maxK; k++) {
				for (int l = j; l < maxL; l++) {
					allCurHashesInverted[l][k] = allCurHashes[k][l];
				}
			}
		}
	}
}



template<typename Dtype>
void WTACacheAwareSecondTry<Dtype>::makeNewHashTable(const FakeSpan<Dtype> & data) {

	int n = data.getnSamples();
	numberOfVectorsInHashTable = n;

	calculateNewHashPermutations();
	const Dtype* weights = data.getData();
	int N_= manHashStoreWidth  = n;
	int K_ = sizeOfEachVector;

	manualHashTable.resize (nHashes * manHashWidth);
	manualHashTableStore.resize (nHashes * manHashStoreWidth);

	vector<vector<int>> allCurHashes;
	allCurHashes.resize(N_);

	for (int i = 0; i < N_; i++) {
		allCurHashes[i].reserve(nHashes);
		getWTAHashFaster(allCurHashes[i], weights + K_ * (size_t(i)));
	}

	vector<int> allCurHashesInverted[nHashes];
	invertMatrixArrayOfVectorForm2(n, nHashes,
			allCurHashesInverted, allCurHashes.data());

	putInvertedHashesIntoManualTable(allCurHashesInverted);
}


template<typename Dtype>
void WTACacheAwareSecondTry<Dtype>::getWTAHashFaster(std::vector<int> &hashes,
		const Dtype* data) {
	int curPermPoint = 0;
	int k = 0; //counter for eachSection
	Dtype permutatedData[sizeOfEachVector];
	int maxIdx;
	Dtype maxVal;

	while (curPermPoint < permutationArraySize) { // do this until we have no more permutations
		for (int i = 0; i < sizeOfEachVector; i++) // permute the array once
				{
			permutatedData[hashPermutations[curPermPoint++]] = data[i];
		}
		int maxL =
				(sizeOfEachVector / (nSectionsPerHash * sectionSize)
						< nHashes - hashes.size()) ?
						(sizeOfEachVector / (nSectionsPerHash * sectionSize)) :
						(nHashes - hashes.size()); // are we going through the whole array or only some of it?
		int currentHashPoint = 0;
		for (int l = 0; l < maxL; l++) {
			int hash = 0;
			for (int j = 0; j < nSectionsPerHash; j++) {
				int maxIdx = 0;
				Dtype maxVal = 0;

				maxVal = permutatedData[currentHashPoint++];
				{
					for (int k = 1; k < sectionSize; k++) {
						Dtype val = permutatedData[currentHashPoint++];
						if (val > maxVal) {
							maxIdx = k;
							maxVal = val;
						}
					}
					hash = hash << nBitsPerSection;
					hash += (maxIdx);
				}
			}
			hashes.push_back(hash);
		}

	}
}


template<typename Dtype>
void WTACacheAwareSecondTry<Dtype>::putInvertedHashesIntoManualTable(
		vector<int> * allCurHashesInverted) {
	int N_ = numberOfVectorsInHashTable;
	//putInvertedHashesIntoHashTable (allCurHashesInverted);
	//LOG(INFO)<< "putting hashes in manual hashtable";
	for (uint64_t j = 0; j < nHashes; j++) {
		//	  LOG(INFO) << j;
		for (unsigned int i = 0; i < manHashWidth; i++) {
			manualHashTable[j * manHashWidth + i] = 0;
		}
		for (unsigned int i = 0; i < N_; i++) {
			manualHashTable[j * manHashWidth + size_t(allCurHashesInverted[j][i])]++;
		}
		//	  if (j != 0){
		//	      manualHashTable[j * manHashWidth] = manualHashTable[j * manHashWidth-1];
		//	  }
		//
		for (unsigned int i = 0; i < manHashWidth - 1; i++) {
			manualHashTable[j * manHashWidth + i + 1] = manualHashTable[j
					* manHashWidth + i]
					+ manualHashTable[j * manHashWidth + i + 1];
		}
		for (unsigned int i = 0; i < N_; i++) {
			//	      if (j >= 499){
			//		  LOG(INFO) << allCurHashesInverted[j][i];
			//	      }
			int idx = --manualHashTable[j * manHashWidth
					+ allCurHashesInverted[j][i]];
			//	      if (j >= 499){
			//		  LOG(INFO) << " idx is:" << idx;
			//	      }
			manualHashTableStore[j * manHashStoreWidth + idx] = i;
		}
	}
}

template<typename Dtype>
void WTACacheAwareSecondTry<Dtype>::lookUpHashForTheseVectors(
		const FakeSpan<Dtype> & data, std::vector <std::vector<int> > & matches) {

	//	HashTableListType * ptrHashTables = (HashTableListType *) hashTables;

	int K_ = sizeOfEachVector;
	int N_ = numberOfVectorsInHashTable;
	int n = data.getnSamples();
	int M_ = n;

	vector<int> allCurHashes[M_];

	//LOG(INFO) << "getting the hashes";
	for (int i = 0; i < M_; i++) {
		allCurHashes[i].reserve(nHashes);
		getWTAHashFaster(allCurHashes[i], data.getData() + K_ * i);
	}
	//  {
	//    computeAllHashes_GPUAndGiveBackAsVector(M_,data,allCurHashes);
	//  }
	lookupHashTableForTheseHashes(matches, allCurHashes, n);
}

template<typename Dtype>
void WTACacheAwareSecondTry<Dtype>::lookupHashTableForTheseHashes(
		vector< vector<int> > &matchedWeights,  const vector<int> * allCurHashes, const int M_) {
	int K_ = sizeOfEachVector;
	int N_ = numberOfVectorsInHashTable;

	vector<int> matchings[nHashes];
	vector<unsigned int> matchingsIdx[nHashes];
	vector<int> allCurHashesInverted[nHashes];

	////LOG(INFO) << "inverting the hashes";
	//lookUpHits(allCurHashes, matchingsIdx, matchings);
	invertHashes2(M_, nHashes, (vector<int> *) allCurHashesInverted,
			(vector<int> *) allCurHashes);

	//lookUpHashes ((vector <int> * )matchings,(vector <unsigned> * ) matchingsIdx, allCurHashesInverted);
	////LOG(INFO) << "looking up hashes manually";

	{
		for (int j = 0; j < nHashes; j++) {

			matchings[j].reserve(M_ * 2);
			matchingsIdx[j].reserve(M_ + 2);
			for (int i = 0; i < M_; i++) {
				{ // loop for prefetching stuff.
					do {
						int lookForward = 2;
						int cacheLineWidth = 16;
						int pi = (i + lookForward) % M_;
						int pj = j;
						if (pi + lookForward * 2 >= M_) {
							// should I precache?
						}
						if (pi < i) {
							pj = j + 1;
							if (pj >= nHashes) {
								break;
							}
							for (int pit = 0; pit < manHashWidth; pit +=
									cacheLineWidth) {
								_mm_prefetch(
										manualHashTable.data()
												+ (pj + 1) * manHashWidth + pit,
										_MM_HINT_T1);
							}

						}
						auto prematching = manualHashTable[pj * manHashWidth
								+ allCurHashesInverted[pj][pi]];
						auto prematchingNext = manualHashTable[pj * manHashWidth
								+ allCurHashesInverted[pj][pi] + 1];
						for (auto pit = prematching; pit < prematchingNext;
								pit += cacheLineWidth) {
							_mm_prefetch(
									(char* )(manualHashTableStore.data()
											+ j * manHashStoreWidth + pit),
									_MM_HINT_T0);
						}
					} while (false); // my type of goto :)
				}

				auto matching = manualHashTable[j * manHashWidth
						+ allCurHashesInverted[j][i]]; // get the stuff
				auto matchingNext = manualHashTable[j * manHashWidth
						+ allCurHashesInverted[j][i] + 1]; // get the end pointer
				matchingsIdx[j].push_back(matchings[j].size()); // record the position
				for (auto it = matching; it < matchingNext; ++it) {
					//  	      auto hash = (j*manHashStoreWidth+it);
					//  	      ////LOG(INFO) << hash;
					matchings[j].push_back(
							manualHashTableStore[j * manHashStoreWidth + it]); // push into the matched data thingi

				}
			}
			matchingsIdx[j].push_back(matchings[j].size()); // push back the very end size (this tells us were the table finishes.

		}

	}

	////LOG(INFO) << "inverting matches";

	vector<int> matchingsInverted[M_];
	vector<unsigned int> matchingsIdxInverted[M_];

	{
		int blocksize = 32;
		for (int i = 0; i < M_; i += blocksize) {
			int maxK = i + blocksize < M_ ? i + blocksize : M_;
			{
				for (int k = i; k < maxK; k++) {
					matchingsIdxInverted[k].push_back(
							matchingsInverted[k].size());
				}
			}
			for (int j = 0; j < nHashes; j += blocksize) {
				int maxL = j + blocksize < nHashes ? j + blocksize : nHashes;
				for (int k = i; k < maxK; k++) {
					for (int l = j; l < maxL; l++) {
						for (int m = matchingsIdx[l][k];
								m < matchingsIdx[l][k + 1]; m++) {
							matchingsInverted[k].push_back(matchings[l][m]);
						}
						matchingsIdxInverted[k].push_back(
								matchingsInverted[k].size());
					}
				}
			}
		}
	}

	{
		////LOG(INFO) << "counting matches manual";

		//    int primes []= {1,7919,8291,9439,12011,10459,14947,19219};
		int crappyRand = 0;
		int nkookoos = 1;
		int hashes[nkookoos];
		int lookAhead = 2;
		int cacheLineWidth = 16;


		unsigned myHashMap[mapSize];
		matchedWeights.resize(M_);
		for (int i = 0; i < M_; i++) {
			memset(myHashMap, 0, mapSize * sizeof(int));
			for (int j = 0; j < nHashes; j++) {
				for (int k = matchingsIdxInverted[i][j];
						k < matchingsIdxInverted[i][j + 1]; k++) {
					{
						//int diffCount = k - matchingsIdxInverted[i][j]
						//  	    	    if ((diffCount%cacheLineWidth) == 0)
						//		    {
						//		      int prek = (k + lookAhead*cacheLineWidth);
						//		      int prei = i;
						//		      int prej = j;
						//		      if (prek > matchingsIdxInverted[i][j+1]){
						//			  prek = matchingsIdxInverted[i][j+1];
						//		      }
						//		    }
						//_mm_prefetch(&matchingsInverted[prei][prek],_MM_HINT_T2);

					}

					unsigned int theOne = matchingsInverted[i][k]; // that we are trying to count up.
					{
						int curHash = theOne % mapSize;
						int oldval = myHashMap[curHash];
						int curid = myHashMap[curHash] & idMask;
						int curCount = myHashMap[curHash] & countMask;
						if (curid == theOne) {
							myHashMap[curHash] += addOne;
						} else if (curCount == 0) {
							myHashMap[curHash] = addOne | theOne;
						} else
							myHashMap[curHash] -= addOne;

					}

				}
			}

			std::nth_element(myHashMap, myHashMap + numberToAccept,
					myHashMap + mapSize, compareCompactHashMatch2_3);
			int j = 0;
			matchedWeights[i].clear();
			for (j = 0; j < numberToAccept; j++) {
				matchedWeights[i].push_back(myHashMap[j] & idMask);
			}

			for (; j < numberToAccept; j++) {
				printf(
						"no matches in inner_product_hashed_layer.cpp in get matches, fix me!");
				exit(0);
				matchedWeights[i].push_back(-1);
			}
		}
	}

}



template<typename Dtype>
void WTACacheAwareSecondTry<Dtype>::calculateNewHashPermutations() {

	int * currLoc = hashPermutations;

	for (int i = 0; i < nPermutatedData; i++) {

		int swapper = var_uniform(); // the guy who will swap his position
		int temp = currLoc[swapper];
		currLoc[swapper] = hashPermutations[i];
		hashPermutations[i] = temp;

		if (i % sizeOfEachVector == sizeOfEachVector - 1) {
			currLoc = currLoc + sizeOfEachVector;
		}
	}

}


template class WTACacheAwareSecondTry<float>;


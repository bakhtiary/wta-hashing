#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <vector>

#include <DotProductHasher.hpp>
#include "gtest/gtest.h"
#include "util/mathematics.h"

using namespace std;

class WTAHasherTest : public ::testing::Test {
public:

	void calculateNewHashPermutations(WTAHasherSimple<float> & tester){
		tester.calculateNewHashPermutations();
	}

	int permutationArraySize (WTAHasherSimple<float> & tester){
		return tester.permutationArraySize;
	}

	int nPermutatedData(WTAHasherSimple<float> & tester){
		return tester.nPermutatedData;
	}

	int * hashPermutations(WTAHasherSimple<float> & tester){
		return tester.hashPermutations;
	}


};

TEST_F(WTAHasherTest, RandomPermutation) {
	int sizeOfEachVector = 2000;
	int nHashes = 1000;
	int nSectionsPerHash = 3;
	int sectionSize = 8;
	WTAHasherSimple<float> tester(nSectionsPerHash,sectionSize,nHashes,100,sizeOfEachVector,1);
	calculateNewHashPermutations(tester);
	EXPECT_GE(permutationArraySize(tester),nPermutatedData(tester)); // We should have enough permutated data.
	EXPECT_EQ(0,permutationArraySize(tester)%sizeOfEachVector); // the number of permuted data should be a multiple of the number of elements
	EXPECT_EQ(nSectionsPerHash*sectionSize*nHashes, nPermutatedData(tester));

	for (int i = 0; i < permutationArraySize(tester); i+= sizeOfEachVector){
		vector <int> counter;
		counter.resize (sizeOfEachVector);
		for (int j = i; j < i+sizeOfEachVector; j++){
			counter[hashPermutations(tester)[j]]++;
		}
		for (int j = 0; j < sizeOfEachVector; j++){
			EXPECT_EQ(1,counter[j]);
		}
	}
}

TEST_F(WTAHasherTest, ComputeHashes) {
	int sizeOfEachVector = 2000;
	int nHashes = 1000;
	int nSectionsPerHash = 3;
	int sectionSize = 8;

	WTAHasherSimple<float> tester(nSectionsPerHash,sectionSize,nHashes,100,sizeOfEachVector,1);
	calculateNewHashPermutations(tester);

	float * data = new float [sizeOfEachVector];
	boost::variate_generator<boost::mt19937,boost::normal_distribution<> > var_nor(boost::mt19937(),boost::normal_distribution<>(0.0, 1.0));
	for (int i = 0; i < sizeOfEachVector; i++){
		data[i] = var_nor();
	}

	vector <int >  hashes;


	tester.computeHashes(data,hashes);

	int * curLoc = hashPermutations(tester);
	for (int i = 0; i < hashes.size(); i++){
		int hash = hashes[i];
		for (int j = 0; j < nSectionsPerHash; j++){
			int curMaxId = (hash & 0x01c0 ) >> 6;
			hash = hash << 3;
			for (int k = 0; k < sectionSize; k++){
				EXPECT_LE(data[curLoc[k] ],data[curLoc[curMaxId] ]);
			}
			curLoc += sectionSize;
		}
	}

}

TEST_F(WTAHasherTest, putDataIntoHashTable) {


}

TEST_F(WTAHasherTest, testSmallExampleWithSameData){

	int sizeOfEachVector = 100;
	int nHashes = 10;
	int nSectionsPerHash = 1;
	int sectionSize = 8;


	int nAccept = 2;
	int N = 10;
	int K = sizeOfEachVector;
	int T1 = N*K;

	//WTAHashCacheAware<float> hashTable(8,2,10000,nAccept,K,N);


	vector <float>  data(T1);
	FakeSpan <float> dataSpan(data.data(),N,K);
	boost::variate_generator<boost::mt19937,boost::normal_distribution<> > var_nor(boost::mt19937(1),boost::normal_distribution<>(0.0, 1.0));
	for (int i = 0; i < T1; i++){
		data[i] = var_nor();
	}

	WTAHasherSimple<float> tester(nSectionsPerHash, sectionSize, nHashes, nAccept, sizeOfEachVector,1);

	tester.makeNewHashTable(dataSpan);

	std::vector <vector<int> > matchedWeights;
	tester.lookUpHashForTheseVectors(dataSpan, matchedWeights);

	EXPECT_EQ(N,matchedWeights.size());
	int nTotal = 0;
	int nFail = 0;
	for (int i = 0; i < matchedWeights.size(); i++){
		EXPECT_EQ(nAccept,matchedWeights[i].size());
		bool found = false;
		for (int j = 0; j < matchedWeights[i].size(); j++){
			nTotal ++;
			int curid = matchedWeights[i][j];
			if (curid == i){
				found = true;
			}
		}
		EXPECT_EQ(true,found);
	}

	//std::cout << "failed this many times" << nFail << "out of" << nTotal << std::endl;
}

TEST_F(WTAHasherTest, testTopPicks){

	int sizeOfEachVector = 1000;
	int nHashes = 10000;
	int nSectionsPerHash = 4;
	int sectionSize = 8;

	int nAccept = 100;
	int N = 4096;
	int K = sizeOfEachVector;
	int M = 500;
	int T1 = N*K;
	int T2 = M*K;

	//WTAHashCacheAware<float> hashTable(8,2,10000,nAccept,K,N);


	vector <float> data(T1);
	boost::variate_generator<boost::mt19937,boost::normal_distribution<> > var_nor(boost::mt19937(1),boost::normal_distribution<>(0.0, 1.0));
	for (int i = 0; i < T1; i++){
		data[i] = var_nor();
	}

	vector <float> data2 (T2);
	for (int i = 0; i < T2; i++){
		data2[i] = var_nor();
	}


	WTAHasherSimple<float> tester(nSectionsPerHash, sectionSize, nHashes, nAccept, sizeOfEachVector,1);

	FakeSpan <float> dataSpan (data.data(),N,K);

	tester.makeNewHashTable(dataSpan);

	std::vector <vector<int> > matchedWeights;
	FakeSpan <float> data2Span (data2.data(),M,K);
	tester.lookUpHashForTheseVectors(data2Span, matchedWeights);

	EXPECT_EQ(M,matchedWeights.size());
	int nTotal = 0;
	int nFail = 0;
	for (int i = 0; i < matchedWeights.size(); i++){
		EXPECT_EQ(nAccept,matchedWeights[i].size());
		for (int j = 0; j < matchedWeights[i].size(); j++){
			nTotal ++;
			int curid = matchedWeights[i][j];
			float res = dotProduct(K,data2.data()+i*K,data.data()+curid*K);
			if (res < 0)
				nFail ++;
			//EXPECT_GE(res,0);
		}
	}

	//std::cout << "failed this many times" << nFail << "out of" << nTotal << std::endl;
	EXPECT_LE(nFail,100);
}

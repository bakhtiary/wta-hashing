#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <vector>

#include <DotProductHasher.hpp>
#include "gtest/gtest.h"
#include "util/mathematics.h"

using namespace std;

TEST(CacheAwareWTATest, testTopPicks){

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

	vector <float> data (T1);
	boost::variate_generator<boost::mt19937,boost::normal_distribution<> > var_nor(boost::mt19937(1),boost::normal_distribution<>(0.0, 1.0));
	for (int i = 0; i < T1; i++){
		data[i] = var_nor();
	}

	vector <float> data2 (T2);
	for (int i = 0; i < T2; i++){
		data2[i] = var_nor();
	}


	WTACacheAware<float> tester(nSectionsPerHash, sectionSize, nHashes, nAccept, sizeOfEachVector,1);

	FakeSpan <float> dataSpan (data.data(),N,K);
	tester.makeNewHashTable(dataSpan);

	std::vector <std::vector<int> > matchedWeights;
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

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <vector>
#include <iostream>
#include "../../include/DotProductHasher.hpp"


using namespace std;

int main(int argc, char *argv[])
{

    boost::mt19937 rng(2000);
    boost::variate_generator<boost::mt19937&,boost::normal_distribution<> > var_nor(rng,boost::normal_distribution<>(0.0, 1.0));

    uint64_t N = 600000;// number of vectors to be hashed
    uint64_t K = 4000;// size of each vector
    int M = 1;// number of vectors to be looked up

    int nToAccept = 10;
    int nSectionsPerHash = 4;
    int sectionSize = 8;
    int nHashes = 100;

    vector <float> A (N*K);
    vector <float> B (M*K);

    FakeSpan <float> As(A.data(),N,K);
    FakeSpan <float> Bs(B.data(),M,K);


    for (uint64_t i=((N-10000)*K); i < N*K; ++i)
    {
      A [i] = var_nor();
    }

//    for (int i=0; i < M*K; ++i)
//	{
//	  B [i] = var_nor();
//	}
    for (int i = 0; i < K; i++){
    	B[i] = A[i + (N-1000)*K];
    }

    cout << "arrays filled" << endl;

//    WTAHasherSimple<float> hasher(nSectionsPerHash,sectionSize,nHashes,nToAccept,K,0);
    WTACacheAwareSecondTry<float> hasher(nSectionsPerHash,sectionSize,nHashes,nToAccept,K,0);

    hasher.makeNewHashTable(As);

    cout << " hashTableBuilt" << endl;

    std::vector <std::vector<int> > ids;
    hasher.lookUpHashForTheseVectors(Bs,ids);

    cout << " arrays looked up" << endl;


    float nPos = 0;
	for (int i = 0; i < M; i++){
		for (int j = 0; j < nToAccept; j++){
			int id = ids[i][j];
			float sum = 0;
			for (int k = 0; k < K ; k++){
				sum += A[id*K + k] * B[i*K + k];
			}
			cout << sum << endl;
//			if (sum > 0.0)
//				nPos++;
		}
	}

//	cout << nPos /( M*nToAccept ) << endl;
    return 0;
}

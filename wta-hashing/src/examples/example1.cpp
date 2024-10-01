#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <vector>
#include <iostream>
#include "../../include/DotProductHasher.hpp"

using namespace std;

int main(int argc, char *argv[])
{

    boost::mt19937 rng;
    boost::variate_generator<boost::mt19937&,boost::normal_distribution<> > var_nor(rng,boost::normal_distribution<>(0.0, 1.0));

    int N = 10000;// number of vectors to be hashed
    int K = 2000;// size of each vector
    int M = 4000;// number of vectors to be looked up

    int nToAccept = 100;
    int nSectionsPerHash = 3;
    int sectionSize = 8;
    int nHashes = 1000;

    vector <float> A (N*K);
    vector <float> B (M*K);

    FakeSpan <float> As(A.data(),N,K);
    FakeSpan <float> Bs(B.data(),M,K);


    for (int i=0; i < N*K; ++i)
    {
      A [i] = var_nor();
    }

    for (int i=0; i < M*K; ++i)
	{
	  B [i] = var_nor();
	}

//    WTAHasherSimple<float> hasher(nSectionsPerHash,sectionSize,nHashes,nToAccept,K,0);
    WTACacheAware<float> hasher(nSectionsPerHash,sectionSize,nHashes,nToAccept,K,0);

    hasher.makeNewHashTable(As);

    std::vector <std::vector<int> > ids;
    hasher.lookUpHashForTheseVectors(Bs,ids);

    hasher.lookUpHashForTheseVectors(Bs,ids);

	for (int i = 0; i < M; i++){
		for (int j = 0; j < nToAccept; j++){
			int id = ids[i][j];
			float sum = 0;
			for (int k = 0; k < K ; k++){
				sum += A[id*K + k] + B[i*K + k];
			}
			cout << sum << endl;
		}
	}

    return 0;
}

#include <algorithm>
#include <DotProductHasher.hpp>


template <typename Dtype>
WTAHasherSimple<Dtype>::WTAHasherSimple(int nSectionsPerHash,int sectionSize,int nHashes,
		int numberToAccept, int sizeOfEachVector_, int seed):
		var_uniform(boost::mt19937(seed),boost::uniform_int<>(0,sizeOfEachVector_-1))
		{
	this->nHashes = nHashes;
	this->nSectionsPerHash = nSectionsPerHash;
	this->sectionSize = sectionSize;
	this->sizeOfEachVector = sizeOfEachVector_;
	this->nToAccept = numberToAccept;
	float epsilon = 0.001;
	{
		nBitsPerSection = 0;
		unsigned int tmp = sectionSize-1;
		while (tmp > 0){
			nBitsPerSection += 1;
			tmp = tmp >> 1;
		}
	}

	nPermutatedData = nSectionsPerHash*sectionSize*nHashes;
	permutationArraySize = (nPermutatedData/sizeOfEachVector_ + 1)*sizeOfEachVector_;
	hashPermutations = new int [permutationArraySize];

	for (int i = 0; i < permutationArraySize; i++){
		hashPermutations[i] = i % sizeOfEachVector;
	}

		};

template <typename Dtype>
void WTAHasherSimple<Dtype>::makeNewHashTable(const FakeSpan<Dtype> & data){

	int n = data.getnSamples();

	calculateNewHashPermutations();

	std::vector <std::vector <int> > allHashes;
	allHashes.resize (n);
	for (int i = 0; i < n; i++){
		computeHashes(data.data()+i*sizeOfEachVector,allHashes[i]);
	}

	//put hashes into table:

	theHashTables.resize(nHashes);
	for (auto & aTable: theHashTables){
		aTable.clear();
	}

	for (int i = 0; i < allHashes.size(); i++){
		for (int j = 0; j < allHashes[i].size(); j++){
			int hash = allHashes[i][j];
			(theHashTables[j]).insert(std::make_pair(hash,i) );
		}
	}
}

class HashMatch{
public:
	int weightID;
	int strength;
	HashMatch(int id, int str){
		this->weightID = id;
		this->strength = str;
	}
	HashMatch(){
		this->weightID = 0;
		this->strength = 0;
	}
};

bool compareHashMatch(HashMatch  p1, HashMatch p2) {
	// return "true" if "p1" is ordered before "p2", for example:
	return p1.strength > p2.strength;
}


template <typename Dtype>
void WTAHasherSimple<Dtype>::lookUpHashForTheseVectors(const FakeSpan<Dtype> & data, std::vector <std::vector <int> > & matches){


	////LOG(INFO) << "counting matches";
	int n;
	int M_ = n = data.getnSamples();


	std::vector <std::vector <int> > allHashes;
	allHashes.resize(n);
	matches.resize(n);
	for (int i = 0; i < n; i++){
		computeHashes(data.data()+i*sizeOfEachVector,allHashes[i]);
		std::unordered_map <int,int>counterHash;
		for (int j = 0; j < allHashes[i].size(); j++ ){
			auto matches = theHashTables[j].equal_range(allHashes[i][j]);
			for (auto & iter = matches.first;iter != matches.second;iter++){
				counterHash[iter->second]++;
			}
		}

		std::vector <HashMatch> allMatches;
		for (auto &it : counterHash){
			allMatches.push_back(HashMatch(it.first,it.second));
		}

		std::nth_element(allMatches.begin(),allMatches.begin()+nToAccept, allMatches.end(),compareHashMatch);

		matches[i].resize(nToAccept);
		int j;
		for (j=0; j < nToAccept && j < allMatches.size(); j++){
			matches[i][j] = (allMatches[j].weightID);
		}
		matches[i].resize(j);
	}
}

template <typename Dtype>
void WTAHasherSimple<Dtype>::computeHashes(const Dtype * data, std::vector < int> & hashes){
	int biasCounter = 0;
	int currentHashPoint = 0;
	hashes.resize(nHashes);
	for (int i = 0;i < nHashes; i++){
		int hash = 0;
		for (int j = 0; j < nSectionsPerHash;j++){
			int maxIdx = 0;
			Dtype maxVal = data[hashPermutations[currentHashPoint++]];
			for (int k = 1;k < sectionSize; k++){
				int id = hashPermutations[currentHashPoint++];
				Dtype val = data[id];
				if (val > maxVal){
					maxIdx = k;
					maxVal = val;
				}
			}
			hash = hash << nBitsPerSection;
			hash += (maxIdx);
		}
		hashes[i] = hash;
	}
}

template <typename Dtype>
void WTAHasherSimple<Dtype>::calculateNewHashPermutations(){


	int * currLoc = hashPermutations;
	for (int i = 0; i < nPermutatedData; i++ ){

		int swapper = var_uniform(); // the guy who will swap his position
		int temp = currLoc[swapper];
		currLoc[swapper] = hashPermutations[i];
		hashPermutations[i] = temp;

		if (i % sizeOfEachVector  == sizeOfEachVector-1){
			currLoc = currLoc+sizeOfEachVector ;
		}
	}
}


template class WTAHasherSimple<float>;


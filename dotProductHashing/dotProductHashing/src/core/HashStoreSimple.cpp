#include <HashStore.hpp>

void HashStoreSimple::record(std::vector <std::vector <int> > allHashes){
	theHashTables.resize(allHashes[0].size());
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


void HashStoreSimple::lookup(std::vector <std::vector <int> > allHashes,std::vector <std::vector <int> > & votes){
	votes.reserve(allHashes.size());
	for (int i = 0; i < allHashes.size(); i++){
		for (int j = 0; j < allHashes[i].size(); j++ ){
			auto matches = theHashTables[j].equal_range(allHashes[i][j]);
			for (auto & iter = matches.first;iter != matches.second;iter++){
				votes[i].push_back(iter->second);
			}
		}
	}

}

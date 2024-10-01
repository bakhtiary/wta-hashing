/*
 * LEMSimple.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: amir
 */

#include <memory>
#include <vector>
#include <algorithm>
#include <DotProductHasher.hpp>
#include <unordered_map>

using namespace std;

bool idValcmp(const IDValPair <float> &a, const IDValPair<float> &b){
	return a.val > b.val;
}

template <typename Dtype>
unique_ptr<vector<Dtype> >invert (const std::vector<Dtype> & data,int sizeOfEachVector){
	auto retval = unique_ptr<vector<Dtype>> (new vector<Dtype> ());

	retval->reserve (data.size());

	int nSamples = data.size()/sizeOfEachVector;
	for (int i = 0; i < data.size(); i++){
		int row = i%nSamples;
		int col = i/nSamples;
		retval->push_back(data[row*sizeOfEachVector + col]);
	}


	return retval;
}


template <typename Dtype>
void getTopElementsAndIds (vector <IDValPair<Dtype>> & curData, vector <IDValPair<Dtype>> & topVals,int n){

	//gsl::array_view <int> t;

	topVals.clear();
	topVals.reserve(n);

	nth_element(curData.begin(),curData.begin()+n,curData.end(),idValcmp);

	topVals.insert(topVals.begin(),curData.begin(),curData.begin()+n);
}



template<typename Dtype>
LEMSimple<Dtype>::LEMSimple(int nElemToKeep_,int numElemToMultiply_,int numberToAccept_,int sizeOfEachVector_):
nElemToKeep(nElemToKeep_),nElemToMultiply(numElemToMultiply_),sizeOfEachVector(sizeOfEachVector_),numberToAccept(numberToAccept_){

}


//vector<IDValPair<float> > * getIDValVector(vector <float> ::iterator & start,vector <float>:: iterator & end){
//	auto retval = new vector<IDValPair<float> > ();
//	int i = 0;
//	for (auto j = start; j != end; j++){
//		retval->push_back(IDValPair<float>{i++,(*j)});
//	}
//	return retval;
//}


void fillIDValVector(vector <float> ::const_iterator & start,const vector <float>::const_iterator & end
		, vector<IDValPair<float> > & retval){
	retval.clear();
	int i = 0;
	for (auto j = start; j != end; j++){
		retval.push_back(IDValPair<float>{i++,(*j)});
	}
}

void fillIDValVector(vector <float> ::iterator & start,const vector <float>::iterator & end
		, vector<IDValPair<float> > & retval){
	retval.clear();
	int i = 0;
	for (auto j = start; j != end; j++){
		retval.push_back(IDValPair<float>{i++,(*j)});
	}
}


//unique_ptr <vector<float> >getIDValVector2(){
//	auto retval = unique_ptr <vector<float> >( new vector<float > ());
//	int i = 0;
////	for (auto j = start; j != end; j++){
////		retval->push_back(IDValPair<float>{i++,(*j)});
////	}
//	return retval;
//}


template<typename Dtype>
void LEMSimple<Dtype>::makeNewHashTable(const FakeSpan<Dtype> & dataIn){
	vector <Dtype> data (dataIn.begin(),dataIn.end());
	auto sampleInverted = invert<Dtype> (data,sizeOfEachVector);

//	auto sampleInverted = unique_ptr <vector<float> > (new vector<float> (data));

	int nSamples = data.size()/sizeOfEachVector;

	store.resize(sizeOfEachVector);

	vector<IDValPair<float> > allIds;

	for (int i = 0; i < sizeOfEachVector; i++ ){

		auto start = sampleInverted->begin()+i*nSamples;
		auto end = sampleInverted->begin() + (i+1)*nSamples;

		fillIDValVector (start,end ,allIds );

		getTopElementsAndIds(allIds,store[i],nElemToKeep);

	}
}

template<typename Dtype>
void LEMSimple<Dtype>::lookUpHashForTheseVectors(const FakeSpan<Dtype> & dataIn,std::vector <std::vector<int> > & matches){

	vector <Dtype> data (dataIn.begin(),dataIn.end());
	int nSamples = data.size()/sizeOfEachVector;

	matches.resize (nSamples);

	vector<IDValPair<Dtype> > topElems;
	vector<IDValPair<Dtype> > allIds;

	for (int i = 0; i < nSamples; i++){
		auto start = data.begin() + sizeOfEachVector*i;
		auto end = data.begin() + sizeOfEachVector*(i+1);

		fillIDValVector(start,end, allIds);

		getTopElementsAndIds(allIds,topElems,nElemToMultiply);

		unordered_map <int,float> sums;
		for (auto & elem:topElems){
			auto listOfMatches = this->store[elem.id];
			for (auto & matchedElem:listOfMatches){
				 Dtype t = elem.val*matchedElem.val;
				 sums[matchedElem.id] += t;
			}
		}

		auto sumsVec = getIDValVector ( sums );
		getTopElementsAndIds((*sumsVec),topElems,numberToAccept);

		for (auto top : topElems){
			matches[i].push_back(top.id);
		}
	}
}

template class LEMSimple<float>;


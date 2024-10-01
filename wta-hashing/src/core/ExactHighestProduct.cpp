/*
 * ExactHighestProduct.cpp
 *
 *  Created on: Nov 10, 2015
 *      Author: amir
 */

#include <DotProductHasher.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "utility.h"


template <typename Dtype>
ExactHighestProduct<Dtype>::ExactHighestProduct(int numberToAccept,int sizeOfEachVector_)
	:numberToAccept(numberToAccept),sizeOfEachVector(sizeOfEachVector_)
{}

using namespace boost::numeric::ublas;

template <typename Dtype>
void ExactHighestProduct<Dtype>::makeNewHashTable(const FakeSpan<Dtype> & data){
	store.clear();
	store.insert(store.begin(),data.data(), data.end());
}

template <typename Dtype>
void getTopElemetIds(matrix_row<matrix<Dtype> > & mr,int numberToAccept,std::vector<int> & matches){
	matches.clear();
	std::vector <Dtype> counts2(mr.begin(),mr.end());
	std::vector <Dtype> countsCopy = counts2;
	nth_element( counts2.begin(), counts2.end()-numberToAccept, counts2.end());
	Dtype threshold = counts2[counts2.size()-numberToAccept];
	std::vector<int> equals;
	equals.reserve (20);
	for (int i = 0; i < countsCopy.size(); i++){
		if (countsCopy [i] > threshold)
			matches.push_back(i);
		else if (countsCopy[i] == threshold){
			equals.push_back(i);
		}
	}
	for (int i = 0; i < equals.size() && matches.size() < numberToAccept; i++){
		matches.push_back(equals[i]);
	}

}


template <typename Dtype>
void ExactHighestProduct<Dtype>::lookUpHashForTheseVectors(const FakeSpan<Dtype> & data,std::vector <std::vector<int> > & matches){

	using namespace boost::numeric::ublas;

	int nStore = store.size()/sizeOfEachVector;
	matrix<Dtype> storeMat (nStore,sizeOfEachVector);
	std::copy(store.begin(),store.end(),storeMat.data().begin());

	int nData = data.size()/sizeOfEachVector;
	matrix<Dtype> dataMat (nData,sizeOfEachVector);
	std::copy(data.begin(),data.end(),dataMat.data().begin());

	matrix<Dtype> res = prod (dataMat, trans(storeMat));


	matches.resize(nData);
	for (int i = 0; i < nData; i++){
		matrix_row<matrix<Dtype> > mr (res, i);
		getTopElemetIds(mr,numberToAccept,matches[i]);
	}

}


template class ExactHighestProduct<float>;


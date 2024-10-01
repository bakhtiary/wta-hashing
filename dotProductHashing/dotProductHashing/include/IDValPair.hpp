#ifndef IDVALPAIR_HPP
#define IDVALPAIR_HPP

#include <unordered_map>
#include <vector>
#include <memory>

template <typename Dtype>

class IDValPair{
public:
	int id;
	Dtype val;
};


template <typename Dtype>
std::unique_ptr<std::vector <IDValPair<Dtype>>> getIDValVector(std::unordered_map <int,Dtype> & inMap){
	auto retval = std::unique_ptr<std::vector <IDValPair<Dtype> > > (new std::vector <IDValPair<Dtype> >);
	retval->reserve(inMap.size());
	for (auto & item : inMap){
		retval->push_back( IDValPair<Dtype> {item.first,item.second} );
	}
	return retval;
}


template <typename Dtype>
std::unique_ptr<std::vector <IDValPair<Dtype> > > getIDValVector(std::vector <Dtype> & inVec){
	auto retval = std::unique_ptr<std::vector <IDValPair<Dtype> > > (new std::vector <IDValPair<Dtype> >);
	retval.reserve(inVec.size());
	int i = 0;
	for (auto & item : inVec){
		retval->push_back(i++,item.second);
	}
	return retval;
}

//template <typename Dtype,class	iterator>
//std::unique_ptr<std::vector <IDValPair<Dtype> > > getIDValVector(const iterator start,const iterator end){
//	auto retval = std::unique_ptr<std::vector <IDValPair<Dtype> > > (new std::vector <IDValPair<Dtype> > () );
//	int i = 0;
//	for (auto j = start; j != end; j++){
//		retval->push_back(IDValPair<Dtype>{i++,(*j)});
//	}
//	return std::move(retval);
//}





#endif

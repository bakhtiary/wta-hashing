/*
 * Counter.hpp
 *
 *  Created on: Oct 4, 2015
 *      Author: amir
 */

#ifndef INCLUDE_COUNTER_HPP_
#define INCLUDE_COUNTER_HPP_

#include <vector>

class Counter{
public:
	virtual void count (const std::vector<int> & votes, std::vector<int> & counts) = 0;
	virtual int getIdBitsMask() = 0;
};

class CounterSimple:public Counter{
public:
	void count (const std::vector<int> & votes, std::vector<int> & counts){};
	int getIdBitsMask(){return 0;};
};



#endif /* INCLUDE_COUNTER_HPP_ */

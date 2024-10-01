#include "utility.h"
#include <algorithm>

using namespace std;

void getTopElemetIds(vector<int> & count,int numberToAccept,vector<int> & matches){
	matches.clear();
	vector <int> counts2 = count;
	nth_element( counts2.begin(), counts2.begin()+numberToAccept, counts2.end());
	int threshold = counts2[numberToAccept];
	std::vector<int> equals;
	equals.reserve (20);
	for (int i = 0; i < count.size(); i++){
		if (count[i] > threshold)
			matches.push_back(i);
		else if (count[i] == threshold){
			equals.push_back(i);
		}
	}
	for (int i = 0; i < equals.size() && matches.size() < numberToAccept; i++){
		matches.push_back(equals[i]);
	}

}


//void getTopElemetIds(vector<float> & count,int numberToAccept,vector<int> & matches){
//	matches.clear();
//	vector <int> counts2 = count;
//	nth_element( counts2.begin(), counts2.begin()+numberToAccept, counts2.end());
//	int threshold = counts2[numberToAccept];
//	std::vector<int> equals;
//	equals.reserve (20);
//	for (int i = 0; i < count.size(); i++){
//		if (count[i] > threshold)
//			matches.push_back(i);
//		else if (count[i] == threshold){
//			equals.push_back(i);
//		}
//	}
//	for (int i = 0; i < equals.size() && matches.size() < numberToAccept; i++){
//		matches.push_back(equals[i]);
//	}
//
//}

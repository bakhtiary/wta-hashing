/*
 * mathematics.cpp
 *
 *  Created on: Sep 30, 2015
 *      Author: amir
 */

float dotProduct (int n,const float * a,const float * b){
	float out = 0;
	for (int i = 0; i < n; i++){
		out += a[i]*b[i];
	}
	return out;
}



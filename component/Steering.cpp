/*
 * Steering.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include "Steering.hpp"

using namespace casadi;

Steering::Steering(int nTimesteps, double maxChange, SX*t) : Steering::Component(nTimesteps, t){
	this->hasOde = false;
	this->maxChange = maxChange;
	this->isCntrld = true;
	this->name = "Steering";

	setUp();
}

void Steering::setUp(){
	this->u = SX::sym("u_"+getName());
	this->space_needed = nTimesteps;
	this->space_0 = 1;

	this->xu_max = Matrix<double>::ones(1,space_needed) * this->maxChange;
	this->xu_min = - xu_max;
	this->xu_init = Matrix<double>::zeros(1, space_needed) ;
}


SX Steering::getODE(){
	throw 42;
	return 0;
}

void Steering::setPlace(int beginning){
	this->place_u = Slice(beginning, beginning + space_needed);
	this->place_u0 = Slice(beginning, beginning + space_0);
	this->place_u1 = Slice(beginning+1, beginning + space_0);
}

Slice Steering::getPlace1(){
	return this->place_u1;
}

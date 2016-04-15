/*
 * Acceleration.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */


#include "Acceleration.hpp"

using namespace casadi;

Acceleration::Acceleration(int nTimesteps, double amin, double amax, SX* t) : Acceleration::Component(nTimesteps, t){
	this->isCntrld = true;
	this->hasOde = false;
	this->name = "Acceleration";
	this->amin = amin;
	this->amax = amax;

	setUp();
}

void Acceleration::setUp(){
	this->u = SX::sym("u_acc"+ std::to_string(this->iD));
	this->space_0 = 1;
	this->space_needed = nTimesteps;

	this->xu_max = Matrix<double>::ones(1,space_needed) *this->amax;
	this->xu_min = Matrix<double>::ones(1, space_needed) * this->amin;
	this->xu_min[0] = 0;
	this->xu_max[0] = 0;
	this->xu_init = Matrix<double>::zeros(1,space_needed);
}

void Acceleration::setPlace(int beginning){
	this->place_u = Slice(beginning, beginning+space_needed);
	this->place_u0 = Slice(beginning, beginning + space_0);
	this->place_u0 = Slice(beginning+1, beginning + space_0);
}

SX Acceleration::getODE(){
	throw 42;
	return 0;
}

double Acceleration::getAmin(){
	return amin;
}

Slice Acceleration::getPlace1(){
	return this->place_u1;
}

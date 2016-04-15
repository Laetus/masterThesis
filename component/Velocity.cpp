/*
 * Velocity.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include "Velocity.hpp"

using namespace casadi;


Velocity::Velocity(int nTimesteps, Acceleration* a, double maxV , double v0,  SX* t) : Velocity::Component(nTimesteps, t){
	this->hasOde = true;
	this->name = "Velocity";
	this->isCntrld = false;
	this->a = a;
	this->maxV = maxV;
	this->v0 = v0;
	setUp();
}

void Velocity::setUp(){
	this->x = SX::sym("x_"+ getName());
	this->space_needed = nTimesteps+1;
	this->space_0 =1;

	this->xu_max = Matrix<double>::ones(1,space_needed) * maxV;
	this->xu_min = Matrix<double>::zeros(1,space_needed);
	this->xu_init = Matrix<double>::ones(1,space_needed) * v0;
	this->xu_max[Slice(0, space_0)] = v0;
	this->xu_min[Slice(0, space_0)] = v0;
}

SX Velocity::getODE(){
	return a->getu();
}

void Velocity::setPlace(int beginning){
	this->place_x = Slice(beginning, beginning+ space_needed);
	this->place_x0 = Slice(beginning, beginning+ space_0);
}

double Velocity::getV0(){
	return this->v0;
}

Acceleration* Velocity::getAcceleration(){
	return this->a;
}

void Velocity::setXU_0(Matrix<double> xu_0) {
	this->xu_0 = xu_0;
	Matrix<double> error = getError(xu_0.size()) * 0.2778/2; //accuracy of 0.5 km/h
	this->xu_max[Slice(0, xu_0.size())] = xu_0 + error;
	this->xu_min[Slice(0, xu_0.size())] = xu_0 + error;
	this->xu_init[Slice(0, xu_0.size())] = xu_0 + error;
}

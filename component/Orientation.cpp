/*
 * Orientation.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include "Orientation.hpp"

using namespace casadi;

Orientation::Orientation(int nTimesteps, Velocity* v, Steering* s,
		double wheelBase, SX* t) :
	Orientation::Component(nTimesteps, t) {
	this->hasOde = true;
	this->name = "Orientation";
	this->isCntrld = false;
	this->velocity = v;
	this->steering = s;
	this->wheelBase = wheelBase;

	setUp();
}

void Orientation::setUp() {
	this->x = SX::sym("x_"+getName());
	this->space_needed = nTimesteps+1 ;
	this->space_0 = 1;

	this->xu_max = Matrix<double>::ones(1, space_needed) * M_PI/8;
	this->xu_min = -xu_max;
	this->xu_init = Matrix<double>::zeros(1, space_needed);
}

SX Orientation::getODE() {
	return (velocity->getx() / wheelBase) * tan(steering->getu()) ;
}

Velocity* Orientation::getVelocity() {
	return this->velocity;
}

void Orientation::setPlace(int beginning) {
	this->place_x = Slice(beginning, beginning + space_needed);
	this->place_x0 = Slice(beginning, beginning + space_0);
}

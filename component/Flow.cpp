/*
 * Flow.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: laetus
 */

#include "Flow.hpp"

Flow::Flow(int nTimesteps, SX* t) :
	Flow::Component(nTimesteps, t) {
	this->u = SX::sym("u_flow" + std::to_string(this->iD));
	this->x = SX::sym("q_flow" + std::to_string(this->iD));

	this->flowMax = std::numeric_limits<double>::max();
	this->flowMin = -std::numeric_limits<double>::max();

	setUp();

	this->controlled = false;
	this->parameter = true;
}

Flow::Flow(int nTimesteps, SX* t, double xu_max, double xu_min, double xu_0) :
	Flow::Component(nTimesteps, t) {
	this->u = SX::sym("u_flow" + std::to_string(this->iD));
	this->x = SX::sym("q_flow" + std::to_string(this->iD));

	this->flowMax = xu_max;
	this->flowMin = xu_min;
	this->flow0 = xu_0;

	setUp();

	this->controlled = false;
	this->parameter = true;
}


void Flow::setUp() {
	this->space_needed = nTimesteps;
	this->space_0 = 1;

	this->xu_min = Matrix<double>::ones(1, this->space_needed)
			* this->flowMin;
	this->xu_max = Matrix<double>::ones(1, this->space_needed)
			* this->flowMax;
	this->xu_0 = Matrix<double>::ones(1, this->space_0) * this->flow0;
	this->xu_min[0] = this->flow0;
	this->xu_max[0] = this->flow0;
	this->xu_init = Matrix<double>::ones(1, this->space_needed) * flow0;
}

std::string Flow::getName() {
	return "Flow_" + std::to_string(this->iD);
}

SX Flow::getODE() {
	return 0;
}

SX Flow::getFlowTo(Component* component) {
	if (component == this->A) {
		return -this->getFlow();
	}
	if (component == this->B) {
		return this->getFlow();
	} else {
		return SX::zeros(1, 1);
	}
}

SX Flow::getFlow() {
	if (this->controlled) {
		return this->u;
	} else {
		return this->x;
	}
}

void Flow::setIsControlled(bool isControlled) {
	this->controlled = isControlled;
}

bool Flow::isControlled() {
	return this->controlled;
}

void Flow::setComponentA(Component* a) {
	this->A = a;
}

void Flow::setComponentB(Component* b) {
	this->B = b;
}

Component* Flow::getComponentA() {
	return this->A;
}

Component* Flow::getComponentB() {
	return this->B;
}

void Flow::setPlace(int beginning) {
	if (controlled) {
		this->place_x = Slice(0, 0);
		this->place_x0 = Slice(0, 0);
		this->place_u = Slice(beginning, beginning + space_needed, 1);
		this->place_u0 = Slice(0, this->space_0);
	} else {
		this->place_x = Slice(beginning, beginning + this->space_needed, 1);
		this->place_x0 = Slice(0, this->space_0);
		this->place_u = Slice(0, 0);
		this->place_u0 = Slice(0, 0);
	}
}

bool Flow::isP() {
	return this->parameter;
}

void Flow::setP(bool p) {
	this->parameter = p;
}

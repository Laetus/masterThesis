/*
 * Lake.cpp
 *
 *  Created on: Nov 9, 2015
 *      Author: laetus
 */

#include <iostream>
#include <string>
#include <casadi/casadi.hpp>
#include "Lake.hpp"
#include "ExtFlow.hpp"
#include <list>

using namespace casadi;

Lake::Lake(int nTimesteps, double srfc, double h0, SX* t) :
	Lake::Waters(nTimesteps, t) {
	this->height = SX::zeros(1, nTimesteps);
	surface = srfc;
	this->flowList = new std::list<Flow*>();
	this->h0 = h0;
	this->hmax = std::numeric_limits<double>::max();
	this->hmin = 0;
	setUp();

}

Lake::Lake(int nTimesteps, double srfc, double h0, double hmax, SX* t) :
	Lake::Waters(nTimesteps, t) {
	this->height = SX::zeros(1, nTimesteps);
	surface = srfc;
	this->flowList = new std::list<Flow*>();
	this->h0 = h0;
	this->hmax = hmax;
	this->hmin = 0;

	setUp();

}

Lake::Lake(int nTimesteps, double srfc, double h0, double hmax, double hmin, SX* t) :
	Lake::Waters(nTimesteps, t) {
	this->height = SX::zeros(1, nTimesteps);
	surface = srfc;
	this->flowList = new std::list<Flow*>();
	this->h0 = h0;
	this->hmax = hmax;
	this->hmin = hmin;

	setUp();

}

void Lake::setUp() {

	this->u = SX::sym("u_lake" + std::to_string(this->iD));
	this->x = SX::sym("h_lake" + std::to_string(this->iD));
	this->space_needed = nTimesteps + 1;
	this->space_0 = 1;

	this->xu_max = Matrix<double>::ones(1, space_needed) * hmax;
	this->xu_min = Matrix<double>::ones(1, space_needed) * hmin;
	this->xu_init = Matrix<double>::ones(1, space_needed) * h0;
	this->xu_max[0] = h0;
	this->xu_min[0] = h0;
}

SX Lake::getHeight(int place) {
	return this->x;
}

double Lake::getSurface() {
	return this->surface;
}

SX Lake::getODE() {
	return (this->getTotalFlow()) / this->surface;
}

void Lake::addFlow(Flow* flow) {
	this->flowList->push_front(flow);
}

SX Lake::getTotalFlow() {
	SX res = SX::zeros(1, 1);
	for (std::list<Flow *>::iterator it = this->flowList->begin(); it
			!= this->flowList->end(); it++) {
		res += (*it)->getFlowTo(this);
	}
	return res;
}

void Lake::setHeight(int time, SX height) {
	if (time < nTimesteps) {
		this->height[time] = height;
	}
}

std::string Lake::print() {
	std::string res = "Lake surface: " + std::to_string(this->surface) + "\n";
	for (int i = 0; i < this->nTimesteps; i++) {
		res += "Timestep " + std::to_string(i) + ": ";
		res += "Height: " + std::to_string(this->getHeight(i).getValue()) + " ";
		res += "Flow: " + std::to_string(this->getTotalFlow().getValue())
				+ " " "\n";
	}

	return res;
}

std::string Lake::getName() {
	return "Lake" + std::to_string(this->iD);
}

std::list<Flow*>* Lake::getFlowList() {
	return this->flowList;
}

void Lake::setPlace(int beginning) {
	this->place_x = Slice(beginning, beginning + this->space_needed, 1);
	this->place_x0 = Slice(beginning, beginning + this->space_0);
	this->place_u = Slice(0, 0);
	this->place_u0 = Slice(0, 0);
}

bool Lake::isControlled() {
	return false;
}

MX Lake::getOptHeights(MX V, int t_act){
	MX height  = V[this->getPlace()];
	Matrix<double> h_opt = this->result_glob[Slice(t_act, std::min(t_act+ this->space_needed, result_glob.size()))];

	while(h_opt.size() < height.size() ){
		h_opt = vertcat(h_opt, h_opt[Slice(h_opt.size()-1, h_opt.size())]);
	}
	MX res = quad_form(height - h_opt);
	return res;
}

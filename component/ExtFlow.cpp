/*
 * ExtFlow.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: laetus
 */

#include "Flow.hpp"

ExtFlow::ExtFlow(int nTimesteps, SX* t, bool flowsIntoSystem) :
	ExtFlow::Flow(nTimesteps, t) {
	this->flowsIntoSystem = flowsIntoSystem;
	this->u = SX::sym("u_extFlow" + std::to_string(this->iD));
	this->x = SX::sym("q_extFlow" + std::to_string(this->iD));
	this->controlled = false;
	this->hasOde = false;
	this->givenValues = -1.0;
	setUp();
}

ExtFlow::ExtFlow(int nTimesteps, SX* t, bool flowsIntoSystem, double x_max,
		double x_min, double x0) :
	ExtFlow::Flow(nTimesteps, t, x_max, x_min, x0) {
	this->flowsIntoSystem = flowsIntoSystem;
	this->u = SX::sym("u_extFlow" + std::to_string(this->iD));
	this->x = SX::sym("q_extFlow" + std::to_string(this->iD));
	this->controlled = false;
	this->hasOde = false;
	this->givenValues = -1.0;
	setUp();
}

ExtFlow::ExtFlow(int nTimesteps, SX* t, bool flowsIntoSystem, std::string name) :
	ExtFlow::Flow(nTimesteps, t) {
	this->flowsIntoSystem = flowsIntoSystem;
	this->u = SX::sym("u_extFlow" + std::to_string(this->iD));
	this->x = SX::sym("q_extFlow" + std::to_string(this->iD));
	this->controlled = false;
	this->hasOde = false;
	this->readFromFile(name);
	setUp();
}

void ExtFlow::setUp() {
	this->space_needed = nTimesteps;
	this->space_0 = 1;
	if (this->givenValues.size() <= 1) {
		this->xu_min = Matrix<double>::ones(1, this->space_needed)
				* this->flowMin;
		this->xu_max = Matrix<double>::ones(1, this->space_needed)
				* this->flowMax;
		this->xu_0 = Matrix<double>::ones(1, this->space_0) * this->flow0;
		this->xu_min[0] = this->flow0;
		this->xu_max[0] = this->flow0;
		this->xu_init = Matrix<double>::ones(1, this->space_needed) * flow0;
	} else {
		if (!isRealtime) {
			this->xu_min = transpose(givenValues[Slice(0, space_needed)]);
		} else {
			this->xu_min = transpose(givenValuesRT[Slice(0, space_needed)]);
		}
		this->xu_max = xu_min;
		this->xu_init = xu_min;
		this->xu_0 = xu_min[Slice(0, space_0)];
	}
}

std::string ExtFlow::getName() {
	return "External_Flow_" + std::to_string(this->iD);
}

bool ExtFlow::flowsInto() {
	return this->flowsIntoSystem;
}

void ExtFlow::setComponent(Component* component) {
	if (this->flowsIntoSystem) {
		this->setComponentB(component);
	} else {
		this->setComponentA(component);
	}
}

Matrix<double> ExtFlow::getXU_Min(int t_act) {
	if (this->givenValuesRT.size() > 1) {
		Matrix<double> res = this->getXU_MinMax(t_act);
		res[0] = xu_min[0];
		return res;
	} else {
		return this->xu_min;
	}
}

Matrix<double> ExtFlow::getXU_Max(int t_act) {
	if (this->givenValuesRT.size() > 1) {
		Matrix<double> res = this->getXU_MinMax(t_act);
		res[0] = xu_max[0];
		return res;
	} else {
		return this->xu_max;
	}
}

Matrix<double> ExtFlow::getXU_MinMax(int t_act) {
	Matrix<double> result = Matrix<double>::zeros(1, this->space_needed);
	if (isRealtime) {
		result[Slice(0, space_needed)] = givenValuesRT[Slice(t_act + 1, t_act
				+ 1 + space_needed)];
	} else {
		result[Slice(0, space_needed)] = givenValues[Slice(t_act + 1, t_act + 1
				+ space_needed)];
	}
	return result;
}

void ExtFlow::readFromFile(std::string name) {
	std::vector<Matrix<double>*> vct(2);
	vct = {&givenValues, &givenValuesRT};
	Utils::readFromFile("data/" + name, &vct);
}

Matrix<double> ExtFlow::estimateNewPoint(Matrix<double> xu_initial, int t_act) {
	if (givenValuesRT.size() > 1) {
		if (isRealtime) {
			return givenValuesRT[t_act+nTimesteps];
		} else {
			return givenValues[t_act+nTimesteps];
		}
	} else {
		return xu_initial[xu_initial.size() - 1];
	}

}

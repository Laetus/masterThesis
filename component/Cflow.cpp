/*
 * Cflow.cpp
 *
 *  Created on: Nov 24, 2015
 *      Author: laetus
 */

#include "Cflow.hpp"

Cflow::Cflow(int nTimesteps, SX* t, double min, double max, double efficiency,
		double diff) :
	Cflow::Flow(nTimesteps, t, max, min, (min+max)/2) {
	this->u = SX::sym("u_cflow_" + std::to_string(this->iD));
	this->x = SX::sym("x_cflow_NOT_NEEDED" + std::to_string(this->iD));

	setUp();

	this->controlled = true;
	this->parameter = false;

	this->heightDifference = diff;
	this->powerCoefficient = Component::GRAV_ACC * Component::DENSITY_WATER * efficiency /1000000;

}

Cflow::Cflow(int nTimesteps, SX* t, double min, double max, double x0, double efficiency,
		double diff) :
	Cflow::Flow(nTimesteps, t, max, min, x0) {
	this->u = SX::sym("u_cflow_" + std::to_string(this->iD));
	this->x = SX::sym("x_cflow_NOT_NEEDED" + std::to_string(this->iD));

	setUp();

	this->controlled = true;
	this->parameter = false;

	this->heightDifference = diff;
	this->powerCoefficient = Component::GRAV_ACC * Component::DENSITY_WATER * efficiency / 1000000;

}


void Cflow::setUp() {

	this->space_needed = nTimesteps;
	this->space_0 = 1;

	this->xu_min = Matrix<double>::ones(1, this->space_needed) * this->flowMin;
	this->xu_max = Matrix<double>::ones(1, this->space_needed) * this->flowMax;
	this->xu_0 = Matrix<double>::ones(1, this->space_0) * this->flow0;
	this->xu_init = Matrix<double>::ones(1, this->space_needed) * flow0;

	this->xu_min[0] = this->xu_0;
	this->xu_max[0] = this->xu_0;
}

bool Cflow::isControlled() {
	return true;
}

std::string Cflow::getName() {
	return "Controlled_Flow_" + std::to_string(this->iD);
}

double Cflow::getHeightDifference() {
	return this->heightDifference;
}

double Cflow::getPowerCoefficient() {
	return this->powerCoefficient;
}

casadi::MX Cflow::calculatePower(casadi::MX V) {
	casadi::MX hA = V[this->A->getPlace()];
	casadi::MX hB = V[this->B->getPlace()];
	casadi::MX heightDiff = hA - hB + this->heightDifference;
	casadi::MX height = (heightDiff[Slice(0, heightDiff.size() - 1)]
			+ heightDiff[Slice(1, heightDiff.size())]) / 2;

	return V[this->getPlace()] * this->powerCoefficient * height;
}

casadi::Matrix<double> Cflow::calculateProducedPower() {
	casadi::Matrix<double> hA = this->A->getVopt();
	casadi::Matrix<double> hB = this->B->getVopt();
	casadi::Matrix<double> hDiff = hA - hB + this->heightDifference;
	casadi::Matrix<double> height = (hDiff[Slice(0, hDiff.size() - 1)]
			+ hDiff[Slice(1, hDiff.size())]) / 2;
	casadi::Matrix<double> res = this->result[Slice(0, height.size())] * height
			* this->powerCoefficient;
	return transpose(res);

}

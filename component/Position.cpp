/*
 * Position.cpp
 *
 *  Created on: Jan 12, 2016
 *      Author: laetus
 */

#include "Position.hpp"

using namespace casadi;

Position::Position(int nTimesteps, Orientation* orientation, double horizon,
		std::string x, bool isReg, SX *t) :
	Position::Component(nTimesteps, t) {
	this->isCntrld = false;
	this->hasOde = true;
	this->orientation = orientation;
	this->horizon = horizon;
	this->type = x;
	this->name = type + "Position";
	this->xu_0 = 0;
	this->isRegularCaseAssumption = isReg;

	setUp();
}

void Position::setUp() {
	this->x = SX::sym(type + "_dir" + std::to_string(this->iD));
	space_0 = 1;
	this->space_needed = nTimesteps + 1;

	if (type == "x") {
		this->xu_init = transpose(linspace(Matrix<double>::zeros(1, 1), Matrix<
				double>::ones(1, 1) * nTimesteps, nTimesteps + 1)
				* orientation->getVelocity()->getV0() * horizon / nTimesteps);
		this->xu_max = Matrix<double>::ones(1, space_needed)
				* std::numeric_limits<double>::max();
	}

	if (type == "y") {
		this->xu_init = Matrix<double>::zeros(1, space_needed);
		/*set y direction
		 *
		 * (Line width german motorway - width of modern car ) / 2 / Safety factor(here 2) )
		 */
		this->xu_max = Matrix<double>::ones(1, space_needed) * (3.15 - 2.16)
				/ 4;
	}

	this->xu_min = -xu_max;
	this->xu_min[Slice(0, space_0)] = this->xu_0;
	this->xu_max[Slice(0, space_0)] = this->xu_0;

	if (isRealtime && type == "x") {
		this->old_pred_pos = predecessor_posStorage[0]
				- (predecessor_posStorage[1] - predecessor_posStorage[0]);
		this->xu_max[Slice(1, space_needed)] = (getPredecessorPosition(0)
				- getSafetyDistance())[Slice(1, space_needed)];
	}

}

SX Position::getODE() {
	SX foo;
	if (type == "x") {
		foo = cos(orientation->getx());
	}
	if (type == "y") {
		foo = sin(orientation->getx());
	}
	SX tmp = orientation->getVelocity()->getx() * foo;
	return tmp;
}

void Position::setPlace(int beginning) {
	this->place_x = Slice(beginning, beginning + this->space_needed);
	this->place_x0 = Slice(beginning, beginning + this->space_0);
}

casadi::Matrix<double> Position::getSafetyDistance() {
	Matrix<double> result;
	Matrix<double> v_act;
	if (isRealtime) {
		v_act = orientation->getVelocity()->getXU_Init();
		v_act = Matrix<double>::ones(1, space_needed)
				* (orientation->getVelocity()->getXU_Init()[0]);
	} else {
		v_act = transpose((predecessor_posStorage[Slice(1, nTimesteps + 2)]
				- predecessor_posStorage[Slice(0, nTimesteps + 1)]) / (horizon
				/ nTimesteps));
	}
	result = (v_act * 3 * horizon / nTimesteps) + (0.5 * v_act * v_act / abs(
			orientation->getVelocity()->getAcceleration()->getAmin())) + 0.2;
	return result;
}

casadi::MX Position::getSafetyDistance(MX V) {
	MX result, v_act;
	if (type == "x") {
		v_act = V[this->orientation->getVelocity()->getPlace()];
	} else {
		v_act = Matrix<double>::zeros(space_needed, 1);
	}

	result = (v_act * 3 * horizon / nTimesteps) + (0.5 * v_act * v_act / abs(
			orientation->getVelocity()->getAcceleration()->getAmin())) + 0.2;
	return transpose(result);
}

casadi::Matrix<double> Position::getPredecessorPosition(int t_act) {
	if (isRealtime) {
		//Get actual position from list aka measurement
		Matrix<double> tmp = predecessor_posStorage[t_act];

		//Estimate the positions in the horizon
		Matrix<double> positions = Matrix<double>::ones(1, space_needed) * tmp;
		if (isRegularCaseAssumption) {
			regularCasePositionEstimation(&positions);
		} else {
			worstCasePositionEstimation(&positions);
		}
		old_pred_pos = tmp;
		return positions;
	} else {
		return transpose(predecessor_posStorage[Slice(0, space_needed)]);
	}
}

void Position::setPredecessor_pos(std::string path) {
	std::vector<Matrix<double>*> vct(1);
	vct = {&predecessor_posStorage};
	Utils::readFromFile(path, &vct);
	this->predecessor_posStorage = *vct[0];
}

void Position::regularCasePositionEstimation(Matrix<double>* positions) {
	// In this case, it is assumed, that the predecessor contains with constant speed.
	*positions = *positions + transpose(linspace(Matrix<double>::zeros(1, 1),
			Matrix<double>::ones(1, 1) * nTimesteps, nTimesteps + 1)
			* ((*positions)[0] - old_pred_pos));
}

void Position::worstCasePositionEstimation(Matrix<double> * positions) {
	// In this case, it is assumed, that the predecessor is not moving at the position it is right now
}

void Position::storeAndShift_XU(
		std::map<std::string, Matrix<double> > res_tact, int t_act) {
	Component::storeAndShift_XU(res_tact, t_act);
	if (type == "x") {
		this->xu_max[Slice(1, space_needed)] = (getPredecessorPosition(t_act
				+ 1) - getSafetyDistance())[Slice(1, space_needed)];
	}
}

void Position::setXU_0(Matrix<double> xu_0) {
	this->xu_0 = xu_0;
	Matrix<double> error;
	if (type == "x") {
		error = getError(xu_0.size()) * 0.2; // accuracy of +-20 cm
	} else {
		error = getError(xu_0.size()) * 0.02; // accuracy of +-2 cm
	}
	this->xu_max[Slice(0, xu_0.size())] = xu_0 + error;
	this->xu_min[Slice(0, xu_0.size())] = xu_0 + error;
	this->xu_init[Slice(0, xu_0.size())] = xu_0 + error;
}

void Position::prepareRealtime(int new_nTimesteps, int new_horizon) {
	this->horizon = new_horizon;
	Component::prepareRealtime(new_nTimesteps, new_horizon);
}

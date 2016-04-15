/*
 * Component.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: laetus
 */

#include <iostream>
#include <string>
#include <casadi/casadi.hpp>
#include "Component.hpp"

using namespace casadi;

Component::Component() {
	this->iD = Component::counter;
}

Component::Component(int nTimesteps, SX* t) {
	this->iD = Component::counter++;
	this->t = t;
	this->nTimesteps = nTimesteps;
	this->hasOde = true;
	this->hasAlg = false;
	this->isRealtime = false;
	this->measurementErrorStorage
			= Matrix<double>::zeros(1, nTimesteps * 100).data();
	resetErrorList();
}

SX Component::getx() {
	return this->x;
}

SX Component::getu() {
	return this->u;
}

SX* Component::gett() {
	return this->t;
}

//Side conditions and storage

int Component::getSpaceNeeded() {
	return this->space_needed;
}

int Component::getSpace0() {
	return this->space_0;
}

Slice Component::getPlace() {
	if (isControlled()) {
		return this->place_u;
	} else {
		return this->place_x;
	}
}

void Component::setPlace(int beginning) {
	this->place_x = Slice(beginning, beginning + this->space_needed, 2);
	this->place_x0 = Slice(0, beginning + this->space_0, 2);
	this->place_u = Slice(beginning + 1, beginning + this->space_needed, 2);
	this->place_u0 = Slice(beginning + 1, beginning + this->space_0, 2);
}

Slice Component::getPlace0() {
	if (isControlled()) {
		return this->place_u0;
	} else {
		return this->place_x0;
	}
}

Matrix<double> Component::getXU_Min() {
	return this->xu_min;
}

Matrix<double> Component::getXU_Min(int t_act) {
	return this->xu_min;
}

Matrix<double> Component::getXU_Max() {
	return this->xu_max;
}

Matrix<double> Component::getXU_Max(int t_act) {
	return this->xu_max;
}

Matrix<double> Component::getXU_0() {
	return this->xu_0;
}

Matrix<double> Component::getXU_Init() {
	return this->xu_init;
}

Matrix<double> Component::getLam_X() {
	return this->lambda_x;
}

Matrix<double> Component::getVopt() {
	return this->result;
}

Matrix<double> Component::getGlobalOpt() {
	return this->result_glob;
}

void Component::setXU_Min(Matrix<double> xu_min) {
	this->xu_min = xu_min;
}

void Component::setXU_Max(Matrix<double> xu_max) {
	this->xu_max = xu_max;
}

void Component::setXU_0(Matrix<double> xu_0) {
	this->xu_0 = xu_0;
	Matrix<double> error = getError(xu_0.size()) * xu_0[0] * 0.01;
	this->xu_max[Slice(0, xu_0.size())] = xu_0 + error;
	this->xu_min[Slice(0, xu_0.size())] = xu_0 + error;
	this->xu_init[Slice(0, xu_0.size())] = xu_0 + error;
}

void Component::setXU_Init(Matrix<double> xu_initial, int t_act) {
	if (this->xu_init.size() == xu_initial.size()) {
		this->xu_init = xu_initial;
	}
	if (this->xu_init.size() - 1 == xu_initial.size()) {
		//		std::cout << xu_initial.dimString() << std::endl;
		this->xu_init = horzcat(transpose(xu_initial), estimateNewPoint(
				xu_initial, t_act));
	} else {
		std::cout << "XU_INIT does not have the correct dimension" << std::endl;
		this->xu_init = Matrix<double>::ones(1, this->xu_init.size())
				* xu_initial[0];
	}
}

Matrix<double> Component::estimateNewPoint(Matrix<double> xu_initial, int t_act) {
	return xu_initial[xu_initial.size() - 1];
}

void Component::setLam_X(Matrix<double> lam_X) {
	if (this->space_needed == lam_X.size()) {
		this->lambda_x = lam_X;
	}
	if (this->space_needed - 1 == lam_X.size()) {
		this->lambda_x = horzcat(transpose(lam_X), lam_X[lam_X.size() - 1]);
	} else {
		std::cout << "Lam_X does not have the correct dimension" << std::endl;
		throw 42;
	}
}

bool Component::hasALG() {
	return this->hasAlg;
}

SX Component::getALG() {
	return 0;
}

SX Component::getz() {
	return SX();
}

template<class T> T Component::getZ(T V, int k) {
	return T();
}

bool Component::isP() {
	return true;
}

void Component::setVopt(Matrix<double> opt) {
	this->result = opt;
}

void Component::storeAndShift_XU(
		std::map<std::string, Matrix<double> > res_tact, int t_act) {
	Matrix<double> res_xu = (res_tact["x"])[this->getPlace()];
	Matrix<double> res_lx = (res_tact["lam_x"])[this->getPlace()];

	this->storeSolution(res_xu[Slice(0, space_0)]);
	this->setXU_Init(res_xu[Slice(space_0, res_xu.size())], t_act);
	this->setLam_X(res_lx[Slice(space_0, res_lx.size())]);

	// Shift initial condition to the calculated value
	this->setXU_0(res_xu[Slice(space_0, 2 * space_0)]);

}

void Component::storeSolution(Matrix<double> sol_part) {
	this->result = vertcat(result, sol_part);
}

void Component::prepareRealtime(int new_nTimesteps, int new_horizon) {
	this->nTimesteps = new_nTimesteps;
	resetErrorList();
	if (!isRealtime) {
		this->lambda_x_glob = this->lambda_x;
		this->result_glob = this->result;
	}

	this->isRealtime = true;

	//Take the first  nTimesteps values as x0
	this->xu_init = transpose(this->result_glob[Slice(0, this->space_needed)]);
	this->xu_0 = transpose(this->result_glob[Slice(0, this->space_0)]);
	this->result = Matrix<double> ();
	this->lambda_x = Matrix<double> ();

	setUp();
}

void Component::prepareGlobal(int new_nTimesteps) {
	this->nTimesteps = new_nTimesteps;
	resetErrorList();
	this->isRealtime = false;
	this->lambda_x_glob = Matrix<double> ();
	this->result_glob = Matrix<double> ();

	setUp();

	this->result = Matrix<double> ();
	this->lambda_x = Matrix<double> ();
}

bool Component::hasODE() {
	return this->hasOde;
}

std::string Component::getName() {
	return this->name + std::to_string(this->iD);
}

bool Component::isControlled() {
	return this->isCntrld;
}

void Component::setMeasurementError(std::string path) {
	Matrix<double> tmp;
	std::vector<Matrix<double>*> vct(1);
	vct = {&tmp};
	Utils::readFromFile(path, &vct);

	measurementErrorStorage = tmp.data();
	errorList = measurementErrorStorage;
}

void Component::resetErrorList() {
	errorList = measurementErrorStorage;
}

Matrix<double> Component::getError(int size) {
	Matrix<double> ret = Matrix<double>::zeros(1, size);
	for (int i = 0; i < size; i++) {
		ret[i] = errorList[errorList.size() - 1];
		errorList.pop_back();
		if (errorList.empty()) {
			resetErrorList();
		}
	}
	return ret;
}

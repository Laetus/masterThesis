/*
 * HydroModel.cpp
 *
 *  Created on: Jan 12, 2016
 *      Author: laetus
 */

#include "HydroModel.hpp"

using namespace casadi;

HydroModel::HydroModel(int nTimesteps, int horizon, int n_f) :
	HydroModel::Model(nTimesteps, horizon, n_f) {
	this->producedPower = Matrix<double>::zeros(1, max(n_f, nTimesteps));
	this->massWater = -1;
}

void HydroModel::calcProducedPower() {
	for (std::list<Cflow*>::iterator it = controls->begin(); it
			!= controls->end(); it++) {
		Matrix<double> profit_i = (*it)->calculateProducedPower();
		if (isRealtime) {
			producedPowerRT += profit_i;
		} else {
			producedPower += profit_i;
		}
	}
}

void HydroModel::setPowerPrice(Matrix<double> pP) {
	if (pP.size() == this->n_f + this->nTimesteps) {
		this->powerPrice = pP;
	} else {
		std::cout << "Power Price has wrong size!" << std::endl;
		throw 42;
	}
}

void HydroModel::prepareRealtime(int new_nTimesteps, int new_horizon, int n_f) {
	this->n_f = n_f;
	this->nTimesteps = new_nTimesteps;
	this->horizon = new_horizon;
	this->massWater = -1;

	if (!isRealtime) {
		this->producedPower = horzcat(producedPower, producedPower[Slice(0,
				new_nTimesteps)]);
		powerPrice = vertcat(powerPrice, powerPrice[Slice(0, new_nTimesteps)]);
		this->isRealtime = true;
	}

	this->producedPowerRT = Matrix<double>::zeros(1, n_f - 1);
	//Reset
	VINIT = Matrix<double> ();
	VMAX = Matrix<double> ();
	VINIT = Matrix<double> ();
	LAM_X = Matrix<double> ();
	LAM_G = Matrix<double> ();

	V = MX();
	this->integration_times = Matrix<double>();

	for (std::list<Component*>::iterator it = this->begin(); it != this->end(); it++) {
		(*it)->prepareRealtime(new_nTimesteps, new_horizon);
	}

	setPlace();
	this->V = MX::sym("V", this->nVariables);

	std::cout << "Model prepared for Realtime" << std::endl;
	//	}
}

void HydroModel::prepareGlobal(int new_nTimesteps, int new_horizon) {
	this->n_f = 1;
	this->nTimesteps = new_nTimesteps;
	this->horizon = new_horizon;
	this->massWater = -1;
	this->isRealtime = false;

	//Reset
	VINIT = Matrix<double> ();
	VMAX = Matrix<double> ();
	VINIT = Matrix<double> ();
	LAM_X = Matrix<double> ();
	LAM_G = Matrix<double> ();

	V = MX();

	powerPrice = powerPrice[Slice(0, nTimesteps)];
	producedPower = Matrix<double>::zeros(1, nTimesteps);

	for (std::list<Component*>::iterator it = this->begin(); it != this->end(); it++) {
		(*it)->prepareGlobal(new_nTimesteps);
	}

	setPlace();
	this->V = MX::sym("V", this->nVariables);

	std::cout << "Model prepared for Global" << std::endl;
}

void HydroModel::writeModelToConsole(bool withVOPT) {
	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		std::cout << "Component:  " << (*it)->getName() << std::endl;
		if (!(*it)->isControlled()) {
			std::cout << "State" << std::endl;
		} else {
			std::cout << "Control" << std::endl;
		}
		VINIT[(*it)->getPlace()].print(std::cout << "VINIT: ");
		VMIN[(*it)->getPlace()].print(std::cout << "VMIN:  ");
		VMAX[(*it)->getPlace()].print(std::cout << "VMAX:  ");
		if (withVOPT) {
			(*it)->getVopt().print(std::cout << "V_OPT: ");
		}
		std::cout << std::endl;
	}
	if (withVOPT) {
		if (!isRealtime) {
			this->producedPower.print(std::cout << "Produced Power: ");
		} else {
			this->producedPowerRT.print(std::cout << "Produced Power: ");
		}
	}
}

void HydroModel::writeResultToFile(std::ofstream* filestream) {
	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		(*it)->getVopt().print(*filestream << (*it)->getName() << " = ", false);
		*filestream << ";" << std::endl;
	}
	*filestream << "nTimesteps = " << this->nTimesteps << ";" << std::endl;
	*filestream << "horizon = " << this->horizon << ";" << std::endl;
	*filestream << "nv = " << this->nVariables << ";" << std::endl;
	*filestream << "n_f = " << this->n_f << ";" << std::endl;
	this->massWater.print(*filestream << "waterMass = ", false);
	*filestream << ";" << std::endl;
	this->integration_times.print(*filestream << "integrationTimes = ", false);
	*filestream << ";" << std::endl;

	if (!isRealtime) {
		this->powerPrice.print(*filestream << "powerPrice = ", false);
		*filestream << ";" << std::endl;
		this->producedPower.print(*filestream << "prodPower = ", false);
		*filestream << ";" << std::endl;
	} else {
		this->powerPriceRT.print(*filestream << "powerPrice = ", false);
		*filestream << ";" << std::endl;
		this->producedPowerRT.print(*filestream << "prodPower = ", false);
		*filestream << ";" << std::endl;
	}

}

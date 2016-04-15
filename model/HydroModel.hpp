/*
 * HydroModel.hpp
 *
 *  Created on: Nov 19, 2015
 *      Author: laetus
 */

#ifndef HYDROMODEL_HPP_
#define HYDROMODEL_HPP_

#include "../component/Component.hpp"
#include <omp.h>

using namespace std;

class HydroModel : public Model {
public:
	HydroModel(int nTimesteps, int horizon, int n_f);

	virtual Matrix<double> waterMass() = 0;
	virtual void calcProducedPower();
	virtual void setPowerPrice(Matrix<double> pP);

	virtual void writeResultToFile(std::ofstream* filestream);
	void writeModelToConsole(bool withVOPT);

	virtual void prepareRealtime(int new_nTimesteps, int new_horizon, int n_f);
	virtual void prepareGlobal(int new_nTimesteps, int new_horizon);


protected:

	list<Cflow*>* controls;

	Matrix<double> powerPrice;
	Matrix<double> powerPriceRT;
	Matrix<double> producedPower;
	Matrix<double> producedPowerRT;
	Matrix<double> massWater;
};

#endif /* HYDROMODEL_HPP_ */

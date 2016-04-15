/*
 * Cflow.hpp
 *
 *  Created on: Nov 24, 2015
 *      Author: laetus
 */

#ifndef CFLOW_HPP_
#define CFLOW_HPP_

class Cflow: public Flow {

public:

	Cflow(int nTimesteps, casadi::SX* t, double min, double max, double efficiency, double diff);
	Cflow(int nTimesteps, casadi::SX* t, double min, double max, double x0, double efficiency, double diff);


	virtual bool isControlled();
	virtual std::string getName();

	double getHeightDifference();
	double getPowerCoefficient();

	casadi::MX calculatePower(casadi::MX V);
	casadi::Matrix<double> calculateProducedPower();

	virtual void setUp();

protected:

	double powerCoefficient;
	double heightDifference; // positive if A is higher as B

};

#endif /* CFLOW_HPP_ */

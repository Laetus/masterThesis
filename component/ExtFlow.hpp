/*
 * ExtFlow.hpp
 *
 *  Created on: Nov 12, 2015
 *      Author: laetus
 */

#include "Flow.hpp"
#include <iostream>
#include <fstream>

#ifndef EXTFLOW_HPP_
#define EXTFLOW_HPP_

class ExtFlow : public Flow {

public :

	ExtFlow(int nTimesteps, SX* t, bool flowsIntoSystem);
	ExtFlow(int nTimesteps, SX* t, bool flowsIntoSystem, double x_max, double x_min, double x0 );
	ExtFlow(int nTimestpes, SX* t, bool flowsIntoSystem, std::string name);

	virtual void setUp();

	virtual std::string getName();
	void setComponent(Component* component);

	bool flowsInto();

	virtual Matrix<double> getXU_Min(int t_act);
	virtual Matrix<double> getXU_Max(int t_act);

	virtual void readFromFile(std::string name);
	virtual Matrix<double> estimateNewPoint(Matrix<double> xu_initial, int t_act);

private:
	bool flowsIntoSystem;

	Matrix<double> getXU_MinMax(int t_act);

	Matrix<double> givenValues;
	Matrix<double> givenValuesRT;
};

#endif /* EXTFLOW_HPP_ */

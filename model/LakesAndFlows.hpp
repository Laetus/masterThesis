/*
 * LakesAndFlows.hpp
 *
 *  Created on: Nov 24, 2015
 *      Author: laetus
 */

#ifndef LAKESANDFLOWS_HPP_
#define LAKESANDFLOWS_HPP_

#include "HydroModel.hpp"

class LakesAndFlows : public HydroModel {

public:

	LakesAndFlows(int nTimesteps, int horizon,int t_f);

	virtual Matrix<double> waterMass();
	virtual casadi::MX getf(int t_act);

private:

	Lake* lake0;
	Lake* lake1;
	Lake* lake2;
	Lake* lake3;

	Cflow* turb12;
	Cflow* turb23;
	Cflow* pump10;
	Cflow* turb02;

	ExtFlow* inflow0;
	ExtFlow* inflow1;
	ExtFlow* outflow3;

};

#endif /* LAKESANDFLOWS_HPP_ */

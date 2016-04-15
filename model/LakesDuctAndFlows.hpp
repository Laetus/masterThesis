/*
 * LakesDuctAndFlows.hpp
 *
 *  Created on: Dec 7, 2015
 *      Author: laetus
 */

#ifndef LAKESDUCTANDFLOWS_HPP_
#define LAKESDUCTANDFLOWS_HPP_

#include "HydroModel.hpp"

class LakesDuctAndFlows : public HydroModel {

public:

	LakesDuctAndFlows(int nTimesteps, int horizon,int t_f);

	virtual Matrix<double> waterMass();
	virtual casadi::MX getf(int t_act);

private:

	Lake* lake0;
	Lake* lake3b;
	Lake* lake1;
	Lake* lake2;
	Lake* lake3;

	LakeDuct* duct3;

	Cflow* turb12;
	Cflow* turb23;
	Cflow* pump10;
	Cflow* turb02;

	ExtFlow* inflow0;
	ExtFlow* inflow1;
	ExtFlow* outflow3;
};

#endif /* LAKESDUCTANDFLOWS_HPP_ */

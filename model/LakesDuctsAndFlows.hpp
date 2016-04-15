/*
 * LakesDuctsAndFlows.hpp
 *
 *  Created on: Dec 7, 2015
 *      Author: laetus
 */

#ifndef LAKESDUCTSANDFLOWS_HPP_
#define LAKESDUCTSANDFLOWS_HPP_

#include "HydroModel.hpp"

class LakesDuctsAndFlows : public HydroModel {

public:

	LakesDuctsAndFlows(int nTimesteps, int horizon,int t_f);

	virtual Matrix<double> waterMass();
	virtual casadi::MX getf(int t_act);

private:

	Lake* lake0;
	Lake* lake3b;
	Lake* lake1;
	Lake* lake2;
	Lake* lake3;
	Lake* lake0b;

	LakeDuct* duct3;
	LakeDuct* duct0;

	Cflow* turb12;
	Cflow* turb23;
	Cflow* pump10;
	Cflow* turb02;

	ExtFlow* inflow0;
	ExtFlow* inflow1;
	ExtFlow* outflow3;
};

#endif /* LAKESDUCTSANDFLOWS_HPP_ */

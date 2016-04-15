/*
 * SimpleLake.hpp
 *
 *  Created on: Nov 24, 2015
 *      Author: laetus
 */

#ifndef SIMPLELAKE_HPP_
#define SIMPLELAKE_HPP_

#include "HydroModel.hpp"

class SimpleLake : public HydroModel {

public:

	SimpleLake(int nTimesteps, int horizon,int t_f);

	virtual Matrix<double> waterMass();
	virtual casadi::MX getf(int t_act);

private:

	Lake* lake;
	double opt_height;

	ExtFlow* inflow;
	Cflow* outflow;

};

#endif /* SIMPLELAKE_HPP_ */

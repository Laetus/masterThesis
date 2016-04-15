/*
 * RealtimeIpopt.hpp
 *
 *  Created on: Nov 29, 2015
 *      Author: laetus
 */

#ifndef REALTIMESOLVER_HPP_
#define REALTIMESOLVER_HPP_

//#include "Solver.cpp"

class RealtimeIpopt : public Solver {

public:

	RealtimeIpopt(Model* model);

	virtual void solve();

protected:

	std::map<std::string, Matrix<double> > solveIpopt();
	std::vector< Matrix<double> > assumedData;

	bool firstIteration;
	std::map<std::string, Matrix<double> > last_result;

};

#endif /* REALTIMESOLVER_HPP_ */

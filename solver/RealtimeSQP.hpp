/*
 * RealtimeHQP.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#ifndef REALTIMESQP_HPP_
#define REALTIMESQP_HPP_

class RealtimeSQP : public Solver {
public :

	RealtimeSQP(Model* model);
	virtual void solve();

protected :
	void solveSubproblem();
	MXFunction nlp_sub;
	std::map<string, Matrix<double> > result;

	bool firstIteration;
	std::vector<Matrix<double> > assumedData;
};


#endif /* REALTIMESQP_HPP_ */

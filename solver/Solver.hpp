/*
 * Solver.hpp
 *
 *  Created on: Nov 29, 2015
 *      Author: laetus
 */

#ifndef SOLVER_HPP_
#define SOLVER_HPP_

using namespace casadi;

class Solver {

public:

	Solver(Model* model);

	virtual void writeResultToFile(std::string path);
	virtual void solve() = 0;

	virtual void setName(std::string name);


protected:

	void storeStatsIpopt(NlpSolver* solver);
	void evaluateOriginalF(int t_act, Matrix<double> x_act);
	void printTimingData(int t_act);

	Model* model;
//	Matrix<double> x0;
	MXFunction nlp;

	Matrix<double> origFevals;

	Matrix<double> v_opt;
	MX G;
	Matrix<double> G_bound;

	Dict opts;

	std::list<Dict>* timing_stats;
	std::list<std::map<std::string, GenericType> >* ipOpt_stats;
	std::string name;

	std::ofstream myfile;

	Matrix<double> timing;
	std::chrono::time_point<std::chrono::system_clock> startIt, endIt,
	startSolver, endSolver, startMS, endMS;
	std::chrono::duration<double> sec_p_It, sec_p_Solve, sec_p_MS;

};

#endif /* SOLVER_HPP_ */

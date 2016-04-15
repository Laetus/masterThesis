/*
 * RealtimeIpopt.cpp
 *
 *  Created on: Nov 29, 2015
 *      Author: laetus
 */

#include "RealtimeIpopt.hpp"

RealtimeIpopt::RealtimeIpopt(Model* model) :
	RealtimeIpopt::Solver(model) {
	this->assumedData = std::vector<Matrix<double> >(this->model->getN_F() + 1);
	this->nlp = MXFunction("nlp", nlpIn("x", model->getV()), nlpOut("f",
			model->getf(0), "g", G));
	this->firstIteration = true;
	this->name = "RT_IpOpt";
}

void RealtimeIpopt::solve() {
	for (int t_act = 0; t_act < model->getN_F(); t_act++) {
		std::cout << "Time: " + to_string(t_act) << std::endl;
		startIt = std::chrono::system_clock::now();
		//Solve the problem at t_act

		this->nlp = MXFunction("nlp", nlpIn("x", model->getV()), nlpOut("f",
				model->getf(t_act), "g", G));

		startSolver = std::chrono::system_clock::now();
		std::map<string, Matrix<double> > result_tact = solveIpopt();
		endSolver = std::chrono::system_clock::now();
		// Store data
		assumedData[t_act] = result_tact["x"];

		evaluateOriginalF(t_act, assumedData[t_act]);

		//Setup new iteration
		model->storeAndShiftValues(result_tact, t_act);
		last_result = result_tact;
		if (firstIteration) {
			firstIteration = false;
			opts["warm_start_init_point"] = "yes";
		}
		endIt = std::chrono::system_clock::now();
		printTimingData(t_act);
	}
}

std::map<std::string, Matrix<double> > RealtimeIpopt::solveIpopt() {

	std::map<std::string, Matrix<double> > arg = make_map("lbx",
			model->getVMIN(), "ubx", model->getVMAX(), "lbg", this->G_bound,
			"ubg", this->G_bound, "x0", model->getVINIT());

	if (!firstIteration) {
		arg.insert(std::pair<std::string, Matrix<double> >("lam_g0",
				model->getLAM_G()));
		arg.insert(std::pair<std::string, Matrix<double> >("lam_x0",
				model->getLAM_X()));
	}
	NlpSolver nlpSolver = NlpSolver("solver", "ipopt", nlp, opts);
	nlpSolver.setOption("warn_initial_bounds", true);
	nlpSolver.setOption("eval_errors_fatal", true);
	std::map<string, Matrix<double> > result = nlpSolver(arg);
	storeStatsIpopt(&nlpSolver);
	return result;
}


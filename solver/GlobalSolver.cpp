/*
 * GlobalSolver.cpp
 *
 *  Created on: Nov 29, 2015
 *      Author: laetus
 */

#include "GlobalSolver.hpp"

GlobalSolver::GlobalSolver(Model* model) :
	GlobalSolver::Solver(model) {
	this->nlp = MXFunction("nlp", nlpIn("x", model->getV()), nlpOut("f",
			model->getf(0), "g", G));
	this->name = "GlobalSolver";
	opts["constr_viol_tol"] = 0.0001;
	opts["max_iter"] = 500;
}

void GlobalSolver::solve() {
	startIt = std::chrono::system_clock::now();
	NlpSolver nlpSolver = NlpSolver("solver", "ipopt", nlp, opts);
	std::map<std::string, Matrix<double> > arg = make_map("lbx",
			model->getVMIN(), "ubx", model->getVMAX(), "lbg",
			Matrix<double>::zeros((int) G.size()), "ubg",
			Matrix<double>::zeros((int) G.size()), "x0", model->getVINIT());
	startSolver = std::chrono::system_clock::now();
	std::map<string, Matrix<double>> result = nlpSolver(arg);
	endSolver = std::chrono::system_clock::now();
	v_opt = result["x"];
	storeStatsIpopt(&nlpSolver);
	for (std::list<Component*>::iterator it = this->model->begin(); it
			!= this->model->end(); it++) {
		(*it)->setVopt(v_opt[(*it)->getPlace()]);
	}

	endIt = std::chrono::system_clock::now();
	printTimingData(0);
}

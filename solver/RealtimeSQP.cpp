/*
 * RealtimeSQP.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include "RealtimeSQP.hpp"

RealtimeSQP::RealtimeSQP(Model* model) : RealtimeSQP::Solver(model) {
	this->nlp = MXFunction("nlp", nlpIn("x", model->getV()), nlpOut("f", model->getf(0), "g", G));

	this->firstIteration = true;
	this->name = "SQP";

}

void RealtimeSQP::solve(){

	//Initialisation
	int t_act = 0;
	std::cout << "Time: " << t_act << std::endl;

	//Solve the original problem
	Dict opts;
	opts["max_iter"] = 500;
	std::map<std::string, Matrix<double> > arg = make_map("lbx",
			model->getVMIN(), "ubx", model->getVMAX(), "lbg", this->G_bound,
			"ubg", this->G_bound, "x0", model->getVINIT());
	NlpSolver nlpSolver = NlpSolver("solver", "ipopt", nlp, opts);
	std::map<string, Matrix<double>> result_tact = nlpSolver(arg);

	//Store data
	assumedData[t_act] = result_tact["x"];

	//Setup new iteration
	model->storeAndShiftValues(result_tact, t_act);
	storeStatsIpopt(&nlpSolver);

	//Define values

	//Iteration
	for(t_act = 1; t_act <= model->getN_F(); t_act++){
		std::cout <<"Time: " << t_act << std::endl;

		MX f_sub;
		MX G_sub;

		this->nlp_sub = MXFunction("nlp_sub", nlpIn("x", model->getV()), nlpOut("f", f_sub, "g", G_sub));

		solveSubproblem();
	}

}


void RealtimeSQP::solveSubproblem(){


}

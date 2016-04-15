/*
 * Solver.cpp
 *
 *  Created on: Nov 29, 2015
 *      Author: laetus
 */

#include "Solver.hpp"

Solver::Solver(Model* model) {
	this->model = model;
	startMS = std::chrono::system_clock::now();
	this->G = this->model->doMultipleShooting(model->getV());
	endMS = std::chrono::system_clock::now();
	this->G_bound = Matrix<double>::zeros((int) G.size());
	this->model->calculateV();
	this->timing_stats = new list<Dict> ();
	this->ipOpt_stats = new list<std::map<std::string, GenericType> > ();
	this->timing = Matrix<double>::zeros(3, model->getN_F());
	this->origFevals = 42 * Matrix<double>::ones(1, this->model->getN_F());
	opts["max_iter"] = 200;
	//	opts["derivative_test"] = "second-order";
	//	opts["derivative_test_print_all"] = "yes";
	opts["print_timing_statistics"] = "no";
	opts["print_level"] = 5;
	opts["gather_stats"] = true;
	opts["constr_viol_tol"] = 0.00001;
	//	opts["output_file"] = "secondStep.txt";
	//	opts["file_print_level"] = 5;
	//	std::vector<std::string> bla;
	//	bla= {"eval_f", "eval_g","eval_grad_f", "eval_h" ,"eval_jac_g", "inputs", "outputs"};


}

void Solver::storeStatsIpopt(NlpSolver* nlpSolver) {
	Dict dict = nlpSolver->getStats();
	try {
		std::map<std::string, GenericType> ipOptStats = dict["iterations"];
		dict.erase("iterations");
		this->ipOpt_stats->push_back(ipOptStats);
	} catch (const exception e) {
		std::cout << "iterations not found" << std::endl;
	}
	this->timing_stats->push_back(dict);
}

void Solver::writeResultToFile(std::string path) {
	std::string filename = this->name + "_" + model->getName();
	std::ofstream filestream;
	filestream.open(path + "/result_" + filename + ".m");

	model->writeResultToFile(&filestream);

	Dict dict1 = this->timing_stats->front();
	int maxIter = 0;
	for (Dict::iterator it = dict1.begin(); it != dict1.end(); it++) {

		filestream << this->name + "_" + it->first << " = [";

		std::list<Dict>::iterator lit = this->timing_stats->begin();
		while (lit != this->timing_stats->end()) {

			filestream << lit->at(it->first);
			if (it->first == "iter_count" && ((double) lit->at(it->first)
					> maxIter)) {
				maxIter = lit->at(it->first);
			}
			if (++lit != this->timing_stats->end()) {
				filestream << ", ";
			}

		}
		filestream << "];" << std::endl;
	}

	maxIter++;

	std::map<std::string, GenericType> map1 = this->ipOpt_stats->front();
	for (map<string, GenericType>::iterator it = map1.begin(); it != map1.end(); it++) {

		filestream << this->name + "_" + it->first << " = [";

		std::list<map<string, GenericType> >::iterator lit =
				this->ipOpt_stats->begin();

		while (lit != this->ipOpt_stats->end()) {
			GenericType tmp = lit->at(it->first);
			int size = 0;
			if (tmp.getType() == 8) {
				std::vector<double, std::allocator<double> > tmp2 =
						tmp.toDoubleVector();
				size = tmp2.size();
			}
			if (tmp.getType() == 5) {
				std::vector<int, std::allocator<int> > tmp3 = tmp.toIntVector();
				size = tmp3.size();
			}

			filestream << tmp;

			if (maxIter - size > 0) {
				filestream << ", ";
				Matrix<double> filler = -42*
						Matrix<double>::ones(1, maxIter - size);
				filler.print(filestream, false);
			}

			if (++lit != this->ipOpt_stats->end()) {
				filestream << "; ";
			}

		}
		filestream << "];" << std::endl;
	}

	Matrix<double> sec_pIT = timing[Slice(0, timing.size(), 3)];
	Matrix<double> sec_pSv = timing[Slice(1, timing.size(), 3)];
	Matrix<double> sec_pMS = timing[Slice(2, timing.size(), 3)];
	sec_pIT.print(filestream << this->name + "_timing_sec_p_Solve = ", false);
	filestream << ";" << std::endl;
	sec_pSv.print(filestream << this->name + "_timing_sec_p_It = ", false);
	filestream << ";" << std::endl;
	sec_pMS.print(filestream << this->name + "_timing_sec_p_MS = ", false);
	filestream << ";" << std::endl;
	this->origFevals.print(filestream << this->name + "_orig_f_evals = ", false);
	filestream << ";" << std::endl;

	filestream.close();
}

void Solver::setName(std::string name) {
	this->name = name;
}

void Solver::printTimingData(int t_act) {
	sec_p_It = endIt - startIt;
	sec_p_Solve = endSolver - startSolver;
	sec_p_MS = endMS - startMS;
	std::cout << "elapsed iteration time: " << sec_p_It.count() << std::endl;
	std::cout << "elapsed time for MS:    " << sec_p_MS.count() << std::endl;
	std::cout << "elapsed time to solve:  " << sec_p_Solve.count() << std::endl;
	startIt = std::chrono::system_clock::now();
	startSolver = startIt;
	timing[3 * t_act] = sec_p_It.count();
	timing[3 * t_act + 1] = sec_p_Solve.count();
	timing[3 * t_act + 2] = sec_p_MS.count();
}

void Solver::evaluateOriginalF(int t_act, Matrix<double> x_act){
	std::map<std::string, Matrix<double> > result = this->nlp(make_map("x",x_act));
	Matrix<double> result_f = result["f"];
	this->origFevals[t_act] = result_f.getValue(0);
}

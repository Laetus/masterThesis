/*
 * RealtimeAPCSCP.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: laetus
 */

#include "RealtimeAPCSCP.hpp"

RealtimeAPCSCP::RealtimeAPCSCP(Model* model) :
	RealtimeAPCSCP::Solver(model) {
	this->nlp = MXFunction("nlp", nlpIn("x", model->getV()), nlpOut("f",
			model->getf(0), "g", G));
	this->dG_f = this->nlp.jacobian("x", "g");
	this->dG_fast = this->nlp.derReverse(1);
	this->choiceH = 0;
	this->exactA = true;
	this->n_updateA = 1;
	this->n_updateH = 1;
	this->assumedData = std::vector<Matrix<double> >(this->model->getN_F() + 1);
	this->firstIteration = true;
	this->name = "APCSCP";
	this->hess_gi = std::vector<Function>(G.size());
	for (int i = 0; i < G.size(); i++) {
		hess_gi[i] = MXFunction("constraint_i", nlpIn("x", model->getV()),
				nlpOut("g", (MX) G[i])).hessian("x", "g");
	}
	hess_fi = nlp.hessian("x", "f");
}

void RealtimeAPCSCP::solve() {
	//Initialisation (calculate the exact solution for inital point)
	int t_act = 0;
	std::cout << "Time: " + to_string(t_act) << std::endl;
	startIt = std::chrono::system_clock::now();
	startSolver = std::chrono::system_clock::now();

	//Solve the first problem exactly
	std::map<std::string, Matrix<double> > arg = make_map("lbx",
			model->getVMIN(), "ubx", model->getVMAX(), "lbg", this->G_bound,
			"ubg", this->G_bound, "x0", model->getVINIT());
	NlpSolver nlpSolver = NlpSolver("solver", "ipopt", nlp, opts);
	nlpSolver.setOption("warn_initial_bounds", true);
	nlpSolver.setOption("eval_errors_fatal", true);
	std::map<string, Matrix<double>> result_tact = nlpSolver(arg);
	endSolver = std::chrono::system_clock::now();
	// Store data
	assumedData[t_act] = result_tact["x"];

	//Setup new iteration
	model->storeAndShiftValues(result_tact, t_act);
	storeStatsIpopt(&nlpSolver);

	//Define values
	this->x_act = result_tact["x"];
	this->y_act = result_tact["lam_g"];

	evaluateOriginalF(t_act,x_act);
	updateA_act(t_act);
	updateH_act(t_act);
	updateM_act();
	m_act = mul(transpose(Dg_act - A_act), y_act);
	firstIteration = false;
	opts["warm_start_init_point"] = "yes";
	endIt = std::chrono::system_clock::now();
	printTimingData(t_act);

	//Iteration
	for (t_act = 1; t_act < model->getN_F(); t_act++) {
		std::cout << "Time: " + to_string(t_act) << std::endl;
		startIt = std::chrono::system_clock::now();

		startMS = std::chrono::system_clock::now();
		G_sub = mul(A_act, model->getV() - x_act) + model->doMultipleShooting(x_act);
		endMS = std::chrono::system_clock::now();
		f_sub = model->getf(t_act) + mul(transpose(m_act), model->getV()
				- x_act) + 0.5 * quad_form(model->getV() - x_act, H_act);

		this->nlp_sub = MXFunction("nlp", nlpIn("x", model->getV()), nlpOut(
				"f", f_sub, "g", G_sub));
		this->nlp = MXFunction("nlp", nlpIn("x", model->getV()), nlpOut("f",
				model->getf(t_act), "g", G));

		// Step2 solve convex subproblem
		startSolver = std::chrono::system_clock::now();
		result_tact = solveConvexSubproblem();
		endSolver = std::chrono::system_clock::now();

		// Step3 update matrices and retrieve new measurement (step 1)
		x_act = result_tact["x"];
		y_act = result_tact["lam_g"];
		model->storeAndShiftValues(result_tact, t_act);

		// update
		this->x_act = result_tact["x"];
		this->y_act = result_tact["lam_g"];

		evaluateOriginalF(t_act, x_act);
		updateA_act(t_act);
		updateH_act(t_act);
		updateM_act();

		endIt = std::chrono::system_clock::now();
		printTimingData(t_act);
	}
}

std::map<string, Matrix<double> > RealtimeAPCSCP::solveConvexSubproblem() {

	std::map<std::string, Matrix<double> > arg = make_map("lbx",
			model->getVMIN(), "ubx", model->getVMAX(), "lbg", this->G_bound,
			"ubg", this->G_bound, "x0", model->getVINIT());

	if (!firstIteration) {
		arg.insert(std::pair<std::string, Matrix<double> >("lam_g0",
				model->getLAM_G()));
		arg.insert(std::pair<std::string, Matrix<double> >("lam_x0",
				model->getLAM_X()));
	}
	NlpSolver nlpSolver = NlpSolver("solver", "ipopt", nlp_sub, opts);
	nlpSolver.setOption("warn_initial_bounds", true);
	nlpSolver.setOption("eval_errors_fatal", true);
	std::map<string, Matrix<double> > result = nlpSolver(arg);
	storeStatsIpopt(&nlpSolver);
	return result;
}

void RealtimeAPCSCP::updateH_act(int t_act) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> duration;
	start = std::chrono::system_clock::now();
	if (t_act % n_updateH == 0) {
		switch (choiceH) {
		case 0:
			if (t_act == 0) {
				H_act = Matrix<double>::zeros(x_act.size(), x_act.size());
			}
			break;
		case 1:
			H_act = calcCheapLagrangian();
			break;
		case 2:
			H_act = calcApproxLagrangian();
			break;
		case 3:
			H_act = calcExactLagrangian();
			break;
		default:
			if (t_act == 0) {
				H_act = Matrix<double>::zeros(x_act.size(), x_act.size());
			}
		}
	}
	end = std::chrono::system_clock::now();
	duration = end - start;
	std::cout << "Duration Update H: " << duration.count() << std::endl;
}

void RealtimeAPCSCP::updateA_act(int t_act) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> duration;

	start = std::chrono::system_clock::now();
	if (exactA) {
		updateDg_act();
		A_act = Dg_act;
	} else {
		if (t_act % n_updateA == 0) {
			updateDg_act();
			A_act = Dg_act;
		}
	}
	end = std::chrono::system_clock::now();
	duration = end - start;
	std::cout << "Duration Update A:  " << duration.count() << std::endl;
}

void RealtimeAPCSCP::updateDg_act() {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> duration;
	start = std::chrono::system_clock::now();
	std::map<std::string, Matrix<double> > result = dG_f(make_map("x", x_act));
	this->Dg_act = result["jac"];
	end = std::chrono::system_clock::now();
	duration = end - start;
	std::cout << "Duration Update dG: " << duration.count() << std::endl;
}

void RealtimeAPCSCP::updateM_act() {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> duration;
	start = std::chrono::system_clock::now();

	std::map<std::string, Matrix<double> > arg = make_map("der_x", x_act,
			"adj0_g", y_act, "der_g", y_act);
	std::map<std::string, Matrix<double> > result = dG_fast(arg);

	//	Matrix<double> m_act_fast = result["adj0_x"];
	//	updateDg_act();
	//	Matrix<double> m_act_slow = mul(transpose(Dg_act), y_act);
	//	Matrix<double> diff = m_act_slow - m_act_fast;
	//	m_act_fast.print(std::cout << "Fast = ", false);
	//	std::cout << ";" << std::endl;
	//	m_act_slow.print(std::cout << "Slow =  ", false);
	//	std::cout << ";" << std::endl;
	//	diff.print(std::cout << "Difference = ", false);
	//	std::cout << ";" << std::endl;
	//	diff = norm_2(diff);
	//	diff.print(std::cout << "Difference Norm =  ", false);
	//	std::cout << ";" << std::endl;
	//	m_act = mul(transpose(Dg_act), y_act);
	//	m_act = m_act_fast;

	m_act = result["adj0_x"] - mul(transpose(A_act), y_act);
	end = std::chrono::system_clock::now();
	duration = end - start;
	std::cout << "Duration Update M:  " << duration.count() << std::endl;
}

void RealtimeAPCSCP::setExactA(bool isExact) {
	this->exactA = isExact;
}

void RealtimeAPCSCP::setChoiceH(int option) {
	this->choiceH = option;
}

void RealtimeAPCSCP::setNupdateA(int n_updateA) {
	this->n_updateA = n_updateA;
	this->setExactA(false);
}

void RealtimeAPCSCP::setNupdateH(int n_updateH) {
	this->n_updateH = n_updateH;
}

Matrix<double> RealtimeAPCSCP::calcExactLagrangian() {
	//	std::chrono::time_point<std::chrono::system_clock> start, end, startf, endf;
	//	std::chrono::duration<double> duration;
	std::map<std::string, Matrix<double> > eval;
	std::vector<std::map<std::string, Matrix<double> > > evec(G.size());
	std::map<std::string, Matrix<double>> map = make_map("x", x_act);
	Matrix<double> result;

	//	start = std::chrono::system_clock::now();
	eval = hess_fi(map);
	result = eval["jac"];

	//	startf = std::chrono::system_clock::now();
	for (int i = 0; i < G.size(); i++) {
		eval = this->hess_gi[i](map);
		result += y_act[i] * (eval)["jac"];
	}
	//	endf = std::chrono::system_clock::now();
	//	duration = endf - startf;
	//	std::cout << "Duration of for Loop:                " << duration.count()
	//			<< std::endl;
	return result;
}

Matrix<double> RealtimeAPCSCP::calcApproxLagrangian() {
	std::map<std::string, Matrix<double> > eval;
	std::map<std::string, Matrix<double> > map = make_map("x", x_act);
	Matrix<double> result = Matrix<double>::zeros(x_act.size(), x_act.size());
	for (int i = 0; i < G.size(); i++) {
		eval = this->hess_gi[i](map);
		result += y_act[i] * eval["jac"];
	}
	return result;
}

Matrix<double> RealtimeAPCSCP::calcCheapLagrangian() {
	std::map<std::string, Matrix<double> > eval;
	std::map<std::string, Matrix<double> > map = make_map("x", x_act);
	Matrix<double> result = Matrix<double>::zeros(x_act.size(), x_act.size());
	for (int i = 0; i < G.size(); i++) {
		if (y_act.getValue(i) > 1e-3) {
			eval = this->hess_gi[i](map);
			result += y_act[i] * eval["jac"];
		}
	}
	return result;
}

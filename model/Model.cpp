/*
 * Model.cpp
 *
 *  Created on: Nov 19, 2015
 *      Author: laetus
 */

#include "Model.hpp"

Model::Model(int nTimesteps, int horizon, int t_f) {
	this->nTimesteps = nTimesteps;
	this->horizon = horizon;
	this->n_f = t_f;
	this->isRealtime = false;
}

list<Component*>* Model::getComponents() {
	return this->components;
}

list<Component*>::iterator Model::begin() {
	return this->components->begin();
}

list<Component*>::iterator Model::end() {
	return this->components->end();
}

MX Model::getV() {
	return this->V;
}

Matrix<double> Model::getVINIT() {
	return this->VINIT;
}

Matrix<double> Model::getVMAX() {
	return this->VMAX;
}

Matrix<double> Model::getVMIN() {
	return this->VMIN;
}

Matrix<double> Model::getLAM_X() {
	return this->LAM_X;
}

Matrix<double> Model::getLAM_G() {
	return this->LAM_G;
}

bool Model::getIsRealtime() {
	return this->isRealtime;
}

int Model::getNTimesteps() {
	return nTimesteps;
}

SXFunction Model::buildDAE() {
	SX x, u, z, ode, alg;
	nDAE = 0;
	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		//		std::cout << (*it)->getName() << std::endl;
		if ((*it)->isControlled()) {
			u = vertcat(u, (*it)->getu());
			//			u.print(std::cout << "U: ");
		} else {

			if ((*it)->hasODE()) {
				x = vertcat(x, (*it)->getx());
				ode = vertcat(ode, (*it)->getODE());
				nDAE += 1;

				if ((*it)->hasALG()) {
					z = vertcat(z, (*it)->getz());
					alg = vertcat(alg, (*it)->getALG());
					nDAE += 1;
				}

			} else {
				if ((*it)->hasALG()) {
					z = vertcat(z, (*it)->getz());
					alg = vertcat(alg, (*it)->getALG());
					nDAE += 1;

				} else {
					if ((*it)->isP()) {
						u = vertcat(u, (*it)->getx());
					}
				}
			}
		}
	}

	//	x.print(std::cout << "X:   ");
	//	z.print(std::cout << "Z:   ");
	//	u.print(std::cout << "U:   ");
	//	ode.print(std::cout << "ODE: ");
	//	alg.print(std::cout << "ALG: ");

	this->dae = SXFunction("dae", daeIn("x", x, "p", u, "z", z, "t", t),
			daeOut("ode", ode, "alg", alg));
	//	std::cout << "DAE created" << std::endl;

	return this->dae;
}

MX Model::doMultipleShooting(MX whichVar) {
	//	std::chrono::time_point<std::chrono::system_clock> startP, endP, start, end;
	//	std::chrono::duration<double> all, setup;
	//
	//	start = std::chrono::system_clock::now();

	MX result = MX::zeros(nTimesteps * nDAE, 1);
	std::vector<MX> g;
	vector<map<std::string, MX> > int_out(omp_get_max_threads());
	omp_lock_t* lock = new omp_lock_t;
	omp_init_lock(lock);
	omp_lock_t* lock2 = new omp_lock_t;
	omp_init_lock(lock2);

	vector<std::chrono::time_point<std::chrono::system_clock> > startV(
			omp_get_max_threads());
	vector<std::chrono::time_point<std::chrono::system_clock> > endV(
			omp_get_max_threads());
	vector<std::chrono::duration<double> > lock_1(omp_get_max_threads());
	vector<std::chrono::duration<double> > lock_2(omp_get_max_threads());
	vector<std::chrono::duration<double> > integration(omp_get_max_threads());
	vector<std::chrono::duration<double> > setup2(omp_get_max_threads());

	vector<MX> Xk(omp_get_max_threads());
	vector<MX> Xk_n(omp_get_max_threads());
	vector<MX> Xk_e(omp_get_max_threads());
	vector<MX> Uk(omp_get_max_threads());
	vector<MX> Zk(omp_get_max_threads());
	vector<MX> Zk_n(omp_get_max_threads());
	vector<MX> Zk_e(omp_get_max_threads());

	//	endP = std::chrono::system_clock::now();
	//	setup = endP - start;

#pragma omp parallel for
	for (int k = 0; k < nTimesteps; k++) {

		startV[omp_get_thread_num()] = std::chrono::system_clock::now();

		Xk[omp_get_thread_num()] = MX();
		Xk_n[omp_get_thread_num()] = MX();
		Xk_e[omp_get_thread_num()] = MX();
		Uk[omp_get_thread_num()] = MX();
		Zk[omp_get_thread_num()] = MX();
		Zk_n[omp_get_thread_num()] = MX();
		Zk_e[omp_get_thread_num()] = MX();

		endV[omp_get_thread_num()] = std::chrono::system_clock::now();
		setup2[omp_get_thread_num()] += endV[omp_get_thread_num()]
				- startV[omp_get_thread_num()];
		startV[omp_get_thread_num()] = std::chrono::system_clock::now();
		omp_set_lock(lock);

		for (std::list<Component*>::iterator it = this->components->begin(); it
				!= this->components->end(); it++) {
			//				std::cout << (*it)->getName() << std::endl;
			if ((*it)->isControlled()) {
				MX tmp = whichVar[(*it)->getPlace()];
				Uk[omp_get_thread_num()] = vertcat(Uk[omp_get_thread_num()],
						tmp[k]);
			} else {

				if ((*it)->hasODE()) {
					MX tmp = whichVar[(*it)->getPlace()];
					Xk[omp_get_thread_num()] = vertcat(
							Xk[omp_get_thread_num()], tmp[k]);
					Xk_n[omp_get_thread_num()] = vertcat(
							Xk_n[omp_get_thread_num()], tmp[k + 1]);

					if ((*it)->hasALG()) {

						MX tmp = whichVar[(*it)->getPlace()];
						Zk[omp_get_thread_num()] = vertcat(
								Zk[omp_get_thread_num()], tmp[k]);
						Zk_n[omp_get_thread_num()] = vertcat(
								Zk_n[omp_get_thread_num()], tmp[k + 1]);

					}

				} else {
					if ((*it)->hasALG()) {

						MX tmp = whichVar[(*it)->getPlace()];
						Zk[omp_get_thread_num()] = vertcat(
								Zk[omp_get_thread_num()], tmp[k]);
						Zk_n[omp_get_thread_num()] = vertcat(
								Zk_n[omp_get_thread_num()], tmp[k + 1]);

					} else {
						if ((*it)->isP()) {
							MX tmp = whichVar[(*it)->getPlace()];
							Uk[omp_get_thread_num()] = vertcat(
									Uk[omp_get_thread_num()], tmp[k]);
						}
					}
				}
			}
		}

		omp_unset_lock(lock);

		endV[omp_get_thread_num()] = std::chrono::system_clock::now();
		lock_1[omp_get_thread_num()] += endV[omp_get_thread_num()]
				- startV[omp_get_thread_num()];
		startV[omp_get_thread_num()] = std::chrono::system_clock::now();

		//		std::cout << "Integrate " + to_string(k) + " :  " << omp_get_thread_num() << std::endl;
		int_out[omp_get_thread_num()]
				= integrators[omp_get_thread_num()](
						make_map("x0", Xk[omp_get_thread_num()], "z0",
								Zk[omp_get_thread_num()], "p",
								Uk[omp_get_thread_num()]));

		Xk_e[omp_get_thread_num()] = int_out[omp_get_thread_num()]["xf"];
		Zk_e[omp_get_thread_num()] = int_out[omp_get_thread_num()]["zf"];

		endV[omp_get_thread_num()] = std::chrono::system_clock::now();
		integration[omp_get_thread_num()] += endV[omp_get_thread_num()]
				- startV[omp_get_thread_num()];
		startV[omp_get_thread_num()] = std::chrono::system_clock::now();

		omp_set_lock(lock2);
		g.push_back(Xk_n[omp_get_thread_num()] - Xk_e[omp_get_thread_num()]);
		//		std::cout << "k: " + to_string(k) << std::endl;

		result[Slice(k * nDAE, (k + 1) * nDAE)] = vertcat(
				Xk_n[omp_get_thread_num()] - Xk_e[omp_get_thread_num()],
				Zk_n[omp_get_thread_num()] - Zk_e[omp_get_thread_num()]);

		omp_unset_lock(lock2);
		endV[omp_get_thread_num()] = std::chrono::system_clock::now();
		lock_2[omp_get_thread_num()] += endV[omp_get_thread_num()]
				- startV[omp_get_thread_num()];

	}

	//	end = std::chrono::system_clock::now();
	//	all = end - start;

	//	double s2, l1, in, l2 = 0;
	//	for (int i = 0; i < omp_get_max_threads(); i++) {
	//		std::cout << "elapsed setup time in iteration" << i << " = "
	//				<< setup2[i].count() << ";" << std::endl;
	//		s2 += setup2[i].count();
	//		std::cout << "elapsed time for lock1 " << i << "=          "
	//				<< lock_1[i].count() << ";" << std::endl;
	//		l1 += lock_1[i].count();
	//		std::cout << "elapsed time for integration " << i << "=    "
	//				<< integration[i].count() << ";" << std::endl;
	//		in += integration[i].count();
	//		std::cout << "elapse time for lock2 " << i << "=           "
	//				<< lock_2[i].count() << ";" << std::endl;
	//		l2 += lock_2[i].count();
	//	}

	//	std::cout << "elapsed setup iteration: " << s2 << std::endl;
	//	std::cout << "elapsed time for lock1 : " << l1 << std::endl;
	//	std::cout << "elap. time integration : " << in << std::endl;
	//	std::cout << "elapse time for lock2  : " << l2 << std::endl;
	//
	//	std::cout << "elapsed setup time =               " << setup.count() << ";"
	//			<< std::endl;
	//	std::cout << "elapse overall time =              " << all.count() << ";"
	//			<< std::endl;
	//
	//	std::cout << "Multiple Shooting is done" << std::endl;

	return result;
}

Matrix<double> Model::doMultipleShooting(Matrix<double> whichVar) {

	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> parfor;

	Matrix<double> X;
	Matrix<double> U;
	Matrix<double> Z;

	vector<Matrix<double>> Xk(omp_get_max_threads());
	vector<Matrix<double>> Xk_n(omp_get_max_threads());
	vector<Matrix<double>> Uk(omp_get_max_threads());
	vector<Matrix<double>> Zk(omp_get_max_threads());
	vector<Matrix<double>> Zk_n(omp_get_max_threads());

	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		//				std::cout << (*it)->getName() << std::endl;
		if ((*it)->isControlled()) {
			U = horzcat(U, whichVar[(*it)->getPlace()]);
		} else {
			if ((*it)->hasODE()) {
				X = horzcat(X, whichVar[(*it)->getPlace()]);
				if ((*it)->hasALG()) {
					Z = horzcat(Z, whichVar[(*it)->getPlace()]);
				}

			} else {
				if ((*it)->hasALG()) {
					Z = horzcat(Z, whichVar[(*it)->getPlace()]);
				} else {
					if ((*it)->isP()) {
						U = horzcat(U, whichVar[(*it)->getPlace()]);
					}
				}
			}
		}
	}

	vector<std::map<std::string, Matrix<double>> > arg(omp_get_max_threads());
	Matrix<double> result = Matrix<double>::zeros(nTimesteps * nDAE, 1);
	vector<map<std::string, Matrix<double>> > int_out(omp_get_max_threads());

	omp_lock_t* lockX = new omp_lock_t;
	omp_init_lock(lockX);
	omp_lock_t* lockU = new omp_lock_t;
	omp_init_lock(lockU);
	omp_lock_t* lockZ = new omp_lock_t;
	omp_init_lock(lockZ);
	omp_lock_t* lockResult = new omp_lock_t;
	omp_init_lock(lockResult);

	start = std::chrono::system_clock::now();

#pragma omp parallel for
	for (int k = 0; k < nTimesteps; k++) {

		Xk[omp_get_thread_num()] = Matrix<double> ();
		Xk_n[omp_get_thread_num()] = Matrix<double> ();
		Uk[omp_get_thread_num()] = Matrix<double> ();
		Zk[omp_get_thread_num()] = Matrix<double> ();
		Zk_n[omp_get_thread_num()] = Matrix<double> ();

		omp_set_lock(lockX);
		Xk[omp_get_thread_num()] = X[Slice(k, X.size(), X.size1())];
		Xk_n[omp_get_thread_num()] = X[Slice(k + 1, X.size(), X.size1())];
		omp_unset_lock(lockX);
		omp_set_lock(lockU);
		Uk[omp_get_thread_num()] = U[Slice(k, U.size(), U.size1())];
		omp_unset_lock(lockU);
		if (Z.size() > 0) {
			omp_set_lock(lockZ);
			Zk[omp_get_thread_num()] = Z[Slice(k, Z.size(), Z.size1())];
			Zk_n[omp_get_thread_num()] = Z[Slice(k, Z.size(), Z.size1())];
			omp_unset_lock(lockZ);
		}

		arg[omp_get_thread_num()] = make_map("x0", Xk[omp_get_thread_num()],
				"z0", Zk[omp_get_thread_num()], "p", Uk[omp_get_thread_num()]);

		int_out[omp_get_thread_num()] = integrators[omp_get_thread_num()](
				arg[omp_get_thread_num()]);

		if (Z.size() > 0) {
			omp_set_lock(lockResult);
			result[Slice(k * nDAE, (k + 1) * nDAE)] = vertcat(
					Xk_n[omp_get_thread_num()]
							- int_out[omp_get_thread_num()]["xf"],
					Zk_n[omp_get_thread_num()]
							- int_out[omp_get_thread_num()]["zf"]);
			omp_unset_lock(lockResult);
		} else {
			omp_set_lock(lockResult);
			result[Slice(k * nDAE, (k + 1) * nDAE)]
					= Xk_n[omp_get_thread_num()]
							- int_out[omp_get_thread_num()]["xf"];
			omp_unset_lock(lockResult);
		}
	}

	end = std::chrono::system_clock::now();
	parfor = end - start;
	this->integration_times.append(parfor.count());

	return result;
}

void Model::calculateV() {
	this->VMAX = Matrix<double> ();
	this->VMIN = Matrix<double> ();
	//	this->VINIT = Matrix<double>::zeros(1, this->nVariables);
	this->VINIT = Matrix<double> ();
	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		//		std::cout << (*it)->getName() << std::endl;
		//		((*it)->getXU_Min()).print(std::cout << ((*it)->getXU_Min()).dimString()<< "XU_MIN:");
		VMIN = horzcat(VMIN, (*it)->getXU_Min());
		VMAX = horzcat(VMAX, (*it)->getXU_Max());
		//		((*it)->getXU_Init()).print(std::cout << ((*it)->getXU_Init()).dimString()<< "XU_INIT:");
		VINIT = horzcat(VINIT, (*it)->getXU_Init());
	}
}

void Model::writeResultToFile(std::ofstream* filestream) {
	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		(*it)->getVopt().print(*filestream << (*it)->getName() << " = ", false);
		*filestream << ";" << std::endl;
	}
	*filestream << "nTimesteps = " << this->nTimesteps << ";" << std::endl;
	*filestream << "horizon = " << this->horizon << ";" << std::endl;
	*filestream << "nv = " << this->nVariables << ";" << std::endl;
	*filestream << "n_f = " << this->n_f << ";" << std::endl;

	this->integration_times.print(*filestream << "integrationTimes = ", false);
	*filestream << ";" << std::endl;
}

void Model::writeModelToConsole(bool withVOPT) {
	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		std::cout << "Component:  " << (*it)->getName() << std::endl;
		if (!(*it)->isControlled()) {
			std::cout << "State" << std::endl;
			std::cout << VMIN[(*it)->getPlace()].dimString() << std::endl;

		} else {
			std::cout << "Control" << std::endl;
			std::cout << VMIN[(*it)->getPlace()].dimString() << std::endl;
		}
		VINIT[(*it)->getPlace()].print(std::cout << "VINIT: ");
		VMIN[(*it)->getPlace()].print(std::cout << "VMIN:  ");
		VMAX[(*it)->getPlace()].print(std::cout << "VMAX:  ");
		if (withVOPT) {
			(*it)->getVopt().print(std::cout << "V_OPT: ");
		}
		std::cout << std::endl;
	}
}

int Model::getNumberOfVariables() {
	return this->nVariables;
}

void Model::storeAndShiftValues(std::map<string, Matrix<double> > result_tact,
		int t_act) {
	VMIN = Matrix<double> ();
	VMAX = Matrix<double> ();
	VINIT = Matrix<double> ();
	LAM_X = Matrix<double> ();
	LAM_G = Matrix<double> ();
	//	result_tact.print(std::cout);
	for (std::list<Component*>::iterator it = this->begin(); it != this->end(); it++) {
		(*it)->storeAndShift_XU(result_tact, t_act);
		// Update VMIN,VMAX, VINIT, LAM_X
		VMIN = horzcat(VMIN, (*it)->getXU_Min(t_act));
		VMAX = horzcat(VMAX, (*it)->getXU_Max(t_act));
		VINIT = horzcat(VINIT, (*it)->getXU_Init());
		LAM_X = horzcat(LAM_X, (*it)->getLam_X());
		shiftLam_G(result_tact["lam_g"]);
	}
}

void Model::prepareRealtime(int new_nTimesteps, int new_horizon, int n_f) {
	this->n_f = n_f;
	this->nTimesteps = new_nTimesteps;
	this->horizon = new_horizon;

	if (!isRealtime) {
		this->isRealtime = true;
	}

	//Reset
	VINIT = Matrix<double> ();
	VMAX = Matrix<double> ();
	VINIT = Matrix<double> ();
	LAM_X = Matrix<double> ();
	LAM_G = Matrix<double> ();

	V = MX();
	this->integration_times = Matrix<double>();

	for (std::list<Component*>::iterator it = this->begin(); it != this->end(); it++) {
		(*it)->prepareRealtime(new_nTimesteps, new_horizon);
	}

	setPlace();
	this->V = MX::sym("V", this->nVariables);

	// Adjust power Price
	std::cout << "Model prepared for Realtime" << std::endl;
}

void Model::prepareGlobal(int new_nTimesteps, int new_horizon) {
	this->n_f = 1;
	this->nTimesteps = new_nTimesteps;
	this->horizon = new_horizon;

	this->isRealtime = false;

	//Reset
	VINIT = Matrix<double> ();
	VMAX = Matrix<double> ();
	VINIT = Matrix<double> ();
	LAM_X = Matrix<double> ();
	LAM_G = Matrix<double> ();

	V = MX();

	for (std::list<Component*>::iterator it = this->begin(); it != this->end(); it++) {
		(*it)->prepareGlobal(new_nTimesteps);
	}

	setPlace();
	this->V = MX::sym("V", this->nVariables);

	std::cout << "Model prepared for Global" << std::endl;

}

void Model::shiftLam_G(Matrix<double> old_lam_G) {
	int sizeOldLamG = old_lam_G.size();
	this->LAM_G = vertcat(old_lam_G[Slice(nDAE, sizeOldLamG)], old_lam_G[Slice(
			sizeOldLamG - nDAE, sizeOldLamG)]);
}

int Model::getNumberOfInitalValue() {
	return this->nInit;
}

int Model::getHorizon() {
	return this->horizon;
}

int Model::getN_F() {
	return this->n_f;
}

void Model::setIsRealtime(bool isRealtime) {
	this->isRealtime = isRealtime;
}

void Model::setPlace() {
	this->nVariables = 0;
	this->nInit = 0;
	for (std::list<Component*>::iterator it = this->components->begin(); it
			!= this->components->end(); it++) {
		//		std::cout << (*it)->getName() <<std::endl;
		(*it)->setPlace(this->nVariables);
		this->nVariables += (*it)->getSpaceNeeded();
		this->nInit += (*it)->getSpace0();
	}
}

void Model::createIntegrators(std::string name) {
	Dict opts;
	opts = make_dict("tf", (double) horizon / (double) nTimesteps, "abstol",
			1e-6, "reltol", 1e-6, "steps_per_checkpoint", 50, "max_num_steps",
			500);
	integrators = vector<Integrator> (omp_get_max_threads());
	for (int i = 0; i < omp_get_max_threads(); i++) {
		integrators[i] = Integrator("integrator", name, buildDAE(), opts);
		integrators[i].setOption("linear_solver", "csparse");
		//		integrators[i].setOption("regularity_check", true);
	}
	std::cout << "Integrator created" << std::endl;
}

std::string Model::getName() {
	return this->name;
}

std::string Model::getPathEnding() {
	std::string timeDisc = "";
	const int factor = 100;
	switch (horizon * factor / nTimesteps) {
	case 15 * 60 * factor:
		timeDisc = "";
		break;
	case 7.5 * 60.0 * factor:
		timeDisc = "_half";
		break;
	case 3.75 * 60.0 * factor:
		timeDisc = "_quarter";
		break;
	case 1.875 * 60.0 * factor:
		timeDisc = "_eighth";
		break;

	case 1 * factor:
		timeDisc = "";
		break;
	case 0.5 * factor:
		timeDisc = "_half";
		break;
	case 0.25 * factor:
		timeDisc = "_quarter";
		break;

	default:
		break;
	}
	return timeDisc + ".csv";
}

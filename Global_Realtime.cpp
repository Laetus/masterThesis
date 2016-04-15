/*
 * Global_Realtime.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: laetus
 */

#include "Global_Realtime.hpp"

using namespace casadi;

int main(int argc, const char ** argv) {

	std::cout << "Programm started" << std::endl;
	std::string path = "data/tmp"; //Default

	//Define settings for the global solution
	omp_set_num_threads(4);
	path = "data/Example_NoisyCar_4S";
	bool isPlant = false;
	int nTimesteps, horizon, n_f;
	Matrix<double> waterMass_realtime, waterMass_realtimeIpopt,
			waterMass_rtglob, waterMass_global;
	Model* model;
	std::string integrator = "cvodes";
	int stepfactor = 4;

	std::vector<int> choicesH(1);
	std::vector<int> choicesA(3);
	choicesH = {0};
	choicesA = {1, 3, 4};
//	choicesA = {1, 3, 4, 750 * stepfactor * 2};

	if (isPlant) {
		nTimesteps = 24 * 4 * stepfactor;
		horizon = nTimesteps * 15 * 60 / stepfactor; // one timestep is 15 Minutes, as this fits best to the collected data.
		n_f = 1; //The global solver needs only one iteration

		//Setup global model
		model = new LakesDuctAndFlows(nTimesteps, horizon, n_f);
		model->createIntegrators(integrator);
	} else {
		nTimesteps = 52;
		horizon = nTimesteps * 1 / stepfactor; // one timestep is 15 Minutes, as this fits best to the collected data.
		n_f = 1; //The global solver needs only one iteration

		//Setup global model
		model = new Car(nTimesteps, horizon, n_f, false); //isRegCasAssumption
		model->createIntegrators(integrator);
	}

	//Solve optimization problem
	GlobalSolver* solverGL = new GlobalSolver(model);
	solverGL->solve();

	if (isPlant) {
		((HydroModel*) model)->calcProducedPower();
		waterMass_global = ((HydroModel*) model)->waterMass();
	}

	//Analysis
	solverGL->writeResultToFile(path);

	//Setup realtime model
	if (isPlant) {
		n_f = 22 * 4 * stepfactor - 1;
		nTimesteps = 8 * stepfactor;
		horizon = nTimesteps * 15 * 60 / stepfactor;
	} else {
//		n_f = 750 * stepfactor;
		n_f = 200 * stepfactor;
		nTimesteps = 12 * stepfactor;
		horizon = nTimesteps * 1 / stepfactor;
	}

	//Solve smaller optimization problems
	RealtimeAPCSCP* solverAPCSCP;
	int choiceA, choiceH;

	for (unsigned int itA = 0; itA < choicesA.size(); itA++) {
		choiceA = choicesA[itA];
		for (unsigned int itH = 0; itH < choicesH.size(); itH++) {

			model->prepareRealtime(nTimesteps, horizon, n_f);
			model->createIntegrators(integrator);
			choiceH = choicesH[itH];

			solverAPCSCP = new RealtimeAPCSCP(model);

			solverAPCSCP->setChoiceH(choiceH);
			solverAPCSCP->setNupdateA(choiceA);
			//			solverAPCSCP->setNupdateH(n_f + 2 );
			solverAPCSCP->setName("APCSCP_H" + std::to_string(choiceH) + "_A"
					+ std::to_string(choiceA));
			solverAPCSCP->solve();

			//Analysis
			if (isPlant) {
				((HydroModel*) model)->calcProducedPower();
				waterMass_realtime = ((HydroModel*) model)->waterMass();
			}

			solverAPCSCP->writeResultToFile(path);
		}
	}

//	model->prepareRealtime(nTimesteps, horizon, n_f);
//	model->createIntegrators(integrator);
//
//	//Solve smaller optimization problems
//	RealtimeIpopt * solverIpopt = new RealtimeIpopt(model);
//	solverIpopt->solve();
//
//	//Analysis
//	if (isPlant) {
//		((HydroModel*) model)->calcProducedPower();
//		waterMass_realtimeIpopt = ((HydroModel*) model)->waterMass();
//	}
//	solverIpopt->writeResultToFile(path);

	std::cout << "Realtime Simulations done!" << std::endl;

	if (isPlant) {
		nTimesteps = 14 * 4 * stepfactor;
		horizon = nTimesteps * 15 * 60 / stepfactor; // one timestep is 15 Minutes, as this fits best to the collected data.
		n_f = 1;
		model = ((HydroModel*) model);
		model->prepareGlobal(nTimesteps, horizon);
		model->createIntegrators(integrator);
		GlobalSolver* solverRTGlob = new GlobalSolver(model);
		solverRTGlob->setName("RTGlob");
		solverRTGlob->solve();
		((HydroModel*) model)->calcProducedPower();
		waterMass_rtglob = ((HydroModel*) model)->waterMass();
		solverRTGlob->writeResultToFile(path);

		waterMass_global.print(std::cout << "Water mass global: ");
		waterMass_realtime.print(std::cout << "Water mass RT APC: ");
		waterMass_realtimeIpopt.print(std::cout << "Water mass RT IpO: ");
		waterMass_rtglob.print(std::cout << "Water mass RTglob: ");
	}

	std::cout << "End of program!" << std::endl;
}

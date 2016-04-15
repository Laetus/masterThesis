/*
 * vdp_multiple_shooting.cpp
 *
 *  Created on: Nov 5, 2015
 *      Author: laetus
 */

#include "simpleSimulation.hpp"

//#include <stdio.h>
using namespace casadi;

Integrator createIntegrator(SXFunction dae, double steplength) {
	Dict opts;
	Integrator integrator;
	opts = make_dict("tf", steplength, "abstol", 1e-8, "reltol", 1e-8,
			"steps_per_checkpoint", 1000, "max_num_steps", 1000);
	integrator = Integrator("integrator", "idas", dae, opts);
	integrator.setOption("linear_solver", "csparse");
	//	integrator.setOption("regularity_check", false);
	std::cout << "Integrator created" << std::endl;
	return integrator;
}

int main(int argc, const char ** argv) {

std::cout << "Programm started" << std::endl;

// Define global variables/settings
omp_set_num_threads(7);
int nTimesteps = 384; // 384;
int horizon = nTimesteps * 15 * 60; //86400/4;
//Zeit, die pro Realtimeschritt vergeht: horizon / nTimesteps
int n_f = 384 / 4;

//	For Global Solver:
n_f = 1;

//	For Realtime Solver:
LakesAndFlows* model = new LakesAndFlows(nTimesteps, horizon, n_f);
SXFunction dae = model->buildDAE();

Integrator integrator = createIntegrator(dae, (double) horizon
		/ (double) nTimesteps);

////////////////////////////////////////////////////////////////
// Optimization problem										  //
////////////////////////////////////////////////////////////////

//	RealtimeSolver* solver = new RealtimeSolver(model, &integrator,n_f);
GlobalSolver* solver = new GlobalSolver(model, &integrator, n_f);
solver->solve();

////////////////////////////////////////////////////////////////
// Visualisation								  			  //
////////////////////////////////////////////////////////////////

model->calcProducedPower();
model->writeModelToConsole(true);
model->writeResultToFile();

////////////////////////////////////////////////////////////////
// Verification												  //
////////////////////////////////////////////////////////////////

Matrix<double> waterMass = model->waterMass();
std::cout << waterMass.dimString();
waterMass.print(std::cout << "Water Mass:  \n");

}


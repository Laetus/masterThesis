/*
 * vdp_multiple_shooting.cpp
 *
 *  Created on: Nov 5, 2015
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include <coin/IpIpoptApplication.hpp>
#include <plplot.h>
#include <plstream.h>
#include <iostream>
#include <limits>
#include <unistd.h>

#include "../util/PlotHelper.cpp"

using namespace casadi;

int main(int argc, const char ** argv) {

	int nk = 1000;
	double tf = 10.0;
	bool coll = false;

	SX t = SX::sym("t");
	SX u = SX::sym("u");
	SX x = SX::sym("x", 3);

	//ODE rhs function
	SX ode = vertcat( (1 - x[1]*x[1]) * x[0] - x[1] + u, x[0], x[0]*x[0] + x[1]*x[1] + u*u );

	SXFunction dae = SXFunction("dae", daeIn("x",x,"p",u,"t",t), daeOut("ode",ode));

	//Create an integrator
	Dict opts;
	Integrator integrator;
	if (coll) {
		opts = make_dict("tf",tf/nk, "number_of_finite_elements", 5, "interpolation_order", 5 ); //"collocation_scheme", "legendre", "implicit_solver", "kinsol", "expand_f", "true",  "implicit_solver_options", make_dict("linear_solver", "csparse"));

		integrator = Integrator("integrator", "oldcollocation", dae, opts);
	} else {
		opts = make_dict("tf", tf/nk, "abstol" , 1e-8, "reltol", 1e-8, "steps_per_checkpoint", 1000);
		integrator = Integrator("integrator", "cvodes", dae, opts);
	}

	//Total number of variables
	int nv = nk + 3* (nk+1);

	//Declare variable vector
	MX V = MX::sym("V",nv);

	//Get the expression for local variables
	MX U  = V[Slice(0,nk)];
	MX X0 = V[Slice(nk+0, nv, 3)];
	MX X1 = V[Slice(nk+1, nv, 3)];
	MX X2 = V[Slice(nk+2, nv, 3)];

	//Initialize Varaible bounds to +- infinity
	Matrix<double> VMIN = -std::numeric_limits<double>::max()/2 * Matrix<double>::ones(nv,1);
	Matrix<double> VMAX = std::numeric_limits<double>::max()/2 * Matrix<double>::ones(nv,1);

	//control bounds
	VMIN[Slice(0,nk)] = -0.75;
	VMAX[Slice(0,nk)] = 1.0;

	//Initial condition
	VMIN[nk+0] = VMAX[nk+0] = 0;
	VMIN[nk+1] = VMAX[nk+1] = 1;
	VMIN[nk+2] = VMAX[nk+2] = 0;

	// Terminal constraint

	VMIN[nv-3] = VMAX[nv-3] = 0;
	VMIN[nv-2] = VMAX[nv-2] = 0;

	//Initial solution guess
	Matrix<double> VINIT = Matrix<double>::zeros(nv,1);

	//Constraint function with bounds
	std::vector<MX > g ;
	std::vector<Matrix<double>> gmin,gmax;
	MX Xk, Xk_next , Xk_end;
	std::map<std::string, MX> int_out;
	for(int k=0; k<nk; k++){
		//Local state vector
		Xk = vertcat(X0[k],X1[k], X2[k]);
		Xk_next = vertcat(X0[k+1], X1[k+1], X2[k+1]);

		// Call the integrator
		int_out = integrator(make_map("x0", Xk, "p", U[k]));
		Xk_end = int_out["xf"];

		g.push_back(Xk_next - Xk_end);
		gmin.push_back(Matrix<double>::zeros(1,Xk.nnz()));
		gmax.push_back(Matrix<double>::zeros(1,Xk.nnz()));
	}

	//Objective function
	MX f = X2[nk];

	// Condition
	MX G = vertcat(g);

	Matrix<double> GMIN = horzcat(gmin);
	Matrix<double> GMAX = horzcat(gmax);


	MXFunction nlp = MXFunction("nlp", nlpIn("x", V), nlpOut("f", f, "g",G));

	NlpSolver solver = NlpSolver("solver", "ipopt", nlp);

	std::map<std::string, Matrix<double> > arg;
	arg["lbx"] = VMIN;
	arg["ubx"] = VMAX;
	arg["lbg"] = GMIN;
	arg["ubg"] = GMAX;
	arg["x0"] = VINIT;
	std::map<std::string, Matrix<double> > res = solver(arg);

	Matrix<double> v_opt = res["x"];
    Matrix<double> u_opt = v_opt[Slice(0,nk)];
    Matrix<double> x0_opt = v_opt[Slice(nk, nv, 3)];
    Matrix<double> x1_opt = v_opt[Slice(nk+1, nv, 3)];
    Matrix<double> x2_opt = v_opt[Slice(nk+2, nv, 3)];

    std::cout << "Print v_opt" << std::endl;
    v_opt.print(std::cout);
    u_opt.print(std::cout);
    x0_opt.print(std::cout);
    x1_opt.print(std::cout);

    PlotHelper *someStuff = new PlotHelper(argc , argv, u_opt, x0_opt, x1_opt );

    delete someStuff;
}

#include <casadi/casadi.hpp>
#include <iostream>

#include <plplot.h>
#include <plstream.h>

#ifdef PL_USE_NAMESPACE
using namespace std;
#endif

#include "PlotHelper.cpp"

using namespace casadi;

int main(int argc, const char * argv[]) {
	std::cout << "Programm started" << std::endl;

	// Declare variables (use simple, efficient DAG)
	SX x0 = SX::sym("x0");
	SX x1 = SX::sym("x1");
	SX x = vertcat(x0, x1);

	//	x0.print(std::cout);
	//	x1.print(std::cout);
	//	x.print(std::cout);

	//Control
	SX u = SX::sym("u");

	//ODE right hand side
	SX xdot = vertcat((1 - x1 * x1) * x0 - x1 + u, x0);
	//xdot.print(std::cout);

	//Lagrangian
	SX L = x0 * x0 + x1 * x1 + u * u;
	//	L.print(std::cout);

	//Costate
	SX lam = SX::sym("lam", 2);
	//	lam.print(std::cout);

	//Hamiltonian function
	SX H = inner_prod(lam, xdot) + L;
	//	H.print(std::cout);

	//Costate equations
	SX ldot = -gradient(H, x);
	//	ldot.print(std::cout);

	std::cout << "Hamiltonian: ";
	H.print((std::cout));

	//H is of a convex quadratic form in u: H = u*u + p*u +q let's get the coefficient p
	SX p = gradient(H, u);
	//	p.print(std::cout);
	p = substitute(p, u, 0);
	//	p.print(std::cout);

	//H's unconstrained minimizer is: u=-p/2
	SX u_opt = -p / 2;
	//	u_opt.print(std::cout);

	/* we must constrian u to the interval [-0.75. 1.0], convexity of H ensures that the optimum
	 * is obtained at the bound when u_opt is outside the interval*/

	u_opt = fmin(u_opt, 1.0);
	u_opt = fmax(u_opt, -0.75);

	std::cout << "Optimal control: ";
	u_opt.print(std::cout);

	//Augment f with lam_dot and sibstitute in the value for the optimal control
	SX f = vertcat(xdot, ldot);
	f = substitute(f, u, u_opt);
	f.print(std::cout);

	//Create the right hand side function
	std::pair<std::map<std::basic_string<char>, casadi::Matrix<
			casadi::SXElement>, std::less<std::basic_string<char> >,
			std::allocator<std::pair<const std::basic_string<char>,
					casadi::Matrix<casadi::SXElement> > > >, std::vector<
			std::basic_string<char> > > rhs_in = daeIn("x", vertcat(x, lam));

	SXFunction rhs = SXFunction("rhs", rhs_in, daeOut("ode", f));
	//rhs.print(std::cout); checked

	//Augmented state dimension
	int nX = 4;

	//End time
	double tf = 10.0;

	//Number of shooting nodes
	int num_nodes = 20;

	//Create an integrator (CVodes)
	Dict iopts = make_dict("abstol", 1e-8, "reltol", 1e-8, "t0", 0.0, "tf", tf
			/ num_nodes);
	Integrator I = Integrator("I", "cvodes", rhs, iopts);
	//std::cout << std::endl << "Integrator: " << std::endl;
	//I.print(std::cout); //checked

	//Variables in the root finding problem
	int NV = nX * (num_nodes + 1);
	MX V = MX::sym("V", NV);

	//Get the state at each shooting node
	std::vector<MX> X;
	int v_offset = 0;
	for (int k = 0; k < num_nodes + 1; k++) {
		X.push_back(V[Slice(v_offset, v_offset + nX)]);
		v_offset += nX;
	}
	//std::cout << X << std::endl; //checked

	//Formulate the root finding problem
	std::vector<MX> G;

	MX tmp = MX::zeros(2, 1);
	tmp[1] = 1;
	G.push_back(X[0][Slice(0, 2)] - tmp);
	for (int k = 0; k < num_nodes; k++) {
		std::map<std::string, MX> I_out = I(make_map("x0", X[k]));
		MX XF = I_out["xf"];
		G.push_back(XF - X[k + 1]);
	}
	G.push_back(X[num_nodes][Slice(2, 4)]);

	std::cout << std::endl;
	//std::cout << G << std::endl; //checked

	//Terminal constraints: lam=0
	std::vector<MX> V_vector;
	std::vector<MX> G_vector;
	G_vector.push_back(vertcat(G));
	V_vector.push_back(V);
	MXFunction rfp = MXFunction("rfp", V_vector, G_vector);

	//std::cout << std::endl;
	//rfp.print(std::cout); //checked

	//Select a solver for the root-finding problem
	std::string Solver = "nlp";

	//Solver options
	Dict opts = make_dict("nlp_solver", "ipopt", "nlp_solver_options",
			make_dict("hessian_approximation", "limited-memory"));

	//Allocate a solver
	ImplicitFunction solver = ImplicitFunction("solver", Solver, rfp, opts);

	std::map<std::string, DMatrix> arg, res;
	//Solve the problem
	res = solver(arg);

	//Time grid for visualization
	DMatrix tgrid = linspace((DMatrix) 0, (DMatrix) tf, 101);
	//	tgrid.print(std::cout);

	//Output functions
	SXFunction output_fcn = SXFunction("output", rhs_in,
			toVector(x0, x1, u_opt));
	//	output_fcn.print(std::cout); //checked

	std::map<std::basic_string<char>, casadi::Matrix<double> >::iterator bla =
			res.find("o0");
	DMatrix tmpres = res["o0"];
	DMatrix resslice = tmpres[Slice(0, 4)];

	//Simulator to get optimal state and control trajectories
	Simulator simulator = Simulator("simulator", I, output_fcn, tgrid);

	simulator.setInput(resslice, "x0");

	simulator.evaluate();

	simulator.getOutput("o0").print(std::cout);
	simulator.getOutput("o1").print(std::cout);
	simulator.getOutput("o2").print(std::cout);

	//Start to plot stuff
	//init plplot
	PlotHelper *someStuff = new PlotHelper(argc, argv,simulator );
	delete someStuff;

	return 0;

}


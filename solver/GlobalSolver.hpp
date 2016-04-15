/*
 * GlobalSolver.hpp
 *
 *  Created on: Nov 29, 2015
 *      Author: laetus
 */

#ifndef GLOBALSOLVER_HPP_
#define GLOBALSOLVER_HPP_

//#include "Solver.cpp"

class GlobalSolver: public Solver {

public:

	GlobalSolver(Model* model);

	virtual void solve();

protected:

};

#endif /* GLOBALSOLVER_HPP_ */

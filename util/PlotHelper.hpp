/*
 * PlotHelper.hpp
 *
 *  Created on: Nov 4, 2015
 *      Author: laetus
 */
#include <casadi/casadi.hpp>

#ifndef PLOTHELPER_HPP_
#define PLOTHELPER_HPP_

class PlotHelper {
public:
	PlotHelper(std::string name);
	PlotHelper(int, const char **);
	PlotHelper(int, const char **, casadi::Simulator simulator);
	PlotHelper(int argc, const char ** argv, casadi::Matrix<double> v1,
			casadi::Matrix<double> v2, casadi::Matrix<double> v3);
	~PlotHelper();

	void plot1(int);
	void plotVector(casadi::Matrix<double> matrix);
	void plotVector(casadi::Matrix<double> matrix,
			casadi::Matrix<double> matrix2, casadi::Matrix<double> matrix3);
	void plotVector(casadi::Matrix<double> matrix,
			casadi::Matrix<double> matrix2, casadi::Matrix<double> matrix3,
			const char* names[]);

	plstream* getPLS();

private:
	// Class data
	plstream *pls;
	char* path;
	char* name;
};

#endif /* PLOTHELPER_HPP_ */

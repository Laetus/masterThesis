/*
 * plotHelper.cpp
 *
 *  Created on: Nov 4, 2015
 *      Author: laetus
 */

#include "PlotHelper.hpp"
#include "plstrm.h"
#include <casadi/casadi.hpp>
#include <iostream>

PlotHelper::~PlotHelper() {
	delete pls;
}

PlotHelper::PlotHelper(std::string  name) {
	std::string path = "figures/"+name + ".svg";
	const char* pathfile = path.c_str();
	this->pls = new plstream();

	const char * bar[5] = { "", "-bg", "FFFFFF", "-dev", "svg" };
	const char ** foo = &bar[0];
	int argc = 5;
	this->pls->parseopts(&argc, foo, PL_PARSE_FULL);
	this->pls->setopt("-o", pathfile);

	this->pls->init();
	this->pls->font(1);

}

PlotHelper::PlotHelper(int argc, const char ** argv) {
	// plplot initialization

	pls = new plstream();

	// Parse and process command line arguments.
	//	const char argv2[] = "-o someTitle -bg FFFFFF -dev svg";

	pls->parseopts(&argc, argv, PL_PARSE_FULL);
}

PlotHelper::PlotHelper(int argc, const char ** argv,
		casadi::Simulator simulator) {

	pls = new plstream();

	// Parse and process command line arguments.
	//	const char argv2[] = "-o someTitle -bg FFFFFF -dev svg";

	pls->parseopts(&argc, argv, PL_PARSE_FULL);

	// Initialize PLplot.
	pls->init();
	pls->font(1);

	// Make log plots using two different styles.

	casadi::Matrix<double> o0 = simulator.getOutput("o0");
	casadi::Matrix<double> o1 = simulator.getOutput("o1").data();
	casadi::Matrix<double> o2 = simulator.getOutput("o2").data();

	plotVector(o0, o1, o2);

}

PlotHelper::PlotHelper(int argc, const char ** argv, casadi::Matrix<double> v1,
		casadi::Matrix<double> v2, casadi::Matrix<double> v3) {

	pls = new plstream();

	// Parse and process command line arguments.
	//	const char argv2[] = "-o someTitle -bg FFFFFF -dev svg";

	pls->parseopts(&argc, argv, PL_PARSE_FULL);

	// Initialize PLplot.
	pls->init();
	pls->font(1);

	// Make log plots using two different styles.

	plotVector(v1, v2, v3);

}


// Log-linear plot.
void PlotHelper::plotVector(casadi::Matrix<double> matrix) {

	const int m_size = matrix.size();

	PLFLT* x = new PLFLT[m_size];
	PLFLT* y = new PLFLT[m_size];

	//	PLFLT ymin = -1., ymax = 1., xmin = 0., xmax = 10.0;
	//	int i;
	//
	//	for (i = 0; i < m_size; i++) {
	//		x[i] = (PLFLT) xmax * (i) / (PLFLT) (m_size - 1);
	//		y[i] = (PLFLT) matrix.getValue(i);
	//
	//	}

	pls->line((PLINT) m_size, x, y);

	delete x;
	delete y;

}

void PlotHelper::plotVector(casadi::Matrix<double> matrix0, casadi::Matrix<
		double> matrix1, casadi::Matrix<double> matrix2, const char* names[]) {

	const int m_size = matrix0.size();

	PLFLT* x = new PLFLT[m_size];
	PLFLT* y1 = new PLFLT[m_size];
	PLFLT* y2 = new PLFLT[m_size];
	PLFLT* y0 = new PLFLT[m_size];

	PLFLT ymin = -.7, ymax = 10;
	PLFLT xmin = 0.0, xmax = matrix0.size();
	int i;

	for (i = 0; i < m_size; i++) {
		x[i] = (PLFLT) xmax * (i) / (PLFLT) (m_size - 1);
		double a = matrix0.getValue(i);
		double b = matrix1.getValue(i);
		double c = matrix2.getValue(i);
		y0[i] = (PLFLT) a;
		y1[i] = (PLFLT) b;
		y2[i] = (PLFLT) c;

		if (x[i] < xmin){
			xmin = x[i];
		}
		if(x[i] > xmax){
			xmax = x[i];
		}


		PLFLT tmax = (PLFLT) std::max(std::max(a,b),c);
		PLFLT tmin = (PLFLT) std::min(std::min(a,b),c);

		if(tmin < ymin){
		 	ymin = tmin;
		}
		if (tmax > ymax){
			ymax = tmax;
		}
	}

	pls->env(xmin, xmax, ymin, ymax, 0, 0);
	pls->lab("time", "", "Simple Plot");

	pls->col0(8);
	pls->line((PLINT) m_size, x, y0);
	pls->col0(9);
	pls->line((PLINT) m_size, x, y1);
	pls->col0(3);
	pls->line((PLINT) m_size, x, y2);

	PLINT nlegend = 3;
	//Legend

	PLINT opt_array[3];
	PLINT text_colors[3];
	PLINT line_styles[3];
	PLINT line_colors[3];
	PLFLT line_widths[3];
	PLINT symbol_numbers[3];
	PLINT symbol_colors[3];
	PLFLT symbol_scales[3];

	const char *text[3], *symbols[3];

	//erster Eintrag
	opt_array[0] = PL_LEGEND_LINE;
	text_colors[0] = 8;
	text[0] = names[0];
	line_colors[0] = 8;
	line_styles[0] = 1;
	line_widths[0] = 1.;
	symbols[0] = "";

	//erster Eintrag
	opt_array[1] = PL_LEGEND_LINE;
	text_colors[1] = 9;
	text[1] = names[1];
	line_colors[1] = 9;
	line_styles[1] = 1;
	line_widths[1] = 1.;
	symbols[1] = "";

	opt_array[2] = PL_LEGEND_LINE;
	text_colors[2] = 3;
	text[2] = names[2];
	line_colors[2] = 3;
	line_styles[2] = 1;
	line_widths[2] = 1.;
	symbols[2] = "";

	PLFLT legend_width, legend_height;

	legend_width = 1000.0;
	legend_height = 2000.0;
	pls->legend(&legend_width, &legend_height, PL_LEGEND_BACKGROUND
			| PL_LEGEND_BOUNDING_BOX, 0, 0.0, 0.0, 0.1, 15, 1, 1, 0, 0,
			nlegend, opt_array, 1.0, 1.0, 1.0, 1.0, text_colors,
			(const char **) text, NULL, NULL, NULL, NULL, line_colors,
			line_styles, line_widths, symbol_colors, symbol_scales,
			symbol_numbers, (const char **) symbols);

	delete x;
	delete y0;
	delete y1;
	delete y2;

}

void PlotHelper::plotVector(casadi::Matrix<double> matrix0, casadi::Matrix<
		double> matrix1, casadi::Matrix<double> matrix2) {
	const char* names[3] = { "1", "2", "3" };
	this->plotVector(matrix0, matrix1, matrix2, names);
}

plstream* PlotHelper::getPLS() {
	return this->pls;
}

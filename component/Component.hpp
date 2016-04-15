/*
 * Component.hpp
 *
 *  Created on: Nov 11, 2015
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include <iostream>
#include <math.h>

#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

using namespace casadi;

class Component {

public:
	const static double GRAV_ACC;
	const static double DENSITY_WATER;
	static int counter;

	Component();

	Component(int nTimesteps, SX* t);

	SX getx();
	SX getu();
	SX* gett();

	virtual bool isControlled();

	virtual SX getODE() = 0;
	virtual bool hasODE();
	virtual bool hasALG();
	virtual SX getALG();

	virtual SX getz();
	template<class T> T getZ(T V, int k);
	virtual bool isP();
	virtual std::string getName();

	//Side conditions and storage
	virtual int getSpaceNeeded();
	virtual int getSpace0();
	virtual Slice getPlace();
	virtual void setPlace(int beginning);
	virtual Slice getPlace0();

	virtual Matrix<double> getXU_Min();
	virtual Matrix<double> getXU_Max();
	virtual Matrix<double> getXU_Min(int t_act);
	virtual Matrix<double> getXU_Max(int t_act);
	virtual Matrix<double> getXU_0(); //initial value for ode/dae
	virtual Matrix<double> getXU_Init(); //initial value for optimization

	virtual void setXU_Min(Matrix<double> );
	virtual void setXU_Max(Matrix<double> );
	virtual void setXU_0(Matrix<double> );
	virtual void setXU_Init(Matrix<double>, int t_act );

	virtual Matrix<double> estimateNewPoint(Matrix<double> xu_initial, int t_act);

	virtual void setVopt(Matrix<double> );
	virtual Matrix<double> getVopt();
	virtual Matrix<double> getGlobalOpt();

	virtual void storeAndShift_XU(
			std::map<std::string, Matrix<double> > res_tact, int t_act);
	virtual void storeSolution(Matrix<double> sol_part);

	virtual void prepareRealtime(int new_nTimesteps, int new_horizon);
	virtual void prepareGlobal(int new_nTimesteps);

	virtual void setUp() = 0;


	virtual Matrix<double> getLam_X();
	virtual void setLam_X(Matrix<double> lam_X);

	virtual void setMeasurementError(std::string path);
	virtual void resetErrorList();
	virtual Matrix<double> getError(int size);



protected:
	int nTimesteps;
	int iD;
	int t_f;

	std::string name;
	bool isCntrld;
	SX x;
	SX u;
	SX* t;

	//Side conditions and storage
	int space_needed;
	Slice place_x;
	Slice place_u;

	bool hasOde;
	bool hasAlg;
	bool isRealtime;

	int space_0;
	Slice place_x0;
	Slice place_u0;

	Matrix<double> xu_min;
	Matrix<double> xu_max;
	Matrix<double> xu_0;
	Matrix<double> xu_init;

	Matrix<double> result;
	Matrix<double> lambda_x;

	Matrix<double> result_glob;
	Matrix<double> lambda_x_glob;

	std::vector<double> measurementErrorStorage;
	std::vector<double> errorList;
};

int Component::counter = 0;
const double Component::GRAV_ACC = 9.81;
const double Component::DENSITY_WATER = 998.204; // at 20°C athmospheric pressure

#endif /* COMPONENT_HPP_ */

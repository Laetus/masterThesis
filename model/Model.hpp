/*
 * Model.hpp
 *
 *  Created on: Nov 19, 2015
 *      Author: laetus
 */

#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "../component/Component.hpp"
#include <omp.h>
#include <chrono>

using namespace std;

class Model {
public:
	Model(int nTimesteps, int horizon,int n_f);
	list<Component*>* getComponents();
	list<Component*>::iterator begin();
	list<Component*>::iterator end();

	virtual void setPlace();
	virtual void calculateV();

	virtual void createIntegrators(std::string name);
	MX doMultipleShooting( MX whichVar);
	Matrix<double> doMultipleShooting( Matrix<double> whichVar);
	virtual casadi::SXFunction buildDAE();

	virtual Matrix<double> getVINIT();
	virtual Matrix<double> getVMAX();
	virtual Matrix<double> getVMIN();
	virtual Matrix<double> getLAM_X();
	virtual Matrix<double> getLAM_G();
	virtual MX getV();
	virtual MX getf(int t_act) = 0;
	virtual void setIsRealtime(bool isRealtime);
	virtual bool getIsRealtime();

	virtual int getNumberOfVariables();
	virtual int getNumberOfInitalValue();
	virtual int getHorizon();
	int getN_F();

	virtual void prepareRealtime(int new_nTimesteps, int new_horizon, int n_f);

	virtual void prepareGlobal(int new_nTimesteps, int new_horizon);

	/*
	 * Moves the value of 1 as starting value to 0 and shifts every entry in
	 * the initial value by one.
	 */
	virtual void storeAndShiftValues(std::map<string,Matrix<double> > result_tact, int t_act);
	virtual void shiftLam_G(Matrix<double> old_lam_G);

	virtual void writeResultToFile(std::ofstream* filestream);

	void writeModelToConsole(bool withVOPT);

	int getNTimesteps();
	std::string getName();

protected:

	virtual std::string getPathEnding();

	int nTimesteps;
	int horizon;
	int n_f;

	std::vector<Integrator> integrators;

	/*how many variables are needed to describe the hole model?*/
	int nVariables;
	int nInit; /*how many variables are needed to describe the initial value of a model.
	 In the general setting: nInit * horizon = nVariables . In the realtime setting, this is different*/
	int nDAE;
	SXFunction dae;

	list<Component*> * components;

	Matrix<double> VINIT;
	Matrix<double> VMAX;
	Matrix<double> VMIN;
	Matrix<double> LAM_X;
	Matrix<double> LAM_G;

	Matrix<double> integration_times;


	MX V;
	MX f;
	SX t;

	bool isRealtime;
	std::string name;


};

#endif /* MODEL_HPP_ */

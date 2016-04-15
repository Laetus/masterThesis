/*
 * SimpleLake.cpp
 *
 *  Created on: Nov 24, 2015
 *      Author: laetus
 */

#include "SimpleLake.hpp"

using namespace casadi;

SimpleLake::SimpleLake(int nTimesteps, int horizon, int n_f) :
	LakesAndFlows::HydroModel(nTimesteps, horizon, n_f) {
	this->t = SX::sym("time");
	this->name = "SimpleLake";
	this->lake = new Lake(nTimesteps, 80000, 13.0, 30, &t);

	this->inflow = new ExtFlow(nTimesteps, &t, true, "usual" + getPathEnding());
	this->outflow = new Cflow(nTimesteps, &t, 0.0, 150, 88.46, 0.0, 0.0);

//			ExtFlow(nTimesteps, &t, false, 300, 60, 98.46);

	this->opt_height = 15.0;
	this->powerPrice = Matrix<double>::zeros(nTimesteps,1);

	//Connect the lakes
	this->lake->addFlow(this->inflow);
	this->lake->addFlow(this->outflow);

	//Connect the flows
	this->inflow->setComponentB(this->lake);
	this->outflow->setComponentA(this->lake);


	this->lake->setMeasurementError("data/error2.csv");
	this->inflow->setMeasurementError("data/error1.csv");


	//Add to lists
	this->components = new std::list<Component*>();
	this->components->push_front(this->lake);
	this->components->push_front(this->inflow);
	this->components->push_front(this->outflow);

	this->controls = new std::list<Cflow*>();
//	this->controls->push_front(this->outflow); A component only
	setPlace();

	this->V = MX::sym("V", this->nVariables);
	std::cout << "Model created" << std::endl;
}

Matrix<double> SimpleLake::waterMass() {
	massWater = lake->getVopt() * lake->getSurface();
	return massWater;
}

casadi::MX SimpleLake::getf(int t_act) {

	Matrix<double> selector = Matrix<double>::zeros(V.size(), 1);
	MX shift = MX::zeros(V.size(), 1);

	selector[lake->getPlace()] = 1;
	shift[lake->getPlace()] = opt_height;

	MX result =  mul(transpose((V * selector) - shift), (V * selector) - shift);
	return result;
}

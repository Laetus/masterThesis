/*
 * LakesAndFlows.cpp
 *
 *  Created on: Nov 24, 2015
 *      Author: laetus
 */

#include "LakesAndFlows.hpp"

using namespace casadi;

LakesAndFlows::LakesAndFlows(int nTimesteps, int horizon, int n_f) :
	LakesAndFlows::HydroModel(nTimesteps, horizon, n_f) {
	this->t = SX::sym("time");
	this->name = "LakesAndFlows";
	this->lake0 = new Lake(nTimesteps, 80000, 1.0, 30, &t);
	this->lake1 = new Lake(nTimesteps, 100000, 2.0, 10, &t);
	this->lake2 = new Lake(nTimesteps, 150000, 2.0, 15, &t);
	this->lake3 = new Lake(nTimesteps, 300000, 1.0, 7.5, &t);

	this->pump10 = new Cflow(nTimesteps, &t, 0.0, 80.0, 0.0, 1.2, -125);
	this->turb02 = new Cflow(nTimesteps, &t, 0.0, 320.0, 10, 0.85, 150);
	this->turb12 = new Cflow(nTimesteps, &t, 0.0, 600.0, 88.46, 0.75, 25);
	this->turb23 = new Cflow(nTimesteps, &t, 0.0, 600.0, 98.46, 0.75, 12);

	this->inflow0 = new ExtFlow(nTimesteps, &t, true, 10, 10, 10);
	this->inflow1
			= new ExtFlow(nTimesteps, &t, true, "usual" + getPathEnding());
	this->outflow3 = new ExtFlow(nTimesteps, &t, false, 300, 60, 98.46);

	//Connect the lakes
	this->lake0->addFlow(this->inflow0);
	this->lake0->addFlow(this->pump10);
	this->lake0->addFlow(this->turb02);

	this->lake1->addFlow(this->inflow1);
	this->lake1->addFlow(this->pump10);
	this->lake1->addFlow(this->turb12);

	this->lake2->addFlow(this->turb12);
	this->lake2->addFlow(this->turb02);
	this->lake2->addFlow(this->turb23);

	this->lake3->addFlow(this->turb23);
	this->lake3->addFlow(this->outflow3);

	//Connect the flows
	this->inflow0->setComponentB(this->lake0);
	this->inflow1->setComponentB(this->lake1);
	this->outflow3->setComponentA(this->lake3);

	this->pump10->setComponentA(this->lake1);
	this->pump10->setComponentB(this->lake0);

	this->turb12->setComponentA(this->lake1);
	this->turb12->setComponentB(this->lake2);

	this->turb02->setComponentA(this->lake0);
	this->turb02->setComponentB(this->lake2);

	this->turb23->setComponentA(this->lake2);
	this->turb23->setComponentB(this->lake3);

	//Load Price from file:
	std::vector<Matrix<double>*> vct(2);
	vct = {&powerPrice , &powerPriceRT};
	Utils::readFromFile("data/powerPrice_dayAhead_intraDay_3_7_2015__2"
			+ getPathEnding(), &vct);

	//	this->inflow0->setMeasurementError("data/error2.csv");
	//	this->inflow1->setMeasurementError("data/error3.csv");


	//Add to lists
	this->components = new std::list<Component*>();
	this->components->push_front(this->lake0);
	this->components->push_front(this->lake1);
	this->components->push_front(this->lake2);
	this->components->push_front(this->lake3);
	this->components->push_front(this->inflow0);
	this->components->push_front(this->inflow1);
	this->components->push_front(this->outflow3);
	this->components->push_front(this->pump10);
	this->components->push_front(this->turb02);
	this->components->push_front(this->turb12);
	this->components->push_front(this->turb23);

	this->controls = new std::list<Cflow*>();
	this->controls->push_front(this->pump10);
	this->controls->push_front(this->turb02);
	this->controls->push_front(this->turb12);
	this->controls->push_front(this->turb23);

	setPlace();

	this->V = MX::sym("V", this->nVariables);
	std::cout << "Model created" << std::endl;
}

Matrix<double> LakesAndFlows::waterMass() {
	massWater = lake0->getVopt() * lake0->getSurface();
	massWater += lake1->getVopt() * lake1->getSurface();
	massWater += lake2->getVopt() * lake2->getSurface();
	massWater += lake3->getVopt() * lake3->getSurface();

	return massWater;
}

casadi::MX LakesAndFlows::getf(int t_act) {
	MX result;
	MX producedPower_tact = pump10->calculatePower(V);
	producedPower_tact += turb02->calculatePower(V);
	producedPower_tact += turb12->calculatePower(V);
	producedPower_tact += turb23->calculatePower(V);
	Matrix<double> powerPrice_tact, producedPower_should;

	double intervall_length_in_h = (horizon / nTimesteps) / 60;
	if (!isRealtime) {
		powerPrice_tact = powerPrice[Slice(t_act, t_act + nTimesteps)];
		producedPower_should = Matrix<double>::zeros(nTimesteps, 1);
	} else {
		powerPrice_tact = powerPriceRT[Slice(t_act, t_act + nTimesteps)];
		producedPower_should = transpose(producedPower[Slice(t_act, t_act
				+ nTimesteps)]);
	}
	result = mul(-transpose(producedPower_tact - producedPower_should),
			intervall_length_in_h * powerPrice_tact);

	Matrix<double> selector = Matrix<double>::zeros(V.size(), 1);
	MX shift = MX::zeros(V.size(), 1);
	selector[turb12->getPlace()] = 1;
	shift[turb12->getPlace()] = 88.46;
	selector[turb23->getPlace()] = 1;
	shift[turb23->getPlace()] = 98.46;

	selector[lake1->getPlace()] = 1;
	shift[lake1->getPlace()] = 2;
	selector[lake2->getPlace()] = 1;
	shift[lake2->getPlace()] = 2;
	selector[lake3->getPlace()] = 1;
	shift[lake3->getPlace()] = 1;

	double lambda = 1;
	result += lambda * mul(transpose((V * selector) - shift), (V * selector) - shift);

	return result;
}

/*
 * LakesDuctsAndFlows.cpp
 *
 *  Created on: Dec 7, 2015
 *      Author: laetus
 */

#include "LakesDuctsAndFlows.hpp"

using namespace casadi;

LakesDuctsAndFlows::LakesDuctsAndFlows(int nTimesteps, int horizon, int n_f) :
	LakesDuctsAndFlows::HydroModel(nTimesteps, horizon, n_f) {
	this->t = SX::sym("time");
	this->name = "LakesDuctsAndFlows";
	this->lake0 = new Lake(nTimesteps, 80000, 10.0, 30, &t);
	this->lake1 = new Lake(nTimesteps, 100000, 7.0, 10, &t);
	this->lake2 = new Lake(nTimesteps, 150000, 4.0, 15, &t);
	this->lake3 = new Lake(nTimesteps, 300000, 6.0, 7.5, &t);
	this->lake3b = new Lake(nTimesteps, 300000, 6.0, 7.5, &t);
	this->lake0b = new Lake(nTimesteps, 80000, 10.0, 25, &t);

	this->duct3 = new LakeDuct(nTimesteps, &t, this->lake3, this->lake3b, 0.0,
			10);
	this->duct0 = new LakeDuct(nTimesteps, &t, this->lake0, this->lake0b, 0,
			5.5);

	this->pump10 = new Cflow(nTimesteps, &t, 0.0, 30.0, 0.0, 1.2, -125);
	this->turb02 = new Cflow(nTimesteps, &t, 0.0, 50.0, 10, 0.85, 150);
	this->turb12 = new Cflow(nTimesteps, &t, 0.0, 150.0, 88.46, 0.75, 25);
	this->turb23 = new Cflow(nTimesteps, &t, 0.0, 200.0, 98.46, 0.75, 12);

	this->inflow0 = new ExtFlow(nTimesteps, &t, true, 10, 10, 10);
	this->inflow1 = new ExtFlow(nTimesteps, &t, true, "easyflow"
			+ getPathEnding());
	this->outflow3 = new ExtFlow(nTimesteps, &t, false, 600, 30, 98.46);

	//Connect the lakes
	this->lake0->addFlow(this->inflow0);
	this->lake0->addFlow(this->pump10);
	this->lake0->addFlow(this->turb02);
	this->lake0->addFlow(this->duct0);

	this->lake0b->addFlow(this->duct0);

	this->lake3->addFlow(this->duct3);
	this->lake3b->addFlow(this->duct3);

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
	this->components->push_front(this->lake3b);
	this->components->push_front(this->lake0b);
	this->components->push_front(this->inflow0);
	this->components->push_front(this->inflow1);
	this->components->push_front(this->outflow3);
	this->components->push_front(this->duct3);
	this->components->push_front(this->duct0);
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

Matrix<double> LakesDuctsAndFlows::waterMass() {
	massWater = lake0->getVopt() * lake0->getSurface();
	massWater += lake3b->getVopt() * lake3b->getSurface();
	massWater += lake1->getVopt() * lake1->getSurface();
	massWater += lake2->getVopt() * lake2->getSurface();
	massWater += lake3->getVopt() * lake3->getSurface();
	massWater += lake0b->getVopt() * lake0b->getSurface();

	return massWater;
}

casadi::MX LakesDuctsAndFlows::getf(int t_act) {
	MX result;
	Matrix<double> selector = Matrix<double>::zeros(V.size(), 1);
	MX shift = MX::zeros(V.size(), 1);
	if (isRealtime) {
		selector[lake0->getPlace()] = 1;
		selector[lake0b->getPlace()] = 1;
		selector[lake1->getPlace()] = 1;
		selector[lake2->getPlace()] = 1;
		selector[lake3->getPlace()] = 1;
		selector[lake3b->getPlace()] = 1;
		//		selector[turb02->getPlace()] = 1;
		//		selector[turb12->getPlace()] = 1;
		//		selector[turb23->getPlace()] = 1;
		//		selector[pump10->getPlace()] = 1;
		//		selector[duct0->getPlace()] = 1;
		//		selector[duct3->getPlace()] = 1;

		//		shift[lake0->getPlace()] = lake0->getGlobalOpt()[Slice(t_act, t_act
		//				+ lake0->getSpaceNeeded())];
		//
		//		shift[lake0b->getPlace()] = lake0b->getGlobalOpt()[Slice(t_act, t_act
		//				+ lake0b->getSpaceNeeded())];
		//		shift[lake1->getPlace()] = lake1->getGlobalOpt()[Slice(t_act, t_act
		//				+ lake1->getSpaceNeeded())];
		//		shift[lake2->getPlace()] = lake2->getGlobalOpt()[Slice(t_act, t_act
		//				+ lake2->getSpaceNeeded())];
		//		shift[lake3->getPlace()] = lake3->getGlobalOpt()[Slice(t_act, t_act
		//				+ lake3->getSpaceNeeded())];
		//		shift[lake3b->getPlace()] = lake3b->getGlobalOpt()[Slice(t_act, t_act
		//				+ lake3b->getSpaceNeeded())];

		shift[lake1->getPlace()] = 7;
		shift[lake2->getPlace()] = 4;
		shift[lake3->getPlace()] = 6;
		shift[lake3b->getPlace()] = 6;
		shift[lake0->getPlace()] = 10;
		shift[lake0b->getPlace()] = 10;

		//		shift[turb02->getPlace()] = turb02->getGlobalOpt()[Slice(t_act, t_act
		//				+ turb02->getSpaceNeeded())];
		//		shift[turb12->getPlace()] = turb12->getGlobalOpt()[Slice(t_act, t_act
		//						+ turb12->getSpaceNeeded())];
		//		shift[turb23->getPlace()] = turb23->getGlobalOpt()[Slice(t_act, t_act
		//						+ turb23->getSpaceNeeded())];
		//		shift[pump10->getPlace()] = 0.04 * pump10->getGlobalOpt()[Slice(t_act,
		//				t_act + pump10->getSpaceNeeded())];

		result = mul(transpose((V * selector) - shift), (V * selector) - shift);

		//		MX producedPower_tact = pump10->calculatePower(V);
		//		producedPower_tact += turb02->calculatePower(V);
		//		producedPower_tact += turb12->calculatePower(V);
		//		producedPower_tact += turb23->calculatePower(V);
		//		Matrix<double> powerPrice_tact, producedPower_should;
		//		double intervall_length_in_h = (horizon / nTimesteps) / 60;
		//
		//		powerPrice_tact = powerPriceRT[Slice(t_act, t_act + nTimesteps)];
		//		producedPower_should = transpose(this->producedPower[Slice(t_act, t_act  + nTimesteps)]);
		//		result += mul(-transpose(producedPower_tact - producedPower_should),
		//				intervall_length_in_h * powerPrice_tact);

	} else {

		result = 0;
		//		MX producedPower_tact = pump10->calculatePower(V);
		//		producedPower_tact += turb02->calculatePower(V);
		//		producedPower_tact += turb12->calculatePower(V);
		//		producedPower_tact += turb23->calculatePower(V);
		//		Matrix<double> powerPrice_tact, producedPower_should;
		//		double intervall_length_in_h = (horizon / nTimesteps) / 60;
		//
		//		powerPrice_tact = 1e-5 * powerPrice[Slice(t_act, t_act + nTimesteps)];
		//		producedPower_should = Matrix<double>::zeros(nTimesteps, 1);
		//		result = mul(-transpose(producedPower_tact - producedPower_should),
		//				intervall_length_in_h * powerPrice_tact);

		//		selector[lake0->getPlace()] = 0.25;
		//		selector[lake0b->getPlace()] = 0.25;
		selector[lake1->getPlace()] = 1;
		selector[lake2->getPlace()] = 1;
		selector[lake3->getPlace()] = 1;
		selector[lake3b->getPlace()] = 1;
		selector[lake0->getPlace()] = 1;
		selector[lake0b->getPlace()] = 1;

		selector[pump10->getPlace()] = 1;

		//		shift[lake0->getPlace()] = 0.25;
		//		shift[lake0b->getPlace()] = 0.25;
		shift[lake1->getPlace()] = 7;
		shift[lake2->getPlace()] = 4;
		shift[lake3->getPlace()] = 6;
		shift[lake3b->getPlace()] = 6;
		shift[lake0->getPlace()] = 10;
		shift[lake0b->getPlace()] = 10;

		//		selector[duct0->getPlace()] = 0.1;
		//		selector[duct3->getPlace()] = 0.1;

		result
				+= mul(transpose((V * selector) - shift), (V * selector)
						- shift);
	}

	return result;
}


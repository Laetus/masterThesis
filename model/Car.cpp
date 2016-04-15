/*
 * Car.cpp
 *
 *  Created on: Jan 12, 2016
 *      Author: laetus
 */

#include "Car.hpp"

using namespace casadi;

Car::Car(int nTimesteps, int horizon, int t_f, bool isRegCasAssumption) :
	Car::Model(nTimesteps, horizon, t_f) {

	this->t = SX::sym("time");
	this->name = "SimpleCar";
	this->acceleration = new Acceleration(nTimesteps, -4.5, 3.4, &t);
	this->velocity = new Velocity(nTimesteps, acceleration, 30 / 3.6, 20 / 3.6,
			&t);
	this->steering = new Steering(nTimesteps, M_PI / 8, &t);
	this->orientation = new Orientation(nTimesteps, velocity, steering, 3.21,
			&t);
	this->xposition = new Position(nTimesteps, orientation, horizon, "x",
			isRegCasAssumption, &t);
	this->yposition = new Position(nTimesteps, orientation, horizon, "y",
			isRegCasAssumption, &t);

	this->xposition->setPredecessor_pos("data/noisy_actionCar" + getPathEnding());

	this->yposition->setMeasurementError("data/error1" + getPathEnding());
	this->xposition->setMeasurementError("data/error2" + getPathEnding());
//	this->velocity->setMeasurementError("data/error3" + getPathEnding());

	this->components = new std::list<Component*>();
	this->components->push_back(acceleration);
	this->components->push_back(velocity);
	this->components->push_back(steering);
	this->components->push_back(orientation);
	this->components->push_back(xposition);
	this->components->push_back(yposition);

	this->controls = new std::list<Component*>();
	this->controls->push_back(steering);
	this->controls->push_back(acceleration);

	setPlace();

	this->V = MX::sym("V", this->nVariables);
	std::cout << "Model created" << std::endl;

}

casadi::MX Car::getf(int t_act) {

	Matrix<double> selector = Matrix<double>::zeros(V.size(), 1);
	MX shift = MX::zeros(V.size(), 1);

	selector[xposition->getPlace()] = 1;
//	selector[xposition->getPlace0()] = 1;
	shift[xposition->getPlace()] = xposition->getPredecessorPosition(t_act)
			- (2 * xposition->getSafetyDistance(V));
	//	selector[velocity->getPlace()] = 1;
//	selector[velocity->getPlace0()] = 0;

	selector[yposition->getPlace()] = 10;
	selector[orientation->getPlace()] = 10;

	selector[steering->getPlace0()] = 1; //is fixed anyway
	selector[steering->getPlace1()] = 10;
	selector[acceleration->getPlace0()] = 1; //is fixed anyway
//	selector[acceleration->getPlace1()] = 0.5;

	MX result = norm_2((V * selector) - shift);
	return result;
}

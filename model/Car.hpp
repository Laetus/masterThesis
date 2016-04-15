/*
 * Car.hpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#ifndef CAR_HPP_
#define CAR_HPP_

#include "Model.hpp"

class Car: public Model {

public:
	Car(int nTimesteps, int horizon, int t_f, bool isRegCasAssumption);

	virtual casadi::MX getf(int t_act);

protected:

	std::list<Component*>* controls;
	Steering* steering;
	Acceleration* acceleration;
	Velocity* velocity;
	Orientation* orientation;
	Position* xposition;
	Position* yposition;


};

#endif /* CAR_HPP_ */

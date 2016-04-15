
/*
 * Steering.hpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include "Component.hpp"

#ifndef STEERING_HPP_
#define STEERING_HPP_

using namespace casadi;

class Steering: public Component {
public:
	Steering(int nTimesteps, double maxChange, SX* t);

	SX getODE();

	virtual void setUp();
	virtual void setPlace(int beginning);
	virtual Slice getPlace1();

protected:
	double maxChange;
	Slice place_u1;
};

#endif /* STEERING_HPP_ */

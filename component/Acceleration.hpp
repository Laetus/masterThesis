/*
 * Acceleration.hpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include "Component.hpp"


#ifndef ACCELERATION_HPP_
#define ACCELERATION_HPP_

using namespace casadi;

class Acceleration : public Component {

public:
	Acceleration(int nTimesteps, double amin, double amax, SX* t);

	SX getODE();

	virtual void setUp();
	virtual void setPlace(int beginning);
	virtual Slice getPlace1();

	virtual double getAmin();

private:
	double amin;
	double amax;

	Slice place_u1;

};
#endif /* ACCELERATION_HPP_ */

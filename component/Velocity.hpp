/*
 * Velocity.hpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include "Component.hpp"

#ifndef VELOCITY_HPP_
#define VELOCITY_HPP_

using namespace casadi;

class Velocity: public Component {

public:
	Velocity(int nTimesteps, Acceleration* a,  double maxV, double v0,  SX* t);

	SX getODE();

	virtual void setUp();
	virtual void setPlace(int beginning);
	virtual double getV0();
	void setXU_0(Matrix<double> xu_0);

	virtual Acceleration* getAcceleration();

protected:
	Acceleration* a;
	double maxV;
	double v0;
};

#endif /* VELOCITY_HPP_ */

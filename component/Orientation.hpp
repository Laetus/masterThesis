/*
 * Orientation.hpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include "Component.hpp"
#include "Velocity.hpp"
#include "Steering.hpp"

#ifndef ORIENTATION_HPP
#define ORIENTATION_HPP

using namespace casadi;

class Orientation : public Component {
public:

	Orientation(int nTimesteps, Velocity* v, Steering* s, double turnRadius, SX* t);
	SX getODE();

	Velocity* getVelocity();
	virtual void setUp();
	virtual void setPlace(int beginning);

private:

	double wheelBase;
	Velocity* velocity;
	Steering* steering;

};

#endif /* ORIENTATION_HPP */

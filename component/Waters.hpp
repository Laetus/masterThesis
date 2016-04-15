/*
 * Water.hpp
 *
 *  Created on: Nov 11, 2015
 *      Author: laetus
 */

//#include "Component.cpp"

#ifndef WATER_HPP_
#define WATER_HPP_

class Waters : public Component {


public:

	Waters();

	Waters(int nTimesteps, SX* t);

	virtual double getMaxHeight();
	void setMaxHeight(double maxHeight);

	virtual casadi::SX getHeight(int place) = 0;

	/*virtual casadi::SX getFlowIn(casadi::SX *t) = 0;
	virtual casadi::SX getFlowOut(casadi::SX *t) = 0;*/

protected:

	double maxHeight;

};

#endif /* WATER_HPP_ */

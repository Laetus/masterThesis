/*
 * Flow.hpp
 *
 *  Created on: Nov 13, 2015
 *      Author: laetus
 */

#include "Component.hpp"

#ifndef FLOW_HPP_
#define FLOW_HPP_

class Flow : public Component {

public:

	Flow(int nTimeSteps, casadi::SX* t);
	Flow(int nTimeSteps, casadi::SX* t, double xu_max, double xu_min, double xu_0);

	virtual casadi::SX getODE();
//	virtual bool hasODE();
	virtual std::string getName();
	virtual bool isP();
	void setP(bool p);
	casadi::SX getFlow();

	casadi::SX getFlowTo(Component* component);

	void setIsControlled(bool isControlled);
	virtual bool isControlled();

	void setComponentA(Component* a);
	void setComponentB(Component* b);
	Component* getComponentA();
	Component* getComponentB();

	virtual void setUp();

	//Side conditions and storage

	virtual void setPlace(int beginning);

protected:

	Component* A;
	Component* B;

	double flowMax;
	double flowMin;
	double flow0;

	bool controlled;
	bool parameter;
};

#endif /* FLOW_HPP_ */

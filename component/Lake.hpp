/*
 * Lake.hpp
 *
 *  Created on: Nov 9, 2015
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include <iostream>
#include "Waters.hpp"
#include "Flow.hpp"


#ifndef LAKE_HPP_
#define LAKE_HPP_

using namespace casadi;

class Lake : public Waters {
public:
	Lake(int nTimesteps, double surface, double h0, SX* t);
	Lake(int nTimesteps, double surface, double h0, double hmax, SX* t);
	Lake(int nTimesteps, double surface, double h0, double hmax, double hmin, SX* t);

	void addFlow(  Flow* flow );

	SX getTotalFlow();
	std::list<Flow*>* getFlowList();

	MX getOptHeights(MX V, int t_act);
	double getSurface();

	//Implement abstract functions
	SX getODE();
	virtual std::string getName() ;
	virtual bool isControlled();

//	double getMaxHeight() ;
	virtual SX getHeight(int place);
	void setHeight(int i, SX height);

	std::string print();

	//Side conditions and storage

	virtual void setUp();

	virtual void setPlace(int beginning);

private:

	double surface;

	double hmax;
	double hmin;
	double h0;

//	General properties:
	std::list<Flow*>* flowList;

//	Specific props:
	SX height;


};

#endif /* LAKE_HPP_ */

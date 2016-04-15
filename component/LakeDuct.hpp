/*
 * Duct2.hpp
 *
 *  Created on: Nov 24, 2015
 *      Author: laetus
 */

#ifndef LAKEDUCT_HPP_
#define LAKEDUCT_HPP_

using namespace casadi;

class LakeDuct : public  Flow {
	//The flow is positive, if water flows from A to B
public:

	LakeDuct(int nTimesteps, SX* t, Lake* from, Lake* to, double diff,
				double surface);

	virtual bool isControlled();
	virtual std::string getName();
	void setIsControlled(bool isControlled);

	virtual SX getODE();
//	virtual bool hasODE();
	virtual SX getALG();
	virtual SX getz();
	virtual SX getx();

	virtual void setUp();


protected:

	double heightDiff; //positive iff A is higher
	double surface;
};

#endif /* LAKEDUCT_HPP_ */

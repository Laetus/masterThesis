/*
 * Position.hpp
 *
 *  Created on: Jan 11, 2016
 *      Author: laetus
 */

#include <casadi/casadi.hpp>
#include "Component.hpp"
#include "Velocity.hpp"
#include "Steering.hpp"

#ifndef POSITION_HPP
#define POSITION_HPP
using namespace casadi;

class Position: public Component {

public:

	Position(int nTimesteps, Orientation* steering , double horizon, std::string type, bool isReg, SX* t);
	SX getODE();
	virtual void setUp();
	virtual void setPlace(int beginning);

	Matrix<double> getSafetyDistance();
	MX getSafetyDistance(MX V);
	Matrix<double> getPredecessorPosition(int t_act);

	virtual void setPredecessor_pos(std::string path);

	virtual void storeAndShift_XU(std::map<std::string, Matrix<double> > res_tact, int t_act);
	virtual void setXU_0(Matrix<double> xu_0);
	virtual void prepareRealtime(int new_nTimesteps, int new_horizon);
private:

	void regularCasePositionEstimation(Matrix<double>* positions);
	void worstCasePositionEstimation(Matrix<double>* positions);

	Orientation* orientation;
	double horizon;
	std::string type;

	bool isRegularCaseAssumption;

	Matrix<double> old_pred_pos;
	Matrix<double> predecessor_posStorage;

};
#endif /* POSITION_HPP */


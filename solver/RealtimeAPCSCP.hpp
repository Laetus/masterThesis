/*
 * RealtimeAPCSCP.hpp
 *
 *  Created on: Dec 14, 2015
 *      Author: laetus
 */

#ifndef REALTIMEAPCSCP_HPP_
#define REALTIMEAPCSCP_HPP_

class RealtimeAPCSCP : public Solver {

public :

	RealtimeAPCSCP(Model* model);
	virtual void solve();
	virtual void setChoiceH(int option);
	virtual void setExactA(bool isExact);
	void setNupdateA(int n_updateA);
	void setNupdateH(int n_updateH);

protected :

	std::map<std::string, Matrix<double> > solveConvexSubproblem();

	void updateH_act(int t_act);
	void updateA_act(int t_act);
	void updateDg_act();
	void updateM_act();


	Matrix<double> calcExactLagrangian();
	Matrix<double> calcApproxLagrangian();
	Matrix<double> calcCheapLagrangian();


	Matrix<double> x_act;
	Matrix<double> y_act; // lambda
	Matrix<double> A_act;
	Matrix<double> H_act;
	Matrix<double> Dg_act;
	Matrix<double> m_act;

	MX G_sub;
	MX f_sub;

	MXFunction nlp_sub;
	Function dG_f;
	Function dG_fast;

	int n_updateA;
	int n_updateH;
	bool firstIteration;
	bool exactA;
	int choiceH;
	std::vector< Matrix<double> > assumedData;


	std::vector<Function> hess_gi;
	Function hess_fi;
 };

#endif /* REALTIMEAPCSCP_HPP_ */

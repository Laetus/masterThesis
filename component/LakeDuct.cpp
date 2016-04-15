#include "LakeDuct.hpp"

LakeDuct::LakeDuct(int nTimesteps, casadi::SX *t, Lake* from, Lake* to,
		double diff, double surface) :
	LakeDuct::Flow(nTimesteps, t) {
	this->u = SX::sym("u_lakeduct_NOT_NEEDED_" + std::to_string(this->iD));
	this->x = SX::sym("x_lakeduct_" + std::to_string(this->iD));

	this->controlled = false;
	this->parameter = false;

	this->A = from;
	this->B = to;
	this->heightDiff = diff;
	this->surface = surface;
	this->hasOde = false;
	this->hasAlg = true;
	setUp();
}

void LakeDuct::setUp() {
	this->space_needed = nTimesteps + 1;
	this->space_0 = 1;
	this->xu_min = -Matrix<double>::ones(1, this->space_needed) * surface;
	this->xu_max = Matrix<double>::ones(1, this->space_needed) * surface;

	this->xu_0 = Matrix<double>::zeros(1, this->space_0);
	this->xu_min[0] = this->xu_0;
	this->xu_max[0] = this->xu_0;
	this->xu_init = Matrix<double>::ones(1, this->space_needed) * xu_0;
}

bool LakeDuct::isControlled() {
	return false;
}

std::string LakeDuct::getName() {
	return "Lake_Duct_" + std::to_string(this->iD);
}

void LakeDuct::setIsControlled(bool isControlled) {
	std::cout << " A Lake duct can not be controlled." << std::endl;
	throw 22;
	this->controlled = false;
}

SX LakeDuct::getODE() {
	throw 42;
	return 0;

}

SX LakeDuct::getALG() {
	Lake * lakeA = (Lake*) this->A;
	Lake * lakeB = (Lake*) this->B;
	SX tmp = lakeA->getx() - lakeB->getx() + this->heightDiff;
	//		tmp =  surface * sqrt(2 * Component::GRAV_ACC * abs(tmp) )  * sign(tmp); //not cont. differentiable
	//		tmp = surface * sqrt(2 * Component::GRAV_ACC) * tmp;
	tmp = surface * 50 * ((2 / (1 + exp(-tmp))) - 1);
	return tmp - this->getz();
}

SX LakeDuct::getz() {
	return this->x;
}

SX LakeDuct::getx() {
	return SX();
}

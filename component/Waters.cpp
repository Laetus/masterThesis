/*
 * Water.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: laetus
 */

#include "Waters.hpp"

Waters::Waters() :
	Waters::Component() {
}

Waters::Waters(int nTimesteps, SX* t) :
	Waters::Component(nTimesteps, t) {
}

void Waters::setMaxHeight(double maxHeight) {
	this->maxHeight = maxHeight;
}

/*int Waters::getHeight(int x) {
	std::cout << "Should not be used!!!" << std::endl;
	return -1;
}*/

double Waters::getMaxHeight() {
	return this->maxHeight;
}

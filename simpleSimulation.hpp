/*
 * simpleSimulation.hpp
 *
 *  Created on: Nov 12, 2015
 *      Author: laetus
 */

#ifndef SIMPLESIMULATION_HPP_
#define SIMPLESIMULATION_HPP_


#include <casadi/casadi.hpp>
#include <coin/IpIpoptApplication.hpp>
#include <iostream>
#include <fstream>
#include <limits>
#include <unistd.h>
#include <list>
#include <typeinfo>
#include <omp.h>

#include "component/Component.cpp"
#include "component/Waters.cpp"
#include "component/Lake.cpp"
#include "component/ExtFlow.cpp"
#include "component/Flow.cpp"
#include "component/LakeDuct.cpp"
#include "component/Duct3.cpp"
#include "component/Cflow.cpp"


#include "model/Model.cpp"
#include "model/EflowLakeNnLakeEflow.cpp"
#include "model/EfLnnLDLEf.cpp"
#include "model/LakesAndFlows.cpp"
#include "model/LakesDuctsAndFlows.cpp"

#include "solver/GlobalSolver.cpp"
#include "solver/RealtimeIpopt.cpp"
#include "solver/RealtimeAPCSCP.cpp"


#endif /* SIMPLESIMULATION_HPP_ */

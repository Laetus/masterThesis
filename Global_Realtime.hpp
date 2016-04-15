/*
 * Global_Realtime.hpp
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
#include <boost/tokenizer.hpp>
#include <boost/iterator.hpp>
#include "util/Utils.cpp"
#include <chrono>

#include "component/Component.cpp"
#include "component/Waters.cpp"
#include "component/Lake.cpp"
#include "component/ExtFlow.cpp"
#include "component/Flow.cpp"
#include "component/LakeDuct.cpp"
#include "component/Cflow.cpp"
#include "component/Acceleration.cpp"
#include "component/Orientation.cpp"
#include "component/Position.cpp"
#include "component/Steering.cpp"
#include "component/Velocity.cpp"

#include "model/Model.cpp"
#include "model/HydroModel.cpp"
#include "model/LakesDuctsAndFlows.cpp"
#include "model/LakesDuctAndFlows.cpp"
#include "model/LakesAndFlows.cpp"
#include "model/SimpleLake.cpp"
#include "model/Car.cpp"


#include "solver/Solver.cpp"
#include "solver/GlobalSolver.cpp"
#include "solver/RealtimeIpopt.cpp"
#include "solver/RealtimeAPCSCP.cpp"

#endif /* GLOBAL_REALTIME */

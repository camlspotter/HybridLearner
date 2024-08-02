/*
 * simulation.h
 *
 *  Created on: 05-Oct-2021
 *      Author: amit
 */

#ifndef SIMULATION_SIMULATION_H_
#define SIMULATION_SIMULATION_H_

#include "../utilities/polytope/polytope.h"
#include "../utilities/math/randomGenerator.h"
#include "../utilities/myRandomNumberGenerator.h"

/*
 * Generate random simulation points from a Polytope (Note: polytope should obey the
 * formation rule of a hyberbox. Each dimension should have both the upper and lower bounds. Ax <= b where b is the bound.
 */

std::list< std::vector<double> > getInternalPoints(polytope::ptr &poly, unsigned int n, myRandomNumberGenerator::ptr &randomGenObj);

std::list<std::list<struct control_points>> getInternalControlPoints(polytope::ptr &poly, unsigned int n_simulations, list<struct control_points> list_var_cps, myRandomNumberGenerator::ptr &randomGenObj);

// std::list<std::list<struct control_points>> getInternalControlPoints_PureRandom(polytope::ptr &poly, unsigned int n_simulations, list<struct control_points> list_var_cps);
// 

void fixed_step_signal(double th, std::vector<double> cps, std::vector<double> &time_vector, std::vector<double> &data_vector);
 
void linear_signal(double th, std::vector<double> cps, std::vector<double> &time_vector, std::vector<double> &data_vector);

void spline_signal(std::unique_ptr<MATLABEngine> &ep, double th, std::vector<double> cps, std::vector<double> &time_vector, std::vector<double> &data_vector);

void sine_wave_signal(double timeHorizon, double amplitude, double zero_offset, std::vector<double> &time_vector, std::vector<double> &data_vector);

#endif /* SIMULATION_SIMULATION_H_ */

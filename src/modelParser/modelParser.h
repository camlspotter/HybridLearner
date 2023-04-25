/*
 * modelParser.h
 *
 *  Created on: 22-Sep-2021
 *      Author: amit
 */

#ifndef MODELPARSER_MODELPARSER_H_
#define MODELPARSER_MODELPARSER_H_

#include "../hybridautomata/hybridAutomata.h"
#include "../hybridautomata/location.h"
#include "../hybridautomata/transition.h"
#include "../commandLineParser/user_inputs.h"
#include "../utilities/intermediateResult.h"
#include "../io_functions/summary.h"
#include <boost/algorithm/string.hpp>	//used for trim()

#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <iostream>
#include "boost/timer/timer.hpp"


/*
 * Reads a model file generated by the Learning algorithm and creates an object of hybridAutomata, when the learning algorithm implements One-Versus-One for inferring hyperplane to represent guard
 */
void modelParser_OneVersusOne(hybridAutomata::ptr &H, user_inputs::ptr &user, intermediateResult::ptr &intermediate);

/*
 * Reads a model file generated by the Learning algorithm and creates an object of hybridAutomata, when the learning algorithm implements One-Versus-Rest for inferring hyperplane to represent guard
 */

void modelParser(hybridAutomata::ptr &H, user_inputs::ptr &user, intermediateResult::ptr &intermediate);


/*
 * The above two parser function is now replaced by new approach. Uses connecting point and human annotation for Reset function
 */
void ha_model_parser(hybridAutomata::ptr &H, user_inputs::ptr &user, intermediateResult::ptr &intermediate);


std::list<std::string> correctGuard(std::string line1);

std::list<std::string> modify_guard_forNonDeterminism(std::list<std::string> guard, double epsilon, unsigned int sign);
std::list<std::string> modify_guard(std::list<std::string> gurad, unsigned int less_or_greater);
std::list<std::string> modify_guard_asRange(std::list<std::string> gurad, double epsilon);

std::list<std::string> modify_guard_asHalfspace(std::list<std::string> guard, double epsilon);

list<transition::ptr> getAllTrans(unsigned int src, std::vector<transition::ptr> forward, std::vector<transition::ptr> backward);

list<transition::ptr> getAllTrans(unsigned int src, list<transition::ptr>& all_trans);

/*
 * Function for Automata display. Useful for debugging the modelParser()'s correctness
 */
void verifyHA_output(hybridAutomata::ptr &H);

/*
 * Create input-model file for dReach/dReal Tool for Property Verification
 */
void modelConstruction(hybridAutomata::ptr &H, user_inputs::ptr &user, intermediateResult::ptr &intermediate, summary::ptr &report);

//Test function to read the last word of a file
void readlastWord();


/*
 * Function to convert a string (with & as concatenated string) into a list of string.
 * Useful when input_str="x0>=0.344648094532294 & x0<=60.7002246998343 & x1>=905.325240216932 & x1<=1093.21291463544"
 * Output is a list of constraint expression where the separating delimiter is &
 */
std::list<std::string> string2list(std::string input_str);



#endif /* MODELPARSER_MODELPARSER_H_ */

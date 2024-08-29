/*
 * learnHA_caller.h
 *
 *  Created on: 21-Feb-2023
 *      Author: amit
 */

#include "../commandLineParser/user_inputs.h"
#include "../commandLineParser/parameters.h"

#ifndef LEARNINGHA_LEARNHA_CALLER_H_
#define LEARNINGHA_LEARNHA_CALLER_H_


void learnHA_caller(user_inputs::ptr user_Inputs);

void initial_setting(parameters::ptr &params);

#endif /* LEARNINGHA_LEARNHA_CALLER_H_ */

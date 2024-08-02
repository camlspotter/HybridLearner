/*
 * learn_ha_loop.h
 *
 *  Created on: 22-Feb-2023
 *      Author: amit
 */


#ifndef CORE_ENGINE_LEARN_HA_LEARN_HA_LOOP_H_
#define CORE_ENGINE_LEARN_HA_LEARN_HA_LOOP_H_

#include "../../utilities/matlab.h"

using namespace std;
using namespace matlab::engine;

void execute_learn_ha_loop(parameters::ptr params, summary::ptr &report, std::unique_ptr<MATLABEngine> &ep);

#endif /* CORE_ENGINE_LEARN_HA_LEARN_HA_LOOP_H_ */

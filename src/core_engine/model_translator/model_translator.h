/*
 * model_translator.h
 *
 *  Created on: 21-Feb-2023
 *      Author: amit
 */

#ifndef CORE_ENGINE_MODEL_TRANSLATOR_MODEL_TRANSLATOR_H_
#define CORE_ENGINE_MODEL_TRANSLATOR_MODEL_TRANSLATOR_H_

#include "../../utilities/matlab.h"
#include "../../commandLineParser/parameters.h"

using namespace std;
using namespace matlab::engine;

void model_translator(parameters::ptr params, std::unique_ptr<MATLABEngine> &ep, summary::ptr &report);

#endif /* CORE_ENGINE_MODEL_TRANSLATOR_MODEL_TRANSLATOR_H_ */

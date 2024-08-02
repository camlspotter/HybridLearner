/*
 * commandLineParser.h
 *
 *  Created on: 08-Sep-2021
 *      Author: amit
 */

#ifndef COMMANDLINEPARSER_COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_COMMANDLINEPARSER_H_

#include "user_inputs.h"
#include "../utilities/polytope/polytope.h"

void commandLineParser(int argc, char *argv[], user_inputs::ptr& userInputs);

void mapsVariable_to_Polytope(user_inputs::ptr& user, polytope::ptr& poly);

#endif /* COMMANDLINEPARSER_COMMANDLINEPARSER_H_ */

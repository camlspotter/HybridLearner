/*
 * modelParser.h
 *
 *  Created on: 22-Sep-2021
 *      Author: amit
 */

#ifndef MODELPARSER_MODELPARSER_H_
#define MODELPARSER_MODELPARSER_H_

#include "../hybridautomata/hybridAutomata.h"
#include "../commandLineParser/user_inputs.h"

/*
 * The above two parser function is now replaced by new approach. Uses connecting point and human annotation for Reset function
 */
void ha_model_parser(hybridAutomata::ptr &H, user_inputs::ptr &user
                     // , intermediateResult::ptr &intermediate  (not used)
                     );

#endif /* MODELPARSER_MODELPARSER_H_ */

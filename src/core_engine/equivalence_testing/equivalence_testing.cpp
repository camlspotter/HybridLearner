/*
 * equivalence_testing.cpp
 *
 *  Created on: 22-Feb-2023
 *      Author: amit
 */

#include "equivalence_testing.h"
#include <string>
#include "../../utilities/intermediateResult.h"
#include "../../hybridautomata/hybridAutomata.h"
#include "../../simulinkModelling/simulinkModelConstructor.h"

equivalence_testing::equivalence_testing() {
	// TODO Auto-generated constructor stub

}

equivalence_testing::~equivalence_testing() {
	// TODO Auto-generated destructor stub
}


/* Generates script file for running the simulink model for the given simulink model_filename
 * This function is used in engine:
 * 		equi-test
 *
 */
void equivalence_testing::generate_run_script_file(std::string model_filename, parameters::ptr params) {

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

	simulinkModelConstructor::ptr model = simulinkModelConstructor::ptr(new simulinkModelConstructor(H, userInputs, intermediate));
	// ---------- Create run_script from .slx model
    std::string script_filename = userInputs->getFilenameUnderOutputDirectory("equivalance_test.m"); // XXX constant
    std::string output_filename = userInputs->getFilenameUnderOutputDirectory("equivalence_test_result.txt"); // XXX constant
	model->generateRunModelScript(model_filename, script_filename, output_filename);
	// ------------------------------------------------------------------------
}

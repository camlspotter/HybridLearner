/*
 * learnHA_caller.cpp
 *
 *  Created on: 21-Feb-2023
 *      Author: amit
 */

#include "../commandLineParser/user_inputs.h"
//#include <string>
#include <boost/timer/timer.hpp>
#include <boost/algorithm/string/join.hpp>
#include "../commandLineParser/parameters.h"
#include "../utilities/intermediateResult.h"
#include "../utilities/filesystem.h"
#include "../utilities/filenames.h"

/*
 * This function is called from engine:
 * 		learn-ha
 *		learn-ha-loop
 */

void learnHA_caller(user_inputs::ptr user_Inputs, const fs::path& trace_filename){

	/*
	 * The InputDataFileName it requires the 1st column to be time-serise values and remaining columns hold y-data
	 *
	 * */

	std::string cmd_str;

	std::cout<<"\n***** Now running Python to execute HA Learning Algorithm ******"<<std::endl;

    // XXX Assumes HybridLearner is executed at build/
	cmd_str = "cd ../src/learnHA && pipenv run python3 run.py ";
	cmd_str.append("--input-filename ");

	cmd_str.append(trace_filename); // user_Inputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES_FOR_LEARNING));

	cmd_str.append(" --output-directory ");
    cmd_str.append(user_Inputs->getOutputDirectory());
    cmd_str.append(" ");

	cmd_str.append(" --modes ");
	cmd_str.append(std::to_string(user_Inputs->getModeSize()));

	/* In the learning algorithm, we have
	 * --clustering-method {dtw, dbscan, piecelienar}
	 */
	cmd_str.append(" --clustering-method " + user_Inputs->getClusterAlgorithm());

	cmd_str.append(" --ode-degree ");
	cmd_str.append(std::to_string(user_Inputs->getPolyDegree()));
	//std::cout << user_Inputs->getPolyDegree() <<std::endl;
	cmd_str.append(" --guard-degree ");
	cmd_str.append(std::to_string(user_Inputs->getBoundaryDegree()));
	cmd_str.append(" --segmentation-error-tol ");
	cmd_str.append(std::to_string(user_Inputs->getClusterError()));
	cmd_str.append(" --segmentation-fine-error-tol ");
	cmd_str.append(std::to_string(user_Inputs->getSegmentationFineError()));

	cmd_str.append(" --lmm-step-size ");
	cmd_str.append(std::to_string(user_Inputs->getLmmStepSize()));

	cmd_str.append(" --filter-last-segment ");
	cmd_str.append(user_Inputs->getFilterLastSegment() ? "True" : "False");


	if (user_Inputs->getClusterAlgorithm()=="dbscan") {
		// Two more options to control the parameters of DBSCAN Algorithm
		cmd_str.append(" --dbscan-eps-dist ");
		cmd_str.append(std::to_string(user_Inputs->getDbScanEpsDist()));	// DBSCAN epsilon-distance
		cmd_str.append(" --dbscan-min-samples ");
		cmd_str.append(std::to_string(user_Inputs->getDbScanMinSamples()));		// DBSCAN minimum samples
	} else if (user_Inputs->getClusterAlgorithm()=="dtw") {
		// Two more options to control the parameters of DBSCAN Algorithm
		cmd_str.append(" --threshold-correlation ");
		cmd_str.append(std::to_string(user_Inputs->getCorrelationThreshold()));	// correlation threshold
		cmd_str.append(" --threshold-distance ");
		cmd_str.append(std::to_string(user_Inputs->getDistanceThreshold()));		// distance threshold
	}
	/*cmd_str.append(" ");
	cmd_str.append(std::to_string(user_Inputs->getDTW_SimuTime()));	// internal simulation timeHorizon for DTW clustering algorithm
	cmd_str.append(" ");
	cmd_str.append(std::to_string(user_Inputs->getDTW_SampleSize())); // internal simulation sample-size for DTW clustering algorithm*/

	cmd_str.append(" --input-variables ");
	cmd_str.append(std::to_string(user_Inputs->getListInputVariables().size()));	// total size of input variables
	cmd_str.append(" --output-variables ");
	cmd_str.append(std::to_string(user_Inputs->getListOutputVariables().size())); // total size of output variables

	cmd_str.append(" --input-variables ");
	cmd_str.append("\"" + boost::algorithm::join(user_Inputs->getListInputVariables(), ",") + "\"");
	cmd_str.append(" --output-variables ");
	cmd_str.append("\"" + boost::algorithm::join(user_Inputs->getListOutputVariables(), ",") + "\"");

	// **************

	// Two more options for human-annotations are added-up. --variable-type and --pool-values for variable-type=t2
	cmd_str.append(" --annotations '" + user_Inputs->getAnnotations() + "'");

	cmd_str.append(" --variable-types '");
	cmd_str.append(user_Inputs->getVariableTypes());
	cmd_str.append("' --pool-values '");
	cmd_str.append(user_Inputs->getT2Values());
	cmd_str.append("' --constant-value '");
	cmd_str.append(user_Inputs->getT3Values());

	cmd_str.append("' --ode-speedup ");
	cmd_str.append(std::to_string(user_Inputs->getOdeSpeedup())); // a Maximum integer number/size after which segments are pruned for ODE computation
	cmd_str.append(" --is-invariant ");
	cmd_str.append(user_Inputs->getInvariant() != 2 ? "True" : "False"); // a integer number: 0 and 1 for computing Invariant or 2 for ignoring computation

	std::cout << "Command: "<< cmd_str << std::endl;

	int x1 = system(cmd_str.c_str());
	if (x1 != 0) {
		std::cout <<"Error executing cmd: " << cmd_str <<std::endl;
        throw std::runtime_error("Error executing cmd: " + cmd_str);
	}
}

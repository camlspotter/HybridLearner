/*
 * learnHA_caller.cpp
 *
 *  Created on: 21-Feb-2023
 *      Author: amit
 */

#include "../commandLineParser/user_inputs.h"
//#include <string>
#include <boost/timer/timer.hpp>
#include "../commandLineParser/parameters.h"
#include "../utilities/linux_utilities.h"
#include "../utilities/intermediateResult.h"

/*
 * This function is called from engine:
 * 		learn-ha
 *		learn-ha-loop
 */

void learnHA_caller(user_inputs::ptr user_Inputs){

	/*
	 * The InputDataFileName it requires the 1st column to be time-serise values and remaining columns hold y-data
	 *
	 * */

	std::string cmd_str;

	std::cout<<"\n***** Now running Python to execute HA Learning Algorithm ******"<<std::endl;

    // XXX Assumes HybridLearner is executed at build/
	cmd_str = "cd ../src/learnHA && pipenv run python3 run.py ";
	cmd_str.append("--input-filename ");
    assert( user_Inputs->getInputFilename().is_absolute() );
	cmd_str.append(user_Inputs->getInputFilename());

	cmd_str.append(" --output-directory ");
    cmd_str.append(user_Inputs->getOutputDirectory());
    cmd_str.append(" ");

	cmd_str.append(" --modes ");
	cmd_str.append(std::to_string(user_Inputs->getModeSize()));

	/* In the learning algorithm, we have
	 * -c {1,2,3}, --clustering-method {1,2,3}
                      Clustering Algorithm. Options are: 1: DTW 2: DBSCAN 3: piecelinear
	 */
	//cout << "Clustering Method = " << user_Inputs->getClusterAlgorithm() << endl;
	cmd_str.append(" --clustering-method ");
	if (user_Inputs->getClusterAlgorithm()=="dtw") {
		cmd_str.append("1");
	} else if (user_Inputs->getClusterAlgorithm()=="dbscan") {
		cmd_str.append("2");
	}/* else if (user_Inputs->getClusterAlgorithm()=="piecelinear") {
		cmd_str.append("3");
	} */

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
	cmd_str.append(std::to_string(user_Inputs->getFilterLastSegment()));


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


	//******* Passing the total size of input and output variables in the simulation traces
	cmd_str.append(" --size-input-variable ");
	cmd_str.append(std::to_string(user_Inputs->getListInputVariables().size()));	// total size of input variables
	cmd_str.append(" --size-output-variable ");
	cmd_str.append(std::to_string(user_Inputs->getListOutputVariables().size())); // total size of output variables
	// **************

	// Two more options for human-annotations are added-up. --variable-type and --pool-values for variable-type=t2
	cmd_str.append(" --variable-types '");
	cmd_str.append(user_Inputs->getVariableTypes());
	cmd_str.append("' --pool-values '");
	cmd_str.append(user_Inputs->getT2Values());
	cmd_str.append("' --constant-value '");
	cmd_str.append(user_Inputs->getT3Values());

	cmd_str.append("' --ode-speedup ");
	cmd_str.append(std::to_string(user_Inputs->getOdeSpeedup())); // a Maximum integer number/size after which segments are pruned for ODE computation
	cmd_str.append(" --is-invariant ");
	cmd_str.append(std::to_string(user_Inputs->getInvariant())); // a integer number: 0 and 1 for computing Invariant or 2 for ignoring computation

	std::cout << "Command: "<< cmd_str << std::endl;

	int x1 = system(cmd_str.c_str());
	if (x1 != 0) {
		std::cout <<"Error executing cmd: " << cmd_str <<std::endl;
        throw std::runtime_error("Error executing cmd: " + cmd_str);
	}
}



/*
 * This function is invoked from engine:
 * 		learn-ha

 * To call learning algorithm implemented in Python we copy the input file(simulation-trace)
 * inside the folder "src/learnHA/data" since at the moment Python project is reading file only within its location (current folder)
 */
void initial_setting(parameters::ptr &params){

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();


	string commandStr ="cp ";
	//Todo: in case user input absolute path with filename extract the filename for further process which may be required
	commandStr.append(userInputs->getSimuTraceFilename()); //absolute path of Release folder from where the project is executed
	commandStr.append(" ");
	//commandStr.append(intermediate->getLearnAlgoDefaultInputfilePath()); //absolute path
	//commandStr.append(" ../src/pwa/naijun"); //relative path from the folder Release ......> OLD implementation
	commandStr.append(" ../src/learnHA/data"); //-------> NEW implementation

//	cout << "commandStr = "<<commandStr<<endl;
	//cout <<"Naijun's Inputfile path: "<< intermediate->getLearnAlgoDefaultInputfilePath() << endl;

	//system("cp finalFile.txt ../src/pwa/naijun"); //This is temporary fix as the Learning algorithm required
	int x = system(commandStr.c_str());
	if (x == -1) {
		std::cout <<"Error executing cmd: " << commandStr <<std::endl;
	}

	// ---------- few Path setting for execution to create the .slx model

	std::string trace_file_user = userInputs->getSimuTraceFilename();	//user can supply: just file name or absolute path from Release folder
	std::string fileName="", filePath="", userPath="";
	std::string only_filename_user;
	std::string original_trace_file_path="", key="/";
	std::size_t found = trace_file_user.rfind(key);	// locate the last "/" character
	if (found!=std::string::npos) {

		fileName = trace_file_user.substr(found+1);		// is "bball.slx"
		unsigned int tot_len = trace_file_user.length(), file_len = fileName.length();
		file_len += 1; //to exclude the last '/' character in the path
		userPath = trace_file_user.substr(0, tot_len - file_len);	// is "../src/test_cases/engine/learn_ha/"

		//std::cout <<"file Name=" << fileName <<"   path="<< filePath << std::endl;

		filePath.append(getcwd());  //Release or Debug
		filePath.append("/");
		filePath.append(userPath);	// this include the last "/". NOW '/' is EXCLUDED

		//std::cout <<"file Name length=" << file_len <<"   path length="<< tot_len << std::endl;

//		intermediate->setMatlabPathForOriginalModel(filePath);


	} else {	//no path is supplied. Only fileName is supplied by the user
		fileName = trace_file_user;
	}

    cout << "simuTraceFilename " << userInputs->getSimuTraceFilename() << endl;
    cout << "fileName " << fileName << endl;
    
	string simuFileNameWithPath = "data/";
	simuFileNameWithPath.append(fileName);

	userInputs->setInputFilename(simuFileNameWithPath);//Now modify the inputfilename since a hard-coded path is specified under folder "naijun/". Now "data/"
	//***************** End of Step 2 ******************
	//cout << "Done 1" << endl;
	params->setUserInputs(userInputs);
	//params->setIntermediate(intermediate); // --------> NEW modification

}

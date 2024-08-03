/*
 * learn_ha_loop.cpp
 *
 *  Created on: 22-Feb-2023
 *      Author: amit
 */

#include <iostream>
#include <list>
#include <vector>
#include "model_setup.h"
#include <boost/timer/timer.hpp>
#include "../../modelParser/modelParser.h"
#include "../../commandLineParser/parameters.h"
#include "../../io_functions/summary.h"
#include "../../utilities/linux_utilities.h"
#include "../../utilities/intermediateResult.h"
#include "../../simulinkModelling/simulinkModelConstructor.h"
#include "../simulation/simulation_utilities.h"
#include "../../io_functions/data_structs.h"
#include "../../learningHA/learnHA_caller.h"
#include "../equivalence_testing/equivalence_test_utils.h"
#include "../../utilities/system.h"
#include "../../utilities/matlab.h"
#include "../../utilities/string_operations.h"

void initial_processing_for_InputFileName(parameters::ptr params);
void generate_initial_traces_for_learn_ha_loop(std::list<struct timeseries_all_var> &list_input_variable_values,
		std::list<std::vector<double> > &list_output_variable_values, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep, summary::ptr &report);
void initial_setup_for_learning(parameters::ptr params);
void updateTraceFile(// unsigned int iteration, (unused)
                     // std::vector<double> CE, (unused)
                     // list<struct timeseries_input> CE_trace, (unused)
                     parameters::ptr params);
void call_LearnHA(parameters::ptr params, summary::ptr &report);
void constructModel(unsigned int count, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep);
bool equivalenceTesting_for_learn_ha_loop(unsigned int iteration, model_setup::ptr la_setup, std::list<struct timeseries_all_var> &list_inputs,
		std::list<std::vector<double> > &list_outputs, std::vector<double> &CE_output_var, std::list<struct timeseries_input> &CE_input_var, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep);
void generate_simulation_traces_original_model_to_learn(std::list<struct timeseries_all_var> &initial_simulation_timeSeriesData,
		std::list<std::vector<double> > &initial_output_values, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep, summary::ptr &report);

/*
 * Learns an Automaton HA, given a simulink model M, by simulating M and learning M' in a refinement loop based on equivalence testing of M' and M.
 *
 */
void execute_learn_ha_loop(parameters::ptr params, summary::ptr &report, std::unique_ptr<MATLABEngine> &ep){

    // Step 1: Take input a Simulink Model M, which is the system to Learn: user may input absolute or relative and/or partial path
	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

	/*
	 * Step 3: Using the traces Learn an HA and create a model file M' (Abstract). This model at present we name as x0, x1, x2 and so on with the sequence input followed by output variables.
	 * Step 4: Using M' create a Simulink Model M' (having input and output variables like M) as per the user's input specification BUT DIFFERENT VARIABLE NAMES
	 * Step 5: Generate random input and perform simulation of M and M' and test for Equivalence
	 * 		 : If M == M' for all random inputs then STOP we have learned the model M
	 * 		 : Otherwise, use the counter trace to improve the learning algorithm by Looping from Step 3 - Step 5
	 *
	 * Note: path for original and learn model can be/is stored in the class intermediate.
	 */

	std::list<struct timeseries_all_var>  list_inputs;
	std::list<std::vector<double> > list_outputs;

	initial_processing_for_InputFileName(params);

    // Step 2: Generate simulation traces based on user's inputs.
    // User will provide variable name present in the original model M and not in M'
    // 
	// Traces generated from the Original model (after creating running-script)
    //
    // Following files are created:
    //   run_script_simu_user_model.m
    //   simu.txt
    //   slprj/*
    //   oscillator.slxc
    //   (result_simu_data.txt)
    //   (tmp_simu.txt)
	generate_initial_traces_for_learn_ha_loop(list_inputs, list_outputs, params, ep, report);

	//Note: due to issue in unique_prt copy, we have also generated random-inputs for equivalence-testing steps here having variable names as original-model's names
	initial_setup_for_learning(params);

	//generate_random_inputs_for_equivalence_testing(list_inputs, list_outputs);	//we may pass seed here if required later

	bool flag=false, loop=true;
	unsigned int tot_count=0;

	//model_setup::ptr la_setup = model_setup::ptr(new model_setup(userInputs->getVariableCategory(), userInputs->getInputSignalType(), userInputs->getNumberOfControlPoints(),
	//userInputs->getInitialSet_userInput()));
	model_setup::ptr la_setup = model_setup::ptr(new model_setup(H, userInputs)); //creates a copy of H and userInputs

	std::vector<double> CE_output_var;
	std::list<struct timeseries_input> CE_input_var;

	while (loop) { //Infinite Loop: stops when StopTime-limit exceeds Conclusion could not be drawn concretely

		//Efficient Learning Loop
		if (tot_count != 0) { //skip on the initial iteration
			updateTraceFile(/* tot_count, CE_output_var, CE_input_var, */ params);	//adds new time-serise data to the initial trace-file
		}

        // Step 3: Using the traces Learn an HA and create a model file M' (Abstract).
        // This model at present we name as x0, x1, x2 and so on with the sequence input followed by output variables.
        //
        // learnHA generates the following files:
        //   data_scale
        //   learnHA_out.txt
        //   svm_model_file
		call_LearnHA(params, report);

		boost::timer::cpu_timer timer;
		timer.start();

		ha_model_parser(H, userInputs); //locations (invariant and ode) followed by transitions (guard and reset)

        double wall_clock_modelParser=0.0, running_time=0.0;
		timer.stop();
		wall_clock_modelParser = timer.elapsed().wall / 1000000; //convert nanoseconds to milliseconds
		running_time = wall_clock_modelParser / (double) 1000;	//convert milliseconds to seconds
		report->setRuntimeModelParsing(running_time);

		//verifyHA_output(H);
		/*
		 * Need to take care of the variable names. Here original model and learned model have different names.
		 * For eg, learned model have names as x0, x1 and so on. Where as original user-supplied model may have names as "u, x , v" where u:input and x:output, v:output.
		 * Note: since the inputs to the learning algorithm has the format {[time,input,output]}, so the variables are in the order input-variables followed by output-variables
		 * 		 We also know the size of input and output variables from user's input (along with the original model).
		 * 		 Having redundant code may be easier then trying to reuse codes/functions. No no found a simple way of only modifying the object ha and userInputs to learned variables
		 * 		 or original variables depending on the need.
		 */

//		cout <<"before calling setup_for_learned_model(): Value is " << userInputs->getFixedIntervalData() << endl;
		//Now create a simulink model (txtslx) for equivalence testing. But run the model_setup_for_learned_model before construction.
		// This setup should modify ha and user class to contain details with variable names.
		la_setup->setup_for_learned_model(H, userInputs);	//This should modify the objects with new variables names

        // This generates the following files:
        //   run_script0.m
        //   simulink_model0.slx
        //   generateSimulinkModel0.m
        //creating .slx and .m (running script) files, taking care of the variable-names for learned model in the presence of original model
		constructModel(tot_count, params, ep);	

		//Execute in loop the learned_model and then the original_model and test the equivalence-distance
		struct CE_list CEs;
		int for_paper = 1;
		if (for_paper == 0)
			flag = equivalenceTesting_for_learn_ha_loop(tot_count, la_setup, list_inputs, list_outputs, CE_output_var, CE_input_var, params, ep); //these list_inputs and list_outputs are generated using original_model's variable-names
		else
			flag = true;	//We do not want to perform equivalence check for this paper we present only a Passive learning.

		if (flag == true) { //could not find a CE, so equivalent. Therefore NO improvement in learning required.
			break;
		} else {	//found a counter_example init_point for the LOOP
			// just store or use this counterexample CE_output_var, CE_input_var
			// the last executed output result can be updated for learning
			// all_CEs.push_back(CEs);
		}

		tot_count++;
		double stopTime = report->computeTotalTimeElapsed();
		cout <<"Computed Total Elapse Time = " << stopTime <<"     userInputs->getMaxStoptime()=" << userInputs->getMaxStoptime() << endl;
		if (stopTime >= userInputs->getMaxStoptime())
			loop = false;	//Time to terminate the loop

	} //End of Loop

    //	report->printSummary();
}


//Returns only the fileName and stores the path in intermediate->getMatlabPathForOriginalModel()
void initial_processing_for_InputFileName(parameters::ptr params) {

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();

	// ---------- few Path setting for execution to create the .slx model
    // MatlabDefault=`pwd`
	intermediate->setMatlabDefaultPath(getcwd());

	// ------------------------------------------------------------------------

    // LearnAlgoDefaultInputfile = HybridLearner/src/learnHA/data
	std::string learnAlgo_inputfile_Path = dirname(getcwd());
	learnAlgo_inputfile_Path.append("/src/learnHA/data");
	intermediate->setLearnAlgoDefaultInputfilePath(learnAlgo_inputfile_Path);

    // model_file_M=../src/test_cases/engine/learn_ha_loop/oscillator.slx
	std::string model_file_M = userInputs->getSimulinkModelFilename();

	std::string filePath = dirname(model_file_M);
    std::string fileName = basename(model_file_M);

	intermediate->setMatlabPathForOriginalModel(filePath);

	intermediate->setMatlabPathForLearnedModel(getcwd());
}


//Generate file containing simulation traces of the original simulink model
void generate_initial_traces_for_learn_ha_loop(std::list<struct timeseries_all_var> &list_input_variable_values,
		std::list<std::vector<double> > &list_output_variable_values, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep, summary::ptr &report) {

	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

	std::list<std::string> inputVarList =  userInputs->getListInputVariables();
	std::string variableName;
	unsigned int index=0;
	for (std::list<std::string>::iterator it = inputVarList.begin(); it != inputVarList.end(); it++) {
		variableName = (*it);
		H->insert_to_map(variableName, index);
		index++;
	}
	std::list<std::string> outputVarList =  userInputs->getListOutputVariables();
	//ind=0; should continue
	for (std::list<std::string>::iterator it = outputVarList.begin(); it != outputVarList.end(); it++) {
		//std::cout << "VarName=" << (*it) <<"  index=" << ind << std::endl;
		variableName = (*it);
		H->insert_to_map(variableName, index);
		index++;
	}

	// --------------

	std::list<std::vector<double> > initial_output_values;
	std::list<struct timeseries_all_var> initial_inputValues_timeSeriesData;
	std::list<std::vector<double> > initial_simu_output_values;
	std::list<struct timeseries_all_var> initial_simu_inputValues_timeSeriesData;

	generate_input_information(initial_inputValues_timeSeriesData, initial_output_values, params, ep, report);	//initial values: This function has the random-generation SEED

//	std::cout <<"10 this is done" <<std::endl;
	// Note: inputs are generated for max(initial-simu-size, max-traces). But for initial traces only use initial-simu-size
	std::list<std::vector<double> >::iterator it_out = initial_output_values.begin();
	std::list<struct timeseries_all_var>::iterator it_in = initial_inputValues_timeSeriesData.begin();
	for (unsigned int i=0; i < userInputs->getSimuInitSize(); i++) {	//copying only the first few inputs for generating initial traces
		initial_simu_output_values.push_back((*it_out));
		initial_simu_inputValues_timeSeriesData.push_back((*it_in));

		it_out++;	it_in++;
	}

	// Following files are created:
    // run_script_simu_user_model.m
    // simu.txt
    // slprj/
    // oscillator.slxc
    // result_simu_data.txt
    // tmp_simu.txt
    generate_simulation_traces_original_model_to_learn(initial_simu_inputValues_timeSeriesData, initial_simu_output_values, params, ep, report);

	std::list<std::vector<double> > initial_output_for_equivalence;
	std::list<struct timeseries_all_var> initial_input_timeseries_for_equivalence;
	it_in = initial_inputValues_timeSeriesData.begin();
	it_out = initial_output_values.begin();
	for (unsigned int i=0; i < userInputs->getMaxTraces(); i++) {	//copying only the first few inputs for generating initial traces for Equivalence Testing
		initial_output_for_equivalence.push_back((*it_out));
		initial_input_timeseries_for_equivalence.push_back((*it_in));
		it_out++;	it_in++;
	}

	list_input_variable_values = initial_input_timeseries_for_equivalence;
	list_output_variable_values = initial_output_for_equivalence;
}


void initial_setup_for_learning(parameters::ptr params) {
	user_inputs::ptr userInputs = params->getUserInputs();
	userInputs->setInputFilename(userInputs->getSimulationFilename());
}


//Generate trace from Original Model is appended
void updateTraceFile(//unsigned int iteration, (unused)
                     // std::vector<double> CE, (unused)
                     // list<struct timeseries_input> CE_trace, (unused)
                     parameters::ptr params) {

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();
    //	hybridAutomata::ptr H = params->getH();

	//adds new time-serise data to the initial trace-file. In fact we do not need to simulate again just append the result-trace-file
	cout <<"Now going to Simulate the Original Model to update the Simulation-Trace-File" << endl;

	// ***** Trace file result.txt generated by Simulink *************
	// *** Now append the trace into the simulation_model.txt file
	string previous_SimulationTraceFile = userInputs->getSimulationFilename();
	cout << "previous_SimulationTraceFile = "<< previous_SimulationTraceFile << endl;
	//Now get result-file name from the original-running-path

	std::string resultFileName = userInputs->getFilenameUnderOutputDirectory("result_simu_data.txt");
	cout << "original Simulation TraceFile = "<< resultFileName << endl;

	std::string tmpSimuFile = userInputs->getFilenameUnderOutputDirectory("update_tmp_simulation.txt");

	/*
	 * cat  simuFileName  resultFile  >  tmpSimuFile   //from 2nd iteration onwards
	 * cp tmpSimuFile  simuFileName
	 */
	std::string cmd="cat ";
	cmd.append(previous_SimulationTraceFile);
	cmd.append(" ");
	cmd.append(resultFileName);
	cmd.append(" > ");
	cmd.append(tmpSimuFile);
	//cout <<"Iteration "<< matlab_execution_count <<"  Cmd: " << cmd <<endl;
	system_must_succeed(cmd);

	cmd="cp ";
	cmd.append(tmpSimuFile);
	cmd.append(" ");
	cmd.append(previous_SimulationTraceFile);
	cout << "  Cmd: " << cmd <<endl;
	system_must_succeed(cmd);

	//Now copy the simulation-Trace file in the Learning Algorithm's folder
	string commandStr ="cp ";
	commandStr.append(intermediate->getMatlabDefaultPath()); //absolute path of Release folder from where the project is executed
	//commandStr.append(user_Inputs->getInputFilename());
	commandStr.append("/");
	commandStr.append(userInputs->getSimulationFilename());
	commandStr.append(" ");
	commandStr.append(intermediate->getLearnAlgoDefaultInputfilePath()); //absolute path

	system_must_succeed(commandStr);
}

/*
 * This function is used in the engine:
 * 		learn-ha-loop
 */
void call_LearnHA(parameters::ptr params, summary::ptr &report) {

	user_inputs::ptr userInputs = params->getUserInputs();
//	hybridAutomata::ptr H = params->getH();

	//This function is called from the engine "learn-ha-loop"
	//std::cout << "Running Engine: Learning Hybrid Automaton  ... \n";

	boost::timer::cpu_timer timer;
	timer.start();
	learnHA_caller(userInputs);	//Make is Simple and call it from everywhere. This invokes our "HA learning Algorithm".
	timer.stop();
	double wall_clock;
	wall_clock = timer.elapsed().wall / 1000000; //convert nanoseconds to milliseconds
	double running_time = wall_clock / (double) 1000;	//convert milliseconds to seconds
	//std::cout << "\n\n*******Learning Nonlinear Switched Dynamical Systems (specific Hybrid Automata)****\n \t Running Time (Boost/Wall clock time) (in Seconds) = " << running_time<<std::endl;
//	std::cout << "\nRunning Time (Boost/Wall clock time) (in Seconds) = " << running_time<<std::endl;
	report->setRuntimeLearningAlgo(running_time);
}


//Constructing system of files for the Learned Model (having variable names as x0, x1 and so on)
void constructModel(unsigned int count, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep) {

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

	//This fixing for userInputs: sys_dimension is a hard to remember fix. Better fixing can be applied later.
	H->setDimension(H->map_size());
	userInputs->setSysDimension(H->getDimension());
	//  ------------

	simulinkModelConstructor::ptr model = simulinkModelConstructor::ptr(new simulinkModelConstructor(H, userInputs, intermediate));
	model->setIteration(count);
	model->printSimulinkModelFile();
//	cout << "Done Creating the script file for creating Simulink Model!!!" << endl;

	// ---------- few Path setting for execution to create the .slx model
	std::string learned_path = userInputs->getOutputDirectory();
	intermediate->setMatlabPathForLearnedModel(learned_path);

//	std::cout <<"Creating Simulink model file with extension .slx .... please wait..." << std::endl;
	model->executeSimulinkModelConstructor(ep);
//	std::cout << "Model .slx file created successfully!!" << std::endl;

	//running script file generated for learned model with just the input variable name
	// ----------------------run script generator-----------------------------

    std::string simulink_model_filename = userInputs->getFilenameUnderOutputDirectory("simulink_model" + to_string(count) + ".txt");
    std::string script_filename = userInputs->getFilenameUnderOutputDirectory("run_script" + to_string(count) + ".m");
    std::string output_filename = userInputs->getFilenameUnderOutputDirectory("output" + to_string(count) + ".txt");
	model->create_runScript_for_learn_ha_loop_engine(simulink_model_filename, script_filename, output_filename);

	// ----------------------------------------------------------------------------
}

//Returns True: when models are equivalent, otherwise False.
//Note: These list_inputs and list_outputs are generated using original_model's variable-names, needed to handle learned-variable-names
bool equivalenceTesting_for_learn_ha_loop(unsigned int iteration, model_setup::ptr la_setup, std::list<struct timeseries_all_var> &list_inputs,
		std::list<std::vector<double> > &list_outputs, std::vector<double> &CE_output_var, std::list<struct timeseries_input> &CE_input_var, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep) {

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();


	/*
	 * Note:
	 * 1) User supplies only a single simulink model file as black-box system input
	 * 2) the learned simulink model will have name supplied by the option --out-file for eg., out.txt by default and so model's name is out0.slx, out1.slx and so on for different iterations
	 * 3) Here list_inputs and list_outputs have variable-names as original-names
	 */


	std::string model_file_two;
    {
        std::string model_filename = "model";
        model_filename.append(to_string(iteration));	// iteration number appended
        model_filename.append(".txt");
        model_filename = userInputs->getFilenameUnderOutputDirectory(model_filename);
        model_file_two = model_filename;	//Learned model is two
    }
    
	std::string model_file_one = userInputs->getSimulinkModelFilename();	//Original model is ONE

	std::string script_filename_one = "run_script_simu_user_model.m";
    std::string output_filename_one="result_simu_data.txt";

    // script_filename_two= run_script_xxx0m
	std::string script_filename_two = userInputs->getFilenameUnderOutputDirectory("eqtest_run_script");
	script_filename_two.append(to_string(iteration));
	script_filename_two.append(".m");		//.m extension appended

    // output_filename_two= result_xxx0.txt
    std::string output_filename_two = userInputs->getFilenameUnderOutputDirectory("eqresult");
	output_filename_two.append(to_string(iteration));
	output_filename_two.append(".txt");

//-----------------------------------------------------------------------------------------------------

	bool flagEquivalent=true;
	double maxDistance=0.0;
	std::list<std::vector<double> >::reverse_iterator it_out_val = list_outputs.rbegin(); //iterator for the output variables
	for (std::list<struct timeseries_all_var>::reverse_iterator it =list_inputs.rbegin(); it != list_inputs.rend(); it++, it_out_val++) {

		std::list<struct timeseries_input> init_point = (*it).timeseries_signal;
		std::vector<double> output_variable_init_values = (*it_out_val);

		// ***************  --------------------------------- ***************
		//userInputs and H are in the learned model setup so first execute Learned model. But variables are in original-names
		cout <<"********* Running Learned model ***************** "<< endl;
		la_setup->setup_for_learned_model(H, userInputs);
		simulate_learned_model_from_learn_ha_loop(ep, userInputs, init_point, output_variable_init_values, script_filename_two, output_filename_two, intermediate, H, la_setup); //Populate initial data in Matlab's Workspace and then run the script file

		cout <<"********* Running Original model ***************** "<< endl;
		la_setup->setup_for_original_model(H, userInputs); //setting up to run the original model
		simulate_original_model_from_learn_ha_loop(ep, userInputs, init_point, output_variable_init_values, script_filename_one, output_filename_one, intermediate, H, la_setup); //Populate initial data in Matlab's Workspace and then run the script file

		// ***************  --------------------------------- ***************

		// -------------- Read the two files and compare --------------
		string file_original_with_path="", file_learned_with_path="";
		file_original_with_path = intermediate->getMatlabPathForOriginalModel();
		file_original_with_path.append("/");
		file_original_with_path.append(output_filename_one);
		file_learned_with_path = intermediate->getMatlabPathForLearnedModel();
		file_learned_with_path.append("/");
		file_learned_with_path.append(output_filename_two);
		//cout <<"file_original_with_path = " << file_original_with_path <<"    file_learned_with_path = " <<file_learned_with_path << endl;

		//flagEquivalent = equivalence_testing(file_original_with_path, file_learned_with_path, maxDistance); //interface modified
		flagEquivalent = compute_trace_equivalence(file_original_with_path, file_learned_with_path, maxDistance, params);

//		cout <<"Maximum Euclidean Distance=" << maxDistance << endl;
//		cout <<"Maximum Average Absolute difference between traces=" << maxDistance << endl;

		// -------------- --------------

		if (flagEquivalent==false)	{ //found a counter-example
			CE_output_var = output_variable_init_values;
			CE_input_var = init_point;
			break;
		}
	}

	if (flagEquivalent==true) {
		std::cout << "The two Simulink models are Equivalent!!!" << std::endl;
	} else {
		std::cout << "The two Simulink models are NOT Equivalent!!!" << std::endl;
	}

	return flagEquivalent;
}



//runs multiple simulation and creates a trace file. This function is called from "learn-ha" engine.
void generate_simulation_traces_original_model_to_learn(std::list<struct timeseries_all_var> &initial_simulation_timeSeriesData,
		std::list<std::vector<double> > &initial_output_values, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep, summary::ptr &report) {

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

	simulinkModelConstructor::ptr model = simulinkModelConstructor::ptr(new simulinkModelConstructor(H, userInputs, intermediate));
	unsigned int iterations_count = 0; // just a label for the first model creation
	model->setIteration(iterations_count);	//this may not be required now

	// We assume file generated is "run_script_simu_user_model.m"
	// Now creating script file for later running the simulation and obtaining output-file having fixed time-step
	// Creating a script file for Running the simulink model just created.
	std::string script_filename = userInputs->getFilenameUnderOutputDirectory("run_script_simu_user_model.m");
    std::string output_filename = userInputs->getFilenameUnderOutputDirectory("result_simu_data.txt");
	std::string simulink_model_filename = userInputs->getSimulinkModelFilename();

    // It builds script_filename="$OUTDIR/run_script_simu_user_model.m"
	model->create_runScript_for_simu_engine(simulink_model_filename, script_filename, output_filename);

	// *************** setting up the mergedFile(s) ***************
	std::string simuFileName = userInputs->getFilenameUnderOutputDirectory("simu.txt");
	std::string tmpSimuFile = userInputs->getFilenameUnderOutputDirectory("tmp_simu.txt");
	userInputs->setSimulationFilename(simuFileName);

    // rm $simuFileName $tmpSimuFile
	{
        std::string deleteCommand = "rm ";
        deleteCommand.append(simuFileName);
        deleteCommand.append(" ");
        deleteCommand.append(tmpSimuFile);
        int x = system(deleteCommand.c_str());	//Deleting simu_modelFile.txt and tmp_simu_modelFile.txt. Todo: check if exists before delete
        if (x == -1) {
            std::cout <<"Error executing cmd: " << deleteCommand <<std::endl;
        }
    }
	// *************** mergedFile deleted if exists ***************

	unsigned int matlab_execution_count=0;
	/*
	//std::ofstream finalFile("finalFile.txt",  std::ios_base::binary | std::ios_base::app);
	std::ofstream finalFile(simuFileName,  std::ios_base::binary | std::ios_base::app);

	 for(std::list < std::vector<double> >::iterator it=list_initial_points.begin(); it !=list_initial_points.end(); it++) {
		std::vector<double> init_point = (*it);


		simulate(ep, user, init_point, intermediate); //this will generate the file "result.tsv" for this init_point

		matlab_execution_count++;
		user->setNumberMatlabSimulationExecuted(matlab_execution_count);

		std::string resultFileName = getSimulationOutputFileName(user, intermediate);

		std::ifstream file_a (resultFileName, std::ios_base::binary);

		finalFile.seekp(0, std::ios_base::end);	//seek the record point to end_of_file
		finalFile << file_a.rdbuf();	//append the file_a in the previous file

	}*/
	/*
	 * The above approach gave error while running in our Google Cloud setup but did not show any issues in my Personal Desktop
	 *
	 * So, trying to use simple system command to concate two files into a third file. This will avoid the use of stream operation of C++
	 * ************ cat /home/amit/3dplot.plt /home/amit/amit.java > /home/amit/raj.txt	 ************************
	 */

	//simulation_trace_testing::ptr simu_test = simulation_trace_testing::ptr(new simulation_trace_testing());;

	boost::timer::cpu_timer matlab_simulation;
	matlab_simulation.start();

	unsigned int counting=1;

	//std::cout <<"initial_simulation_timeSeriesData.size()=" << initial_simulation_timeSeriesData.size() << std::endl;
	std::list<std::vector<double> >::iterator it_out_val = initial_output_values.begin(); //iterator for the output variables
	for (std::list<struct timeseries_all_var>::iterator it =initial_simulation_timeSeriesData.begin(); it != initial_simulation_timeSeriesData.end(); it++, it_out_val++) {

//		cout <<"counting = " << counting <<endl;
		std::list<struct timeseries_input> init_point = (*it).timeseries_signal;

		std::vector<double> output_variable_init_values = (*it_out_val);

		//if (counting >= 31) { //because upto 32 I have already generated tracefiles

		// ***************  --------------------------------- ***************

        // This builds the following files:
        // slprj/
        // result_simu_data.txt
		simu_model_file(ep, userInputs, init_point, output_variable_init_values, script_filename, output_filename, intermediate, H); //Populate initial data in Matlab's Workspace and then run the script file

		// ***************  --------------------------------- ***************

/*
		// **************** Code to  generating simulation-trace file for comparison with tool POSEHAD and also to generate traces used for Testing ***********
		// Temporary Code for generating simulation-trace file for comparison with tool POSEHAD
		string copy_cmd="cp ", benchmarkName="outputData/bball_";  //benchmarkName="outputData/bball_"; // benchmarkName="outputData/excitableCells_";
		benchmarkName.append(std::to_string(counting));
		benchmarkName.append(".csv");
		copy_cmd.append(resultFileName);
		copy_cmd.append(" ");
		copy_cmd.append(benchmarkName);
		x = system(copy_cmd.c_str());

		// Can also disable the cat section of the code to speedup

		// ***********************************
*/

		//cout << "Absolute path of the simulation generated output file: " << resultFileName << endl;
		if (matlab_execution_count==0){	//1st iteration
            // cat $output_filename > $tmpSimuFile
            std::string cmd;
			cmd="cat ";
			cmd.append(output_filename);
			cmd.append(" > ");
			cmd.append(tmpSimuFile);
			//cout <<"Iteration "<< matlab_execution_count <<"  Cmd: " << cmd <<endl;
			int x = system(cmd.c_str());
			if (x == -1) {
				std::cout <<"Error executing cmd: " << cmd <<std::endl;
			}
		} else {	//2nd iterations onwards
            // cat $simuFileName $output_filename > $tmpSimuFile
            std::string cmd;
			cmd="cat ";
			cmd.append(simuFileName);
			cmd.append(" ");
			cmd.append(output_filename);
			cmd.append(" > ");
			cmd.append(tmpSimuFile);
			//cout <<"Iteration "<< matlab_execution_count <<"  Cmd: " << cmd <<endl;
			int x = system(cmd.c_str());
			if (x == -1) {
				std::cout <<"Error executing cmd: " << cmd <<std::endl;
			}
		}

        // cp $tmpSimuFile $simuFileName
        {
            std::string cmd;
            cmd="cp ";
            cmd.append(tmpSimuFile);
            cmd.append(" ");
            cmd.append(simuFileName);
            //cout << "  Cmd: " << cmd <<endl;
            int x = system(cmd.c_str());
            if (x == -1) {
                std::cout <<"Error executing cmd: " << cmd <<std::endl;
            }
        }

		matlab_execution_count++;
		userInputs->setNumberMatlabSimulationExecuted(matlab_execution_count);
		counting++;
	} //End of all simulation traces

	matlab_simulation.stop();

	double wall_clock;
	wall_clock = matlab_simulation.elapsed().wall / 1000000; //convert nanoseconds to milliseconds
	double running_time = wall_clock / (double) 1000;	//convert milliseconds to seconds
//	std::cout << "Matlab Simulation and Trace File generation: Running Time (in Seconds) = " << running_time << std::endl;
	report->setRuntimeMatlabInitialSimulation(running_time);
}



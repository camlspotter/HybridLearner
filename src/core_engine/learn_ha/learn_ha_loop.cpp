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
#include "../../utilities/intermediateResult.h"
#include "../../simulinkModelling/simulinkModelConstructor.h"
#include "../simulation/simulation_utilities.h"
#include "../../io_functions/data_structs.h"
#include "../../learningHA/learnHA_caller.h"
#include "../equivalence_testing/equivalence_test_utils.h"
#include "../../utilities/system.h"
#include "../../utilities/matlab.h"
#include "../../utilities/filesystem.h"
#include "../../utilities/filenames.h"
#include "../../utilities/misc.h"

void initial_processing_for_InputFileName(parameters::ptr params);
void generate_initial_traces_for_learn_ha_loop(std::list<struct timeseries_all_var> &list_input_variable_values,
		std::list<std::vector<double> > &list_output_variable_values, parameters::ptr params, std::unique_ptr<MATLABEngine> &ep, summary::ptr &report);
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

    cout << "execute_learn_ha_loop" << endl;

    // Step 1: Take input a Simulink Model M, which is the system to Learn: user may input absolute or relative and/or partial path
	user_inputs::ptr userInputs = params->getUserInputs();
    intermediateResult::ptr intermediate = params->getIntermediate();
	hybridAutomata::ptr H = params->getH();

	/*
	 * Step 3: Using the traces Learn an HA and create a model file M' (Abstract). This model at present we name as x0, x1, x2 and so on with the sequence input followed by output variables.
	 * Step 4: Using M' create a Simulink Model M' (having input and output variables like M) as per the user's input specification BUT DIFFERENT VARIABLE NAMES
	 * Step 5: Generate random input and perform simulation of M and M' and test for Equivalence
	 * 		 : If M == M' for all random inputs then STOP we have generated the model M
	 * 		 : Otherwise, use the counter trace to improve the learning algorithm by Looping from Step 3 - Step 5
	 *
	 * Note: path for original and learn model can be/is stored in the class intermediate.
	 */

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

    cout << "generating initial traces" << endl;

    // ep is initialized in this function
    // list_inputs and list_outputs are initial data for the equivalence test
	std::list<struct timeseries_all_var>  list_inputs;
	std::list<std::vector<double> > list_outputs;
	generate_initial_traces_for_learn_ha_loop(list_inputs, list_outputs, params, ep, report);

	unsigned int tot_count=0;

	model_setup::ptr la_setup = model_setup::ptr(new model_setup(H, userInputs)); //creates a copy of H and userInputs

	std::vector<double> CE_output_var;
	std::list<struct timeseries_input> CE_input_var;

    cout << "entering learn_ha loop..." << endl;

    // Need to copy ORIGINAL_MODEL_TRACES ORIGINAL_MODEL_TRACES_FOR_LERANING
    // since ORIGINAL_MODEL_TRACES will be overwritten by equivalenceTesting_for_learn_ha_loop
	system_copy_file(userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES),
                     userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES_FOR_LEARNING));

    // Loop stops when StopTime-limit exceeds Conclusion could not be drawn concretely
	while (true) {

		//Efficient Learning Loop
		if (tot_count != 0) { //skip on the initial iteration
            // XXX This function must append ORIGINAL_MODEL_TRACES to ORIGINAL_MODEL_TRACES_FOR_LEARNING
			updateTraceFile(params);	//adds new time-serise data to the initial trace-file
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
        // creating .slx and .m (running script) files, taking care of the variable-names for learned model in the presence of original model
        // It also generates learned_model_run[0..].m but does NOT execute it.
		constructModel(tot_count, params, ep);	

		//Execute in loop the learned_model and then the original_model and test the equivalence-distance
		bool for_paper = true;
        bool quit = false;

		if (for_paper) {
			quit = equivalenceTesting_for_learn_ha_loop(tot_count, la_setup, list_inputs, list_outputs, CE_output_var, CE_input_var, params, ep); //these list_inputs and list_outputs are generated using original_model's variable-names
        } else {
			quit = true;	//We do not want to perform equivalence check for this paper we present only a Passive learning.
        }

        quit = false;
        
		double stopTime = report->computeTotalTimeElapsed();
		cout <<"Computed Total Elapse Time = " << stopTime <<"     userInputs->getMaxStoptime()=" << userInputs->getMaxStoptime() << endl;

		if (quit) {
            cout << "Exiting the loop because flag = true" << endl;
            break; //could not find a CE, so equivalent. Therefore NO improvement in learning required.
        }
        else {
            //found a counter_example init_point for the LOOP
			// just store or use this counterexample CE_output_var, CE_input_var
			// the last executed output result can be updated for learning
            // struct CE_list CEs;
			// all_CEs.push_back(CEs);
		}

		if (stopTime >= userInputs->getMaxStoptime()) {
            cout << "Exiting the loop due to timeout loops=" << tot_count << endl;
            break;
        }

		tot_count++;
	}
    //	report->printSummary();
}


// Generate file containing simulation traces of the original simulink model
void generate_initial_traces_for_learn_ha_loop(std::list<struct timeseries_all_var> &list_input_variable_values,
                                               std::list<std::vector<double> > &list_output_variable_values,
                                               parameters::ptr params,
                                               std::unique_ptr<MATLABEngine> &ep,
                                               summary::ptr &report)
{
    // cout << "generate_initial_traces_for_learn_ha_loop()..." << endl;

	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

    // Assign uint indices to the input and output variables
    { 
        unsigned int index=0;

        std::list<std::string> inputVarList =  userInputs->getListInputVariables();
        for (std::list<std::string>::iterator it = inputVarList.begin(); it != inputVarList.end(); it++) {
            std::string variableName = *it;
            H->insert_to_map(variableName, index);
            index++;
        }

        std::list<std::string> outputVarList =  userInputs->getListOutputVariables();
        //ind=0; should continue
        for (std::list<std::string>::iterator it = outputVarList.begin(); it != outputVarList.end(); it++) {
            std::string variableName = *it;
            H->insert_to_map(variableName, index);
            index++;
        }
    }

	// --------------

	std::list<std::vector<double> > initial_output_values;
	std::list<struct timeseries_all_var> initial_inputValues_timeSeriesData;

    // Generate inputs of size max(initial-simu-size, max-traces).
    // core_engine/simulation/simualtion_utilities.cpp
	generate_input_information(initial_inputValues_timeSeriesData, initial_output_values, params, ep, report);

    // Simulate the original model using only getSimuInitSize() samples.
	{
        // But for initial traces only use initial-simu-size
        std::list<std::vector<double> > initial_simu_output_values;
        std::list<struct timeseries_all_var> initial_simu_inputValues_timeSeriesData;
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
    }

    // The entire inputs are used for the equivalence test
    {
        std::list<std::vector<double> > initial_output_for_equivalence;
        std::list<struct timeseries_all_var> initial_input_timeseries_for_equivalence;
        std::list<struct timeseries_all_var>::iterator it_in = initial_inputValues_timeSeriesData.begin();
        std::list<std::vector<double> >::iterator it_out = initial_output_values.begin();
        for (unsigned int i=0; i < userInputs->getMaxTraces(); i++) {	//copying only the first few inputs for generating initial traces for Equivalence Testing
            initial_output_for_equivalence.push_back(*it_out);
            initial_input_timeseries_for_equivalence.push_back(*it_in);
            it_out++;
            it_in++;
        }
        list_input_variable_values = initial_input_timeseries_for_equivalence;
        list_output_variable_values = initial_output_for_equivalence;
    }
}


//Generate trace from Original Model is appended
void updateTraceFile(parameters::ptr params)
{

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();

	// Adds new time-serise data to the initial trace-file.
    // In fact we do not need to simulate again just append the result-trace-file

	cout << "Now going to Simulate the Original Model to update the Simulation-Trace-File" << endl;

	// ***** Trace file result.txt generated by Simulink *************
	// *** Now append the trace into the simulation_model.txt file
	string previous_SimulationTraceFile = userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES_FOR_LEARNING);
	cout << "previous_SimulationTraceFile = "<< previous_SimulationTraceFile << endl;
	//Now get result-file name from the original-running-path

	std::string resultFileName = userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES);
	cout << "original Simulation TraceFile = "<< resultFileName << endl;

    system_append_file(resultFileName, previous_SimulationTraceFile);
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
	learnHA_caller(userInputs, userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES_FOR_LEARNING));	//Make is Simple and call it from everywhere. This invokes our "HA learning Algorithm".
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
	model->generateSimulinkModelScript(); // generateSimulinkModel[0..].m
	model->executeSimulinkModelConstructor(ep); // simulink_model[0..].slx

	//running script file generated for learned model with just the input variable name
	// ----------------------run script generator-----------------------------

    // Must be as same as the one of model->executeSimulinkModelConstructor(ep)
    std::string simulink_model_filename = userInputs->getFilenameUnderOutputDirectory(formatString(LEARNED_MODEL_SLX_FMT, count));
    std::string script_filename = userInputs->getFilenameUnderOutputDirectory(formatString(LEARNED_MODEL_SIMULATE_M_FMT, count));
    std::string output_filename = userInputs->getFilenameUnderOutputDirectory(formatString(LEARNED_MODEL_TRACES_FMT, count));
	model->generateRunLearnedModelScript(simulink_model_filename, script_filename, output_filename);

	// ----------------------------------------------------------------------------
}

// XXX This function is never called for now
//Returns True: when models are equivalent, otherwise False.
//Note: These list_inputs and list_outputs are generated using original_model's variable-names, needed to handle learned-variable-names
bool equivalenceTesting_for_learn_ha_loop(unsigned int iteration,
                                          model_setup::ptr la_setup,
                                          std::list<struct timeseries_all_var> &list_inputs,
                                          std::list<std::vector<double> > &list_outputs,
                                          std::vector<double> &CE_output_var,
                                          std::list<struct timeseries_input> &CE_input_var,
                                          parameters::ptr params,
                                          std::unique_ptr<MATLABEngine> &ep)
{
	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

	/*
	 * Note:
	 * 1) User supplies only a single simulink model file as black-box system input
	 * 2) the learned simulink model will have name supplied by the option --out-file for eg., out.txt by default and so model's name is out0.slx, out1.slx and so on for different iterations
	 * 3) Here list_inputs and list_outputs have variable-names as original-names
	 */

	std::string model_file_orig = userInputs->getSimulinkModelFilename();
	std::string script_filename_orig = userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_SIMULATE_M);
    std::string output_filename_orig = userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES);

	std::string model_file_learned = userInputs->getFilenameUnderOutputDirectory(formatString(LEARNED_MODEL_SLX_FMT, iteration));
	std::string script_filename_learned = userInputs->getFilenameUnderOutputDirectory(formatString(LEARNED_MODEL_SIMULATE_M_FMT, iteration));
    std::string output_filename_learned = userInputs->getFilenameUnderOutputDirectory(formatString(LEARNED_MODEL_TRACES_FMT, iteration));

    //-----------------------------------------------------------------------------------------------------

	bool flagEquivalent=true;
	double maxDistance=0.0;
	std::list<std::vector<double> >::reverse_iterator it_out_val = list_outputs.rbegin(); //iterator for the output variables

	for (std::list<struct timeseries_all_var>::reverse_iterator it = list_inputs.rbegin();
         it != list_inputs.rend();
         it++, it_out_val++)
    {
		std::list<struct timeseries_input> init_point = (*it).timeseries_signal;
		std::vector<double> output_variable_init_values = (*it_out_val);

		// ***************  --------------------------------- ***************
		//userInputs and H are in the learned model setup so first execute Learned model. But variables are in original-names
		cout <<"********* Running Learned model ***************** "<< endl;
		la_setup->setup_for_learned_model(H, userInputs);
		simulate_learned_model_from_learn_ha_loop(ep, userInputs, init_point, output_variable_init_values, script_filename_learned, output_filename_learned, intermediate, H, la_setup); //Populate initial data in Matlab's Workspace and then run the script file

		cout <<"********* Running Original model ***************** "<< endl;
		la_setup->setup_for_original_model(H, userInputs); //setting up to run the original model
		simulate_original_model_from_learn_ha_loop(ep, userInputs, init_point, output_variable_init_values, script_filename_orig, output_filename_orig, intermediate, H, la_setup); //Populate initial data in Matlab's Workspace and then run the script file

		// ***************  --------------------------------- ***************

		// -------------- Read the two files and compare --------------
		string file_original_with_path = output_filename_orig;

        string file_learned_with_path = output_filename_learned;

		flagEquivalent = compute_trace_equivalence(file_original_with_path, file_learned_with_path, maxDistance, params);

//		cout <<"Maximum Euclidean Distance=" << maxDistance << endl;
//		cout <<"Maximum Average Absolute difference between traces=" << maxDistance << endl;

		// -------------- --------------

        //found a counter-example
		if (!flagEquivalent) {
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
                                                        std::list<std::vector<double> > &initial_output_values,
                                                        parameters::ptr params,
                                                        std::unique_ptr<MATLABEngine> &ep,
                                                        summary::ptr &report)
{
    // cout << "generate_simulation_traces_original_model_to_learn() ..." << endl;

	intermediateResult::ptr intermediate = params->getIntermediate();
	user_inputs::ptr userInputs = params->getUserInputs();
	hybridAutomata::ptr H = params->getH();

	simulinkModelConstructor::ptr model = simulinkModelConstructor::ptr(new simulinkModelConstructor(H, userInputs, intermediate));
	model->setIteration(0);

	// We assume file generated is "run_script_simu_user_model.m"
	// Now creating script file for later running the simulation and obtaining output-file having fixed time-step
	// Creating a script file for Running the simulink model just created.
	std::string script_filename = userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_SIMULATE_M);
    std::string output_filename = userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES_TMP);
	std::string simulink_model_filename = userInputs->getSimulinkModelFilename();

    // It builds script_filename="$OUTDIR/run_original_model.m"
	model->generateRunModelScript(simulink_model_filename, script_filename, output_filename);

	// *************** setting up the mergedFile(s) ***************
	std::string simuFileName = userInputs->getFilenameUnderOutputDirectory(ORIGINAL_MODEL_TRACES);

    // rm $simuFileName $tmpSimuFile
    fs::remove(simuFileName);
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

    boost::timer::cpu_timer timer;
    timer.start();

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

        system_append_file(output_filename, simuFileName);

		matlab_execution_count++;
		userInputs->setNumberMatlabSimulationExecuted(matlab_execution_count);
		counting++;
	} //End of all simulation traces

	timer.stop();
	double wall_clock;
	wall_clock = timer.elapsed().wall / 1000000; //convert nanoseconds to milliseconds
	double running_time = wall_clock / (double) 1000;	//convert milliseconds to seconds
//	std::cout << "Matlab Simulation and Trace File generation: Running Time (in Seconds) = " << running_time << std::endl;
	report->setRuntimeMatlabInitialSimulation(running_time);

    // cout << "generate_simulation_traces_original_model_to_learn() done" << endl;
}



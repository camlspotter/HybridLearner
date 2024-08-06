/*
 * simulinkModelConstructor.h
 *
 *  Created on: 20-Oct-2021
 *      Author: amit
 */

#ifndef SIMULINKMODELLING_SIMULINKMODELCONSTRUCTOR_H_
#define SIMULINKMODELLING_SIMULINKMODELCONSTRUCTOR_H_


#include "../utilities/matlab.h"

#include <iomanip>
#include <sstream>
#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "string.h"
#include <string>

#include "../hybridautomata/hybridAutomata.h"
#include "../commandLineParser/user_inputs.h"
#include "../utilities/intermediateResult.h"

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace matlab::engine;


class simulinkModelConstructor {
	hybridAutomata::ptr ha;
	user_inputs::ptr user;
	intermediateResult::ptr intermediate;
	unsigned int iteration; //Keeps track of the number of iterations (when the .slx file created)

	// script file that generates the simulink model
    // "$OUTDIR/generateSimulinkModel0.m", "$OUTDIR/generateSimulinkModel1.m", etc
    string simulinkModelScriptFilename() const;

	//Printing an identity mapping for reset equations
	std::string resetPrinter();
	//Prints reset equations when the mapping is known
	std::string resetPrinter(std::list<reset_equation> reset_list);

	std::string generateInitialValues();

	//Auxiliary function
	/*
//	 * I assume Input is a constraint string of the form "left op val", where left is the variable involving the constraint, op is the operator
	 * valid operators are  >=, <= etc and val is the numeric value
	 * Returns left which is a variable name.
	 */
	std::string getVariableComponent(std::string constriant_str);

	void addFilteringCode(ofstream &modelfile);

	void generateSimulinkModelScript_sub(ofstream &outfile);
	void printDefinition(ofstream &outfile);

	void addMatlabFunction(ofstream &outfile);	//Adds a MATLAB Function inside StateFlow.Chart to generate a Random number for non-deterministic transitions
	void addLocations(ofstream &outfile);
//	void invariantPrinter(); Could not learned Invariant
	void addTransitions(ofstream &outfile);
	void addDefaultTransition(ofstream &outfile);
	void variableCreation(ofstream &outfile);
	void addInputComponents(ofstream &outfile);
	void addOutputComponents(ofstream &outfile);
	//void addConnectionLines(ofstream &outfile);

// ** Auxiliary functions **
	//void addMainTransitions(ofstream &outfile);
	void addLoopTransitions(ofstream &outfile, unsigned int sourceLoc, unsigned int number_loop_trans,
                            // unsigned int exec_order,
                            unsigned int pos_x,
                            unsigned int next_height, std::string condition_str, std::string reset_str);
//	void createConnectiveJunction(ofstream &outfile);
//	void completeLoopTransitions(ofstream &outfile);


	void inputVariableCreation(ofstream &outfile);
	void outputVariableCreation(ofstream &outfile);
	void localVariableCreation(ofstream &outfile);
	void parameterVariableCreation(ofstream &outfile);

	unsigned int getIteration() const;

	const string& getScriptForSimulinkModelName() const;

	void setScriptForSimulinkModelName(const string &scriptForSimulinkModelName);

	void createSmallScriptFile_ForFixedOutput();

    // simulink_model0, etc
    std::string simulinkModelName() const
    {
        return "simulink_model" + std::to_string(iteration);
    }

public:
	typedef boost::shared_ptr<simulinkModelConstructor> ptr;

	simulinkModelConstructor(){
		user = user_inputs::ptr (new user_inputs());
		intermediate  = intermediateResult::ptr(new intermediateResult());
		ha = hybridAutomata::ptr(new hybridAutomata());
		iteration = 0;
	};

	simulinkModelConstructor(hybridAutomata::ptr &H, user_inputs::ptr &user_input, intermediateResult::ptr &inter) {
		ha = H;
		user = user_input;
		intermediate = inter;
		iteration = 0;
	};

    // Generate generateSimulinkModel[0..].m
	void generateSimulinkModelScript();

    // Execute generateSimulinkModel[0..].m
	int executeSimulinkModelConstructor(std::unique_ptr<MATLABEngine> &ep);

    // Generate script_filename which writes the result to output_filename
	// This is specific to the engine="simu"
    // Not for learned model
	void generateRunModelScript(fs::path simulink_model_filename, fs::path script_filename, fs::path output_filename);

    // Generate script_filename which writes the result to output_filename
	//This has similar function like create_runScript_for_simu_engine() but differs in the path
	void generateRunLearnedModelScript(fs::path simulink_model_filename, fs::path script_filename, fs::path output_filename);

    // Generate $OUTDIR/run_model_by_txt2slx[0..].m
	void generateRunModelByTxt2slxScript(std::list<struct timeseries_input> init_point, std::vector<double> initial_output_values);

	//The keeps track of the number of times Equivalence Test is executed and so it the .slx model files created.
	void setIteration(unsigned int iteration);
};


#endif /* SIMULINKMODELLING_SIMULINKMODELCONSTRUCTOR_H_ */

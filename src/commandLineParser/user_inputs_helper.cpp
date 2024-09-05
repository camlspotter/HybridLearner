/*
 * user_inputs_helper.cpp
 *
 *  Created on: 21-Feb-2023
 *      Author: amit
 */

#include "user_inputs_helper.h"

#include "../io_functions/data_structs.h"
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include "commandLineParser.h"
#include "../simulation/simulation.h"

user_inputs_helper::user_inputs_helper() {
	// TODO Auto-generated constructor stub
	//std::cout <<"Created user inputs helper object" << std::endl;
}

user_inputs_helper::~user_inputs_helper() {
	// TODO Auto-generated destructor stub
}


void user_inputs_helper::setVariable_CP_Size(std::list<struct control_points> &res, string variableName, unsigned int cp_size) {
	for (std::list<struct control_points>::iterator it = res.begin(); it != res.end(); it++) {
		if (it->var_name == variableName) {
			it->numberOf_cp = cp_size;
			break;
		}
	}
}

void user_inputs_helper::setVariableType(std::list<struct control_points> &res, string variableName, string variableType) {
	for (std::list<struct control_points>::iterator it = res.begin(); it != res.end(); it++) {
		if (it->var_name == variableName) {
			it->var_type = variableType;
			break;
		}
	}
}

std::list<struct control_points> user_inputs_helper::getUserInputSignal_parameter(user_inputs::ptr user_Inputs)
{
	/*
	 * Initialize the res with default Type and other details for all system-variables
	 *
	 * 1) Parse input_signal_type to determine Type of each variable (fixed-Steps or fixed-linear or var-Steps)
	 * 2) Parse numberOf_control_points to obtain the number of control-points for each variable
	 *
	 */

	std::list<struct control_points> res;

	int sys_dimension = user_Inputs->getSysDimension();

	//Initialize to default setting
	//Todo: this need to be generalized instead of hard-coding get from automata or user's input
	//std::cout <<"sys_dimension=" << sys_dimension << std::endl;	//
	//Fixing done only for --engine=txt2slx for now, this may work for other engine as well need to verify
	if (user_Inputs->getEngine()=="bbc-old-code") {	//just keeping the old code for bbc

		for (int i=0; i < sys_dimension; i++) {
			struct control_points sys_variable;
			sys_variable.var_index = i;

			string varName =  "x"+to_string(i);	//instead of hard-coding get from automata or user's input


			sys_variable.var_name = varName;
			sys_variable.numberOf_cp = 0;
			sys_variable.var_type = "fixed-step";
			sys_variable.cps.resize(0);

			res.push_back(sys_variable);
		}
	} else if (user_Inputs->getEngine()=="txt2slx"
               || user_Inputs->getEngine()=="simu"
               || user_Inputs->getEngine()=="equi-test"
               || user_Inputs->getEngine()=="learn-ha"
               || user_Inputs->getEngine()=="learn-ha-loop"
               || user_Inputs->getEngine()=="bbc" ) {
		std::list<std::string> inputVariables = user_Inputs->getListInputVariables();
		int i=0;	//Todo: here we assume indices of input variables are first in the hybrid-automata
		for (list<std::string>::iterator it = inputVariables.begin(); it != inputVariables.end(); it++) {
			string varName = *it;	//extract the variable name
			struct control_points sys_variable;

			sys_variable.var_index = i;	//this index is used also in learn-ha-loop engine
			//Fixing: take the index from the hybrid-automaton. Assumption parsing and mapping of variable to hybrid-automaton must be done before call to this function
			//sys_variable.var_index = ha->get_index(varName);

			sys_variable.var_name = varName;
			sys_variable.numberOf_cp = 0;
			sys_variable.var_type = "fixed-step";
			sys_variable.cps.resize(0);

			res.push_back(sys_variable);
			i++;
		}
		//std::cout <<"sys_dimension=" << sys_dimension  << std::endl;
	}


	// ******** Parsing input_signal_type ********
	std::list<std::string> signalType_list;
	std::string signal_type = user_Inputs->getInputSignalType();	//input_signal_type;
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("& ");
	tokenizer tokens(signal_type, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter) {
		signalType_list.push_back((std::string) *tok_iter);
	}

	for (std::list<std::string>::iterator st = signalType_list.begin(); st != signalType_list.end(); ++st) {
		std::string expr = *st;	//Now expression is of the "x0=fixed-step"
		boost::char_separator<char> sep1("=");
		tokenizer each_term(expr, sep1);
		tokenizer::iterator it = each_term.begin(); //Reading the first token, which is Variable's name

		std::string varName =  *it;	//variable name
		//std::cout <<"varName is = " << varName <<endl;
		it++;	//Reading the second token, which is Variable's value
		std::string varType = *it;	//Variable-Type
		//std::cout <<"varType is = " << varType <<endl;

		setVariableType(res, varName, varType);
	}

	// ******** Parsing numberOf_control_points ********
	std::list<std::string> var_cp_size_list;
	std::string var_cp_size = user_Inputs->getNumberOfControlPoints();	//numberOf_control_points;
	//typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep2("& ");
	tokenizer tokens2(var_cp_size, sep2);
	for (tokenizer::iterator tok_iter = tokens2.begin();
			tok_iter != tokens2.end(); ++tok_iter) {
		var_cp_size_list.push_back((std::string) *tok_iter);
	}
	for (std::list<std::string>::iterator st = var_cp_size_list.begin(); st != var_cp_size_list.end(); ++st) {
		std::string expr = *st;	//Now expression is of the "x0=2"
		boost::char_separator<char> sep1("=");
		tokenizer each_term(expr, sep1);
		tokenizer::iterator it = each_term.begin(); //Reading the first token, which is Variable's name

		std::string varName =  *it;	//variable name
		it++;	//Reading the second token, which is Variable's cp_size value
		std::string varVal = *it;	//cp_size value
		double cp_size_Value = (double) std::atof(varVal.c_str());


		setVariable_CP_Size(res, varName, cp_size_Value);
	}


	return res;
}



std::list<struct timeseries_all_var> user_inputs_helper::generate_input_signals(std::unique_ptr<MATLABEngine> &ep,
                                                                                parameters::ptr params)
{
    cout << "generate_input_signals()..." << endl;

	std::list<struct timeseries_all_var> initial_simulation_timeSeriesData; //For N-simulation
	user_inputs::ptr user_Inputs = params->getUserInputs();

	/*
	 *
	 * 1) First create Hyperbox of the initial-value and Compute the desired 'N' random control-points for each variables.
	 *
	 * 2) Get the input parameters for each variable (Type: fixed-Steps or var-Steps and number of control-points for each variable)
	 * 3) Generate Fixed-Steps or linear or spline signals based on user's input choice
	 * 		a) generate pure-Random "cp-" number of control points for "N-" number of simulations input signals
	 * 		b) Calculate for each variable the hold_time_i := TimeHorizon / numberOfCP
	 * 		   Finally, compute timeVector[i] =  hold_time_i + Summation_hold_time_i-1
	 *
	 */

	std::list<std::list<struct control_points>> N_list_control_points; //N-list of list-of control-points for each variable.
	std::list<struct control_points> list_control_points = getUserInputSignal_parameter(user_Inputs); 	//get this value from user_Inputs_helper
	//Method-1: randomly generate simulation-trace based on the user's input for option: simu-init-size

    polytope::ptr init_poly_usersInput = polytope::ptr(new polytope());

    if (user_Inputs->getEngine()=="bbc-old-code") {	//Required for --engine="bbc". Now variable mapping to H is done before call to this function
        mapsVariable_to_Polytope(user_Inputs, init_poly_usersInput); //init_poly now has variableName-Index mapping
    } // For "txt2slx": variableName-Index mapping is already done in the main function

    string_list_to_polytope(user_Inputs->getInitialSet(), init_poly_usersInput); //Note: when the parsing model do not have ODE for the input variable, but variable mapping done from user's supplied information.
    // so, the init_poly has both input and output variables.
    //init_poly = init_poly_usersInput; //This init_poly is used elsewhere

    params->setInitPoly(init_poly_usersInput);

    unsigned int number_initial_points;
    if (user_Inputs->getEngine()=="learn-ha-loop") {
        number_initial_points = user_Inputs->getMaxGenerateTraceSize();	//todo: Remove: used only for taking reproducible reading
    } else if  (user_Inputs->getEngine()=="bbc") {
        number_initial_points = max(user_Inputs->getMaxTraces(), user_Inputs->getSimuInitSize());
    } else {
        number_initial_points = user_Inputs->getSimuInitSize();
    }
    assert(number_initial_points > 0);
    
    myRandomNumberGenerator::ptr randomGenObject = params->getRandomGenObj();

    // `number_initial_points` random points inside the polyope specified by `init_poly_usersInput`
    // Actually, the polytope is simply a hyperbox.
    N_list_control_points = getInternalControlPoints(init_poly_usersInput, number_initial_points, list_control_points, randomGenObject);

    double amplitude, zero_offset;

    cout << "N_list_control_points: " << N_list_control_points.size() << endl;

    for (std::list<std::list<struct control_points>>::iterator it_n = N_list_control_points.begin(); it_n != N_list_control_points.end(); it_n++) {
        struct timeseries_all_var all_variable_set;

        std::list<struct timeseries_input> each_data_set;		//For all variables

        std::list<struct control_points> cp_set = (*it_n); //total random signals
        for (std::list<struct control_points>::iterator it_var= cp_set.begin(); it_var != cp_set.end(); it_var++) {

            struct timeseries_input value1;
            vector<double> time_vector;
            vector<double> var_vector;

            // cout << "Var " << it_var->var_name << " : " << it_var->var_type << endl;

            // var_type is either fixed-step, var-step, linear, spline, or sine-wave
            if ((*it_var).var_type == "fixed-step" || (*it_var).var_type == "var-step") { //Todo: work on variable-STEP constant-piecewise signal
                fixed_step_signal(user_Inputs->getTimeHorizon(), (*it_var).cps, time_vector, var_vector);
            } else if ((*it_var).var_type == "linear") {
                linear_signal(user_Inputs->getTimeHorizon(), (*it_var).cps, time_vector, var_vector);
            } else if ((*it_var).var_type == "spline") {
                spline_signal(ep, user_Inputs->getTimeHorizon(), (*it_var).cps, time_vector, var_vector);
            } else if ((*it_var).var_type == "sine-wave") {
                std::map<std::string, std::pair<double, double>> sine_parameter = user_Inputs->getInputVariableSinewaveParameterMapping();
                std::pair<double, double> param_values ;
                try{
                    param_values = sine_parameter.at((*it_var).var_name);	//if item is present finds with no error
                    amplitude = param_values.first;
                    zero_offset = param_values.second;
                }
                catch (std::out_of_range&) {	//when item is absent returns this error
                    std::cout<< "User selected Input variable " << (*it_var).var_name << " as a sine-wave signal but missing parameters!!" << std::endl;
                    std::cout<< "Please enter values for the option --sine-wave-parameter . For menu help type ./BBC4CPS --help" << std::endl;
                    exit(1);	//Terminating the Project here
                }

                sine_wave_signal(user_Inputs->getTimeHorizon(), amplitude, zero_offset, time_vector, var_vector);

            }

            value1.var_detail = (*it_var);
            value1.time_values = time_vector;
            value1.var_values = var_vector;

            each_data_set.push_back(value1);
        }
        all_variable_set.timeseries_signal = each_data_set;
        initial_simulation_timeSeriesData.push_back(all_variable_set);
    }

    cout << "generate_input_signals() done" << endl;

	return initial_simulation_timeSeriesData;
}

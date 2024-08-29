/*
 * equivalence_test_utils.cpp
 *
 *  Created on: 22-Feb-2023
 *      Author: amit
 */

#include "../../commandLineParser/parameters.h"
#include "../../utilities/vector_operations.h"

bool compute_trace_equivalence(std::string file_one, std::string file_two, double &max_distance, parameters::ptr params);

#ifdef PAPER

double call_python_compute_cost(std::string file_one, std::string file_two);

double call_python_compute_DTW_cost(std::string file_one, std::string file_two);

#endif

/*
 * This function is used in the engines:
 * 		equi-test
 * 		learn-ha-loop
 *
 */
//Reads the two result/trace files and test for equivalence
bool compute_trace_equivalence(std::string file_one, std::string file_two, double &max_distance, parameters::ptr params) {

	user_inputs::ptr userInputs = params->getUserInputs();


#ifdef PAPER
		//this True-block is called to only print the output printed from the function call_python_compute_cost() and not to iterate in the LOOP

		//computes and print average of absolute distance between two output signals (learned and original) given by Equation (4) in paper
		//"A Passive Online Technique for Learning Hybrid Automata from Input/Output Traces" by Iman Saberi, Fathiyeh Faghih and Farzad Sobhi Bavil. arXiv, Jan 2021  in paper "
		// ***** WE dont want to use this..... max_distance = call_python_compute_cost(file_one, file_two);

		//Now compute DTW (dynamic time warping) distance and correlation between the two signals (learned and original)
//		unsigned int input_variable_size = userInputs->getListInputVariables().size(), output_variable_size = userInputs->getListOutputVariables().size();
		//max_distance = call_python_compute_DTW_cost(file_one, file_two);
		cout <<"Not computing cost here" << endl;

#else

		double maxDist=0.0, dist = 0.0, epsilon = userInputs->getPrecisionEquivalence();
		unsigned int dim = userInputs->getSysDimension();
		vector<double>  variable_learned(dim), variable_original(dim);
		/*
		 * Read the two result.txt files and look for the same time-value and then compare other dimensions
		 */

		std::ifstream first_file(file_one); //file pointer created
		std::ifstream second_file(file_two); //file pointer created

		//cout <<"epsilon: " <<epsilon <<endl;
		//cout <<"second_file: " <<second_file <<endl;
		//cout <<"file_one: " <<file_one <<endl;
		//cout <<"file_two: " <<file_two <<endl;

		if (first_file && second_file) {
			std::string line1, line2;
			double t1=0, t2=0;
			//std::vector<double> variable_value(dim);
			while (std::getline(first_file, line1) && std::getline(second_file, line2)) {
				//std::cout << "first line: " << line1  << endl;
				//std::cout << "second line: " << line2 << endl;

				std::istringstream word1(line1);
				word1 >> t1;	//reads the first number which is time value from learned_file
				for (unsigned int i=0; i < dim; i++) {
					word1 >> variable_learned[i]; //Reading rest of the columns consisting of variables value.
				}

				std::istringstream word2(line2);
				word2 >> t2;	//reads the first number which is time value from original_file
				for (unsigned int i=0; i < dim; i++) {
					word2 >> variable_original[i]; //Reading rest of the columns consisting of variables value.
				}

				dist = euclidean_distance(variable_learned, variable_original);
				//cout <<"Dist = " << dist << "\n";
				if (dist > maxDist) {
					maxDist = dist;
					max_distance = dist;
				}
				if (dist >= epsilon) { //The Simulation Trace for the learned and original Model is not Similar by epsilon value.
					//cout <<"Dist = " << dist << "\n";
					max_distance = maxDist;
					cout <<"max_distance = " << max_distance << "\n";
					return false;
				}

				line1 = " ";
				line2 = " ";

			} //end-of all time-points
			cout <<"max_distance = " << max_distance << "\n";
		} //end of file reading
		first_file.close();	//closing the file
		second_file.close();	//closing the file

		//modelfile.close();

		max_distance = maxDist;
#endif

	return true;

}

#ifdef PAPER
double call_python_compute_cost(std::string file_one, std::string file_two) {
	double max_distance=0.0;
/*	currently the function is not in use
	linux_utilities::ptr linux_util = linux_utilities::ptr (new linux_utilities());
	intermediate->setMatlabDefaultPath(linux_util->getCurrentWorkingDirectoryWithPath());
	intermediate->setToolRootPath(linux_util->getParentWorkingDirectoryWithPath());


	std::string releasePath = intermediate->getToolRootPath();
	string runComputeCost = releasePath, cmd="python3 ";
	runComputeCost.append("/src/utilities/compute_cost.py");
	cmd.append(runComputeCost);
	cmd.append(" ");
	cmd.append(file_one);
	cmd.append(" ");
	cmd.append(file_two);
	unsigned int inputVar_size = userInputs->getListInputVariables().size();	//file will have 1st column as time followed by input_Variable and then output_Variable
	unsigned int outputVar_size = userInputs->getListOutputVariables().size();
	unsigned int total_size = 1 + inputVar_size + outputVar_size;	//1 is for time variable

	int x1 = system(cmd.c_str());
//	cout <<"Let me check x1 = " << x1 <<endl;

	double max_result=0.0, result=0.0, max_time=0.0;
	string modelfile ="cost_computed.txt";	//file created in python that stores the Average of absolute difference for each variable/columns
	std::ifstream in(modelfile);//This is the file generated by running the Learning Algorithm
	if (in) {
		std::string line1;
		std::getline(in, line1); //Reading First Line: Each getline() function will read a line and moves the pointer to the beginning of the next line
		std::istringstream value(line1);
		value >> max_time;	//read time the 1st column
		for (int i=1; i< total_size; i++) {
			value >> result;
			if (i >= (1+inputVar_size)) {	//checking only for output variables of the learned and original. Input difference will be zero being same inputs
				if (result >= max_result)
					max_result = result;
			}
		}
	}
	max_distance = max_result;

	cout <<"Average of absolute difference (Max of the output variables) = " << max_distance <<endl;*/

	return max_distance;
}

double call_python_compute_DTW_cost(std::string file_one, std::string file_two) {
	double max_distance=0.0;
/*	currently the function is not in use
	linux_utilities::ptr linux_util = linux_utilities::ptr (new linux_utilities());
	intermediate->setMatlabDefaultPath(linux_util->getCurrentWorkingDirectoryWithPath());
	intermediate->setToolRootPath(linux_util->getParentWorkingDirectoryWithPath());

	std::string releasePath = intermediate->getToolRootPath();
	string runComputeCost = releasePath, cmd="python3 ";
	runComputeCost.append("/src/utilities/compute_cost.py");
	cmd.append(runComputeCost);
	cmd.append(" ");
	cmd.append(file_one);
	cmd.append(" ");
	cmd.append(file_two);
	unsigned int inputVar_size = userInputs->getListInputVariables().size();	//file will have 1st column as time followed by input_Variable and then output_Variable
	unsigned int outputVar_size = userInputs->getListOutputVariables().size();
	unsigned int total_size = 1 + inputVar_size + outputVar_size;	//1 is for time variable

	cmd.append(" ");
	cmd.append(std::to_string(inputVar_size));
	cmd.append(" ");
	cmd.append(std::to_string(outputVar_size));

	int x1 = system(cmd.c_str());	//execute python function with arguments file1, file2, size of input and output variables.
//	cout <<"Let me check x1 = " << x1 <<endl;

	double max_result=0.0, result=0.0, max_time=0.0;
	vector<double>  correl_per_variable(outputVar_size);
	string modelfile ="cost_computed.txt";	//file created in python that stores the Average of absolute difference for each variable/columns
	std::ifstream in(modelfile);//This is the file generated by running the Learning Algorithm
	if (in) {
		std::string line1;
		std::getline(in, line1); //Reading First Line: Each getline() function will read a line and moves the pointer to the beginning of the next line
		std::istringstream value(line1);
		value >> max_distance;	//read distance the 1st column
		for (int i=0; i< outputVar_size; i++) {
			value >> correl_per_variable[i];
		}
	}
	cout <<"DTW Distance = " << max_distance <<  "\t \t Correlation (output variables) = ";
	for (int i=0; i< outputVar_size; i++) {
		cout << correl_per_variable[i]  << "\t";
	}
	cout << endl;*/
	return max_distance;
}
#endif

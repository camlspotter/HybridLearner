//=============================================================================
// Name        : HybridLearner.cpp
// Description : HybridLearner: A tool to learn Hybrid Systems for Cyber Physical Systems
// Author      : Amit Gurung, Kyoto University, Japan
// Version     : 1.0
//=============================================================================

#include <iostream>
#include "../commandLineParser/commandLineParser.h"
#include "../commandLineParser/user_inputs.h"

#include "engineSelector.h"


using namespace std;

unsigned int log_set = 1;


int main(int argc, char *argv[]) {


	user_inputs::ptr user_Inputs = user_inputs::ptr(new user_inputs());

	try {

		commandLineParser(argc, argv, user_Inputs);

	} catch (...) {
		std::cout << "\nTerminating due to error in command-line inputs.\n" ;
		std::cout << "Try HybridLearner --help to see valid command-line options\n";
		exit(1);
	}


	engineSelector engSelect(user_Inputs);

	engSelect.select();
	double runTime = engSelect.getReport()->getRuntimeLearningAlgo();
//	cout <<"Learning Time = " << runTime << endl;
	cout << runTime << endl;

	return 0;

}

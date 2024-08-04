/*
 * engineSelector.h
 *
 *  Created on: 27-Jun-2022
 *      Author: amit
 */

#ifndef APPLICATION_ENGINESELECTOR_H_
#define APPLICATION_ENGINESELECTOR_H_

#include "../utilities/matlab.h"
#include "../io_functions/summary.h"
#include "../hybridautomata/hybridAutomata.h"
#include "../utilities/myRandomNumberGenerator.h"
#include "../commandLineParser/user_inputs.h"
#include "../utilities/intermediateResult.h"
#include "../utilities/polytope/polytope.h"

using namespace std;
using namespace matlab::engine;

class engineSelector {
	/*
	 * selects one of the tool's engine from:
	 *  " - simu : Trajectory Simulation."
		" - txt2slx : model file in .txt to .SLX Matlab's model."
		" - learn-ha: learns an HA model from a simulation traces file
		" - learn-ha: learns an HA model from a Simulink model file	iteratively in a loop
		" - equi-test : perform equivalence testing for two .SLX Matlab's model files."
	 */
private:

	user_inputs::ptr userInputs;
	summary::ptr report;
	intermediateResult::ptr intermediate;
	hybridAutomata::ptr H;
	std::unique_ptr<MATLABEngine> ep;	// = connectMATLAB();
	//Engine *ep;
	polytope::ptr init_poly;

	myRandomNumberGenerator::ptr randomGenObject;

public:

	engineSelector();
	engineSelector(user_inputs::ptr userInputs);

	/*
	 * Selects one of the engine based on user option (default is bbc)
	 */
	void select();

	/* selects the Simulation engine */
	void selectSimu();

	/* selects the model file in .txt to .SLX Matlab's model engine */
	void selectMdl2Slx();

	/* Creates an Hybrid Automaton model (.txt file) from user supplied simulation trajectories */
	void selectLearn_HA();

	/* Learns an Automaton HA, given a simulink model M, by simulating M and learning M' in a refinement loop based on equivalence testing of M' and M. */
	void selectLearn_HA_loop();

	/* selects the equi-test (Equivalence Testing) engine */
	bool selectEquiTest();


	virtual ~engineSelector();
	const summary::ptr& getReport() const;
	void setReport(const summary::ptr &report);
};


#endif /* APPLICATION_ENGINESELECTOR_H_ */

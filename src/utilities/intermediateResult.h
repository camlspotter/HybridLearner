/*
 * intermediateResult.h
 *
 *  Created on: 24-Sep-2021
 *      Author: amit
 */

#ifndef UTILITIES_INTERMEDIATERESULT_H_
#define UTILITIES_INTERMEDIATERESULT_H_

#include <string>
#include <list>
#include <iostream>
#include <boost/shared_ptr.hpp>


class intermediateResult {
private:
	std::string dReach_sat_smt2filename_with_path;	//Eg., osci_1_0.smt2
	std::string dReach_sat_smt2filename_with_path_without_extension;	//Eg., osci_1_0   This is used to identify the correct json file

	std::string matlab_path_for_original_model;	//Absolute Path of the benchmark for Eg., "/home/amit/workspace/BBC4CPS/src/benchmark/circle"
	std::string matlab_path_for_learned_model;	//Absolute Path of the learned model for Eg., "/home/amit/workspace/BBC4CPS/Release/outfilename"

	bool original_model;	//True if previous execution path is the original model ("...../BBC4CPS/src/benchmark/circle"), Otherwise False
	bool learned_model;	//True if previous execution path is the learned model ("...../BBC4CPS/Release/outfilename"), Otherwise False

public:
	typedef boost::shared_ptr<intermediateResult> ptr;

	const std::string& getDReachSatSmt2filenameWithPath() const;
	void setDReachSatSmt2filenameWithPath(
			const std::string &dReachSatSmt2filenameWithPath);

	const std::string& getDReachSatSmt2filenameWithPathWithoutExtension() const;
	void setDReachSatSmt2filenameWithPathWithoutExtension(
			const std::string &dReachSatSmt2filenameWithPathWithoutExtension);

    // Equals to OutputDirectory.  Should be removed
	const std::string&  getMatlabPathForLearnedModel() const;
	void setMatlabPathForLearnedModel(
			const std::string &matlabPathForLearnedModel);

	const std::string&  getMatlabPathForOriginalModel() const;
	void setMatlabPathForOriginalModel(
			const std::string &matlabPathForOriginalModel);

	bool isLearnedModel() const;
	void setLearnedModel(bool learnedModel);

	bool isOriginalModel() const;
	void setOriginalModel(bool originalModel);
};

#endif /* UTILITIES_INTERMEDIATERESULT_H_ */

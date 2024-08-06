/*
 * intermediateResult.cpp
 *
 *  Created on: 25-Sep-2021
 *      Author: amit
 */


#include "intermediateResult.h"


const std::string& intermediateResult::getDReachSatSmt2filenameWithPath() const {
	return dReach_sat_smt2filename_with_path;
}

void intermediateResult::setDReachSatSmt2filenameWithPath(
		const std::string &dReachSatSmt2filenameWithPath) {
	dReach_sat_smt2filename_with_path = dReachSatSmt2filenameWithPath;
}

const std::string& intermediateResult::getDReachSatSmt2filenameWithPathWithoutExtension() const {
	return dReach_sat_smt2filename_with_path_without_extension;
}

void intermediateResult::setDReachSatSmt2filenameWithPathWithoutExtension(
		const std::string &dReachSatSmt2filenameWithPathWithoutExtension) {
	dReach_sat_smt2filename_with_path_without_extension =
			dReachSatSmt2filenameWithPathWithoutExtension;
}

bool intermediateResult::isLearnedModel() const {
	return learned_model;
}

void intermediateResult::setLearnedModel(bool learnedModel) {
	learned_model = learnedModel;
}

bool intermediateResult::isOriginalModel() const {
	return original_model;
}

void intermediateResult::setOriginalModel(bool originalModel) {
	original_model = originalModel;
}

/*
 * string_operations.h
 *
 *  Created on: 27-Feb-2023
 *      Author: amit
 *
 *      This file holds all operations related to string data type.
 */

#ifndef UTILITIES_STRING_OPERATIONS_H_
#define UTILITIES_STRING_OPERATIONS_H_

std::string concat_path(std::string a, std::string b);
std::string dirname(std::string path);
std::string basename(std::string path);
std::string basename_without_ext(std::string path);

#endif /* UTILITIES_STRING_OPERATIONS_H_ */

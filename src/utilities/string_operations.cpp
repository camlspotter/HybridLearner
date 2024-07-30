/*
 * string_operations.cpp
 *
 *  Created on: 27-Feb-2023
 *      Author: amit
 */


#include "string_operations.h"

// XXX Bug: this does not work properly when user_filename is absolute
// XXX Bug: Not portable
std::string getFileName_without_Path(std::string user_filename, std::string &absdirname){
	std::string filename_without_path;

	linux_utilities::ptr linux_util = linux_utilities::ptr (new linux_utilities());

	std::string key = "/";
	std::size_t found = user_filename.rfind(key);	// locate the last "/" character

	if (found != std::string::npos) {
        // Input has some '/'s.
        std::string basename = user_filename.substr(found+1);
		unsigned int tot_len = user_filename.length();
        unsigned int file_len = basename.length();
		file_len += 1; //to exclude the last '/' character in the path
        std::string dirname = user_filename.substr(0, tot_len - file_len);
        absdirname = "";
        absdirname.append(linux_util->getCurrentWorkingDirectoryWithPath());
		absdirname.append("/");
		absdirname.append(dirname);	// XXX breaks if dirname is absolute
        return basename;
	} else {
        // Contains no '/' 
        std::string basename = user_filename;
		absdirname = linux_util->getCurrentWorkingDirectoryWithPath();
        return basename;
	}
}


// XXX Bug: this does not work properly when filename is absolute
// XXX Bug: Not portable
std::string getFileNameWithPath(std::string filename, std::string path){

	//cout << "path = "  << path << endl;
	//cout << "filename = " << filename << endl;
	string str="";
	str.append(path);
	str.append("/");
	str.append(filename);

	//cout << "filename with path = " << str << endl;
	return str;
}

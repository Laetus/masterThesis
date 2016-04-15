/*
 * Utils.cpp
 *
 *  Created on: Jan 3, 2016
 *      Author: laetus
 */

#ifndef UTILS_CPP_
#define UTILS_CPP_

#include <casadi/casadi.hpp>
using namespace casadi;

class Utils {

public:
	static void readFromFile(std::string name, std::vector< Matrix<double>* > *dest) {
		std::ifstream infile(name);
		std::string line;
		for (unsigned int i = 0; i < dest->size()  ; i++) {
			std::getline(infile, line);
			std::vector<Matrix<double> > lines;
			boost::tokenizer<boost::escaped_list_separator<char> > tk(line,
					boost::escaped_list_separator<char>('\\', ',', '\"'));

			for (boost::tokenizer<boost::escaped_list_separator<char> >::iterator
					it(tk.begin()); it != tk.end(); ++it) {
				lines.push_back((Matrix<double> ) std::stod(*it));
			}
			*((*dest)[i]) = vertcat(lines);
		}
	}
};

#endif /* UTILS_CPP_ */

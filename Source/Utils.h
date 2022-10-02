#pragma once

#include <string>
#include <vector>

namespace musicus {

	using std::string;

	std::vector<string> split_string(string s, string delimiter);

	string get_file_ending(string f);

}

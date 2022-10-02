#include "Utils.h"

#include <string>
#include <vector>


using std::string;
using std::vector;

std::vector<string> musicus::split_string(string s, string separator) {
	std::vector<string> str_parts{};
	//Declare integer variable
	int position;
	//Declare string variable
	string outstr, token;

	/*
	Split the string using substr() function
	and adding the splitted word into the vector
	*/
	while ((position = s.find(separator)) != std::string::npos) {
		token = s.substr(0, position);
		//Remove the extra space from the front of the splitted string
		str_parts.push_back(token.erase(0, token.find_first_not_of(" ")));
		s.erase(0, position + separator.length());
	}
	//Add the last splitted word to the vector
	str_parts.push_back(s.erase(0, s.find_first_not_of(" ")));

	return str_parts;
}

string musicus::get_file_ending(string f) {
	std::vector<string> parts = musicus::split_string(f, ".");
	return parts[parts.size() - 1];
}
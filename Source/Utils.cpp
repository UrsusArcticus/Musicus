#include "Utils.h"

#include <string>
#include <vector>
#include <math.h>

#include <iostream>

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
	while ((position = (int)s.find(separator)) != std::string::npos) {
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

unsigned int musicus::read_int_from_bytes(char* bytes, int start_position) {
    char *a = new char[4] { bytes[start_position + 3], bytes[start_position + 2],
        bytes[start_position + 1], bytes[start_position]};
    return *reinterpret_cast<int*>(a);
}

std::string musicus::read_string_from_bytes(char* bytes, int start, int length) {
    char s[length + 1];
    for (int i = start; i < length; i++) {
        s[i - start] = bytes[i];
    }
    s[length] = '\0';
    return std::string(s);
}

/*
 This function is for study purposes but is not required for the functioning of Musicus
 */
template <typename T>
std::string musicus::print_bits(T value) {
    int size_in_bits = sizeof(T) * 8;
    std:string r = "";
    unsigned int p;
    for (int i = size_in_bits - 1; i >= 0 ; i--) {
        p = (int)pow(2, i);
        r.append(((p & value) == p) ? "1" : "0");
    }
    return r;
}
template std::string musicus::print_bits(char value);
template std::string musicus::print_bits(int value);


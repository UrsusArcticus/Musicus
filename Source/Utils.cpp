#include "Utils.h"

#include <string>
#include <vector>
#include <math.h>

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

string musicus::get_file_extension(string f) {
	std::vector<string> parts = musicus::split_string(f, ".");
	return parts[parts.size() - 1];
}

string musicus::get_filename(std::string path) {
    std::vector<string> parts = musicus::split_string(path, "/");
    return parts[parts.size() - 1];
}

string musicus::get_filename_without_extension(string path) {
    std::vector<string> path_parts = split_string(path, "/");
    std::vector<string> file_parts = split_string(path_parts[path_parts.size() - 1], ".");
    auto size = file_parts.size();
    return file_parts[ size > 1 ? size - 2 : size - 1];
}

unsigned int musicus::read_int_from_bytes(char* bytes, int start_position) {
    char *a = new char[4] { bytes[start_position + 3], bytes[start_position + 2],
        bytes[start_position + 1], bytes[start_position]};
    return *reinterpret_cast<int*>(a);
}

std::string musicus::read_string_from_bytes(char *bytes, int start_position) {
    int i = start_position;
    while ( bytes[i] != 0 ) {
        i++;
    }
    return read_string_from_bytes(bytes, start_position, i - start_position);
}

std::string musicus::read_string_from_bytes(char* bytes, int start, int length) {
    char s[length + 1];
    for (int i = start; i < (start + length); i++) {
        s[i - start] = bytes[i];
    }
    s[length] = '\0';
    return std::string(s);
}


std::string musicus::left_pad(const std::string& str, size_t width, char fill = ' ') {
    if (str.size() >= width) return str;
    return std::string(width - str.size(), fill) + str;
}

std::string musicus::create_timestring(double seconds) {
    int s = static_cast<int>(std::ceil(seconds));
    int r = s % 60;
    int m = s / 60;
    return std::to_string(m) + ":" + left_pad(std::to_string(r), 2, '0');
}

std::string musicus::get_directory(std::string path) {
    return path.substr(0, path.find_last_of("/"));
}

bool musicus::has_image_extension(string f) {
    string image_file_extensions[] = {"jpg", "jpeg"};
    string extension = get_file_extension(f);
    for (string e : image_file_extensions) {
        if ( e == extension ) { return true; }
    }
    return false;
}

/*
 These functions are for study purposes but are not required for Musicus to work
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

string musicus::bytes_to_string(char *bytes, int size) {
    string s;
    for ( int i = 0; i < size; i++) {
        s.append(std::to_string((int)bytes[i]));
        s.append(" ");
    }
    return s;
}

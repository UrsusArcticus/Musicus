#pragma once

#include <string>
#include <vector>

namespace musicus {

std::vector<std::string> split_string(std::string s, std::string delimiter);

std::string get_file_ending(std::string f);

unsigned int read_int_from_bytes(char* bytes, int start_position);

std::string read_string_from_bytes(char* bytes, int start);

std::string read_string_from_bytes(char* bytes, int start, int length);

template <typename T>
std::string print_bits(T);

}

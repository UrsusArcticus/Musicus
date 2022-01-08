#pragma once

#include <string>
#include <vector>

namespace musicus {

std::vector<std::string> split_string(std::string s, std::string delimiter);

std::string get_file_ending(std::string f);

unsigned int btoi(char* bytes, int size, int offset);

std::string bytes_to_string(char* bytes, int start, int length);

template <typename T>
std::string print_bits(T);

}

#pragma once

#include <string>
#include <vector>

namespace musicus {

std::vector<std::string> split_string(std::string s, std::string delimiter);

std::string get_file_ending(std::string f);

unsigned int read_int_from_bytes(char* bytes, int start_position);

std::string read_string_from_bytes(char* bytes, int start);

std::string read_string_from_bytes(char* bytes, int start, int length);

std::string left_pad(const std::string&, size_t, char);

std::string create_timestring(double seconds);

template <typename T>
std::string print_bits(T);

std::string bytes_to_string(char *bytes, int size);

}

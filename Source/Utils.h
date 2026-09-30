#pragma once

#include <string>
#include <vector>

namespace musicus {

std::vector<std::string> split_string(std::string s, std::string delimiter);

std::string get_file_extension(std::string f);

std::string get_filename(std::string path);

std::string get_filename_without_extension(std::string path);

unsigned int read_int_from_bytes(char* bytes, int start_position);

std::string read_string_from_bytes(char* bytes, int start);

std::string read_string_from_bytes(char* bytes, int start, int length);

std::string left_pad(const std::string&, size_t, char);

std::string create_timestring(double seconds);

std::string get_directory(std::string path);

bool has_image_extension(std::string);

/*
 These functions are for study purposes but are not required for Musicus to work
 */
template <typename T>
std::string print_bits(T);

std::string bytes_to_string(char *bytes, int size);

}

/*
  ==============================================================================

    Config.cpp
    Created: 2 Mar 2026 12:34:23am
    Author:  Philipp Otte

  ==============================================================================
*/

#include "Config.h"
#include <JuceHeader.h>
#include "simdjson.h"
#include <fstream>
#include <cstdlib>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#endif

using namespace std;
using namespace simdjson;

musicus::Config::Config() {
    string file_name, path;
    #if __linux__
    const char* user_profile = getenv("HOME");
    path = string(user_profile).append("/.config/musicus/");
    file_name = "settings.json";
    #elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    const char* user_profile = getenv("USERPROFILE");
    if ( user_profile != nullptr ) {
        path = string(user_profile).append("\\AppData\\Musicus\\Roaming\\");
    } else {
        path = "C:\\Users\\Public\\AppData\\Roaming\\";
    }
    file_name = "settings.json";
    #elif __APPLE__
    const char* user_profile = getenv("HOME");
    path = string(user_profile).append("/Library/Application Support/Musicus/");
    file_name = "settings.json";
    #endif
    juce::File config_directory(path);
    if ( !config_directory.isDirectory() ) {
        config_directory.createDirectory();
    }
    this->config_file = path.append(file_name);
}

string musicus::Config::getLibraryPath() {
    return this->library_path;
}

void musicus::Config::setLibraryPath(string path) {
    this->library_path = path;
}

void musicus::Config::loadConfig() {
    juce::File appConfig{juce::String{this->config_file}};
    if ( appConfig.exists() ) {
        ondemand::parser parser;
        auto text = padded_string::load(config_file);
        ondemand::document document = parser.iterate(text);
        ondemand::object r = document.get_object();
        ondemand::object settings = r.find_field("settings");
        for (auto field : settings) {
            std::string_view key = field.unescaped_key();
            if ( key == "library" ) this->library_path = string{field.value().get_string().value()};
        }
    }
}

void musicus::Config::saveConfig() {
    simdjson::builder::string_builder builder;
    builder.start_object();
    builder.escape_and_append_with_quotes("settings");
    builder.append_colon();
    builder.start_object();
    builder.append_key_value("library", this->library_path);
    builder.end_object();
    builder.end_object();
    string_view object{builder};
    juce::File file{config_file};
    file.replaceWithText(string{object.data(), object.size()});
}

bool musicus::Config::libraryExists() {
    return ifstream(this->library_path.append("/.metadata.musicuslib")).good();
}

/*
  ==============================================================================

    Config.h
    Created: 2 Mar 2026 12:33:46am
    Author:  Philipp Otte

  ==============================================================================
*/

#pragma once

#include <string>

using namespace std;

namespace musicus {

class Config {
    string config_file;
    string library_path = "";
public:
    Config();
    string getLibraryPath();
    void setLibraryPath(string library_path);
    void loadConfig();
    void saveConfig();
    bool libraryExists();
};

}

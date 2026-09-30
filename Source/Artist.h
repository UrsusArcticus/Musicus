#pragma once

#include <string>
#include <vector>

#include "Album.h"

using std::string;

namespace musicus {

	class Artist {
	private:
		string name;
		vector<Album*>* albums;
	public:
        Artist(string _name);
        string getName();
        void addAlbum(Album *album);
        vector<Album*>* getAlbums();
        bool hasAlbum(Album*);
	};
}

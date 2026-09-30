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
        string getName() const;
        void addAlbum(Album *album);
        vector<Album*>* getAlbums();
        bool hasAlbum(Album*);
        bool operator<(const Artist& other) const;
	};
}

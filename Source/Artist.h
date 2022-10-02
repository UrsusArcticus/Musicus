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
		Artist(string _name) : name(_name) {
			albums = new vector<Album*>();
		}

		string getName() {
			return name;
		}

		void addAlbum(Album *album) {
			this->albums->push_back(album);
		}

		vector<Album*>* getAlbums() {
			return albums;
		}
	};
}
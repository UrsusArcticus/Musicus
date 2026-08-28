#include <string>
#include <vector>

#include "Song.h"

using std::string;
using std::vector;

namespace musicus {

	class Album {
	private:
		string name;
		vector<Song*>* tracks;
	public:
		Album(string _name) : name(_name) {
			tracks = new vector<Song*>();
		}

		string getName() {
			return name;
		}

		void addTrack(Song *t) {
			this->tracks->push_back(t);
		}

		vector<Song*>* getSongs() {
			return tracks;
		}
	};
}

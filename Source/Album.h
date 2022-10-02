#include <string>
#include <vector>

#include "Track.h"

using std::string;
using std::vector;

namespace musicus {

	class Album {
	private:
		string name;
		vector<Track*>* tracks;
	public:
		Album(string _name) : name(_name) {
			tracks = new vector<Track*>();
		}

		string getName() {
			return name;
		}

		void addTrack(Track *t) {
			this->tracks->push_back(t);
		}

		vector<Track*>* getTracks() {
			return tracks;
		}
	};
}
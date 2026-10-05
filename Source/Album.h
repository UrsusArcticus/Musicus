#include <string>
#include <vector>

#include "Song.h"

using std::string;
using std::vector;

namespace musicus {

	class Album {
	private:
		string name;
		vector<Song*>* songs;
	public:
        Album(string _name);
        string getName();
        void addSong(Song *t);
        vector<Song*>* getSongs();
        void sortSongs();
        bool operator<(const Album& other) const;
	};
}

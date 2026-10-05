#include "Album.h"

using std::string;
using std::vector;
using musicus::Song;

musicus::Album::Album(string _name) : name(_name) {
    songs = new vector<Song*>();
}

string musicus::Album::getName() {
    return name;
}

void musicus::Album::addSong(Song *t) {
    this->songs->push_back(t);
}

vector<Song*>* musicus::Album::getSongs() {
    return songs;
}

void musicus::Album::sortSongs() {
    std::sort(songs->begin(), songs->end(), [](const Song* a, const Song* b) { return *a < *b; });
}

bool musicus::Album::operator<(const Album& other) const {
    return name < other.name;
}

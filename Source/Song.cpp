#include "Song.h"

string musicus::Song::getSongName() {
    return song_name;
};
    
void musicus::Song::setSongName(string name) {
    song_name = name;
};
    
string musicus::Song::getArtist() {
    return artist;
};

void musicus::Song::setArtist(string _artist) {
    artist = _artist;
};

string musicus::Song::getAlbum() {
    return album;
};

void musicus::Song::setAlbum(string _album) {
    album = _album;
};
    
string musicus::Song::getTrack() {
    return this->track;
}
    
void musicus::Song::setTrack(string _track) {
    this->track = _track;
}

string musicus::Song::getDuration() {
    return this->duration;
}

void musicus::Song::setDuration(string duration) {
    this->duration = duration;
}

string musicus::Song::getComposer() {
    return this->composer;
}
    
void musicus::Song::setComposer(string _composer) {
    this->composer = _composer;
}
    
string musicus::Song::getFile() {
    return file;
}

void musicus::Song::setFile(string _file) {
    this->file = _file;
}

int musicus::Song::getTrackAsInt() const {
    try {
        size_t si = track.find_first_of("/");
        return std::stoi(si == string::npos ? track : track.substr(0, si));
    } catch (std::invalid_argument& e) {
        return -1;
    }
}

bool musicus::Song::operator<(const Song& other) const {
    if ( track.empty() && other.track.empty() ) {
        return song_name < other.song_name;
    } else if ( track.empty() ) {
        return false;
    } else if ( other.track.empty() ) {
        return true;
    }
    return getTrackAsInt() < other.getTrackAsInt();
}

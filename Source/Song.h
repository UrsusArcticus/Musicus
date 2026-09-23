#pragma once

#include <string>

using std::string;

namespace musicus {
	
class Song {
private:
    string song_name = "";
    string artist = "";
    string album = "";
    string track = "";
    string composer = "";
    string file = "";

public:
    string getSongName() {
        return song_name;
    };
		
    void setSongName(string name) {
        song_name = name;
    };
		
    string getArtist() {
        return artist;
    };

    void setArtist(string _artist) {
        artist = _artist;
    };

    string getAlbum() {
        return album;
    };

    void setAlbum(string _album) {
        album = _album;
    };
        
    string getTrack() {
        return this->track;
    }
        
    void setTrack(string _track) {
        this->track = _track;
    }

    string getComposer() {
        return this->composer;
    }
        
    void setComposer(string _composer) {
        this->composer = _composer;
    }
        
    string getFile() {
        return file;
    }

    void setFile(string _file) {
        this->file = _file;
    }
};

struct SongImage {
    char *data;
    int size;
};

enum SongStatus {
    NONE,
    PLAYING,
    PAUSED,
    STOPPED
};

}

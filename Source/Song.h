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
    string duration = "";
    string composer = "";
    string file = "";

public:
    string getSongName();
    void setSongName(string name);
    string getArtist();
    void setArtist(string _artist);
    string getAlbum();
    void setAlbum(string _album);
    string getTrack();
    void setTrack(string _track);
    string getDuration();
    void setDuration(string duration);
    string getComposer();
    void setComposer(string _composer);
    string getFile();
    void setFile(string _file);
    int getTrackAsInt() const;
    bool operator<(const Song& other) const;
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

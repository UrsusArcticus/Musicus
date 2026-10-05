#include "Library.h"
#include "Song.h"
#include "ID3Tag.h"
#include "Utils.h"
#include "simdjson.h"

#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

using std::string;
using std::vector;
using musicus::Artist;
using musicus::Album;
using musicus::Song;
using musicus::split_string;
using musicus::ID3Tag;
using musicus::ID3Tag_Field;

void readTagAndStore(Song*);
string guessTrackInfo(string, string);

musicus::Library::Library() {
	this->artists = new vector<Artist*>();
}

bool has_audiofile_ending(string f) {
	string audio_file_extensions[] = {"mp3", "flac", "wma", "wav"};
	string extension = musicus::get_file_extension(f);
	for (string e : audio_file_extensions) {
		if (e == extension) {
			return true;
		}
	}
	return false;
}

bool musicus::Library::initializeNewLibrary(string root_directory) {
	this->root_directory = root_directory;
	for (const auto & entry : fs::recursive_directory_iterator(root_directory)) {
		string f = entry.path();
		if (has_audiofile_ending(f)) {
			this->addSong(this->createSongFromFile(f));
		}
	}
    std::sort(artists->begin(), artists->end());
    sortLibrary();
    this->status = LibraryStatus::MODIFIED;
    return true;
}

Song* musicus::Library::createSongFromFile(string audio_file) {
	Song* t = new Song();
	t->setFile(audio_file);
	readTagAndStore(t);
	string relative_audiofile_path = audio_file.substr(this->root_directory.length() + 1);
	if ( t->getSongName() == "" ) {
		t->setSongName(guessTrackInfo("title", relative_audiofile_path));
	}
	if ( t->getAlbum() == "" ) {
		t->setAlbum(guessTrackInfo("album", relative_audiofile_path));
	}
	if ( t->getArtist() == "" ) {
		t->setArtist(guessTrackInfo("artist", relative_audiofile_path));
	}
	return t;
}

void musicus::Library::readTagAndStore(Song* t) {
    ID3Tag tag(t->getFile());
    t->setArtist(tag.getField(ID3Tag_Field::ARTIST));
    t->setAlbum(tag.getField(ID3Tag_Field::ALBUM));
    t->setSongName(tag.getField(ID3Tag_Field::TITLE));
    t->setTrack(tag.getField(ID3Tag_Field::TRACK));
    t->setComposer(tag.getField(ID3Tag_Field::COMPOSER));
    if ( durationRequestHandler != nullptr ) {
        double duration = durationRequestHandler(t->getFile());
        t->setDuration( duration == 0 ? "" : create_timestring(duration) );
    }
}

// Use File name for Track Info if an Id3 Tag is not available or can not be read
string guessTrackInfo(string field, string file_path) {
	vector<string> path_parts = split_string(file_path, "/");
	if ( field == "artist" && path_parts.size() >= 3 ) {
		return path_parts[path_parts.size() - 3];
	} else if ( field == "album" && path_parts.size() >= 2 ) {
		return path_parts[path_parts.size() - 2];
	} else if ( field == "title" && path_parts.size() >= 1 ) {
		string file_name = path_parts[path_parts.size() - 1];
		return split_string(file_name, ".")[0];
	}
	return "Unknown";
}

void musicus::Library::addSong(Song *t) {
	for (auto artist : *this->artists) {
		if (t->getArtist() == artist->getName()) {
			for (auto album: *artist->getAlbums()) {
				if ( album->getName() == t->getAlbum() ) {
					album->addSong(t);
					return;
				}
			}
			Album *newAlbum = new Album(t->getAlbum());
			artist->addAlbum(newAlbum);
			return;
		}
	}
	Album *newAlbum = new Album(t->getAlbum());
	newAlbum->addSong(t);
	Artist *newArtist = new Artist(t->getArtist());
	newArtist->addAlbum(newAlbum);
	this->artists->push_back(newArtist);
}

musicus::LibraryStatus musicus::Library::getLibraryStatus() {
	return status;
}

int musicus::Library::getArtistCount() {
	return (int)artists->size();
}

string musicus::Library::getArtistNameAt(int i) {
	return artists->at(i)->getName();
}

string musicus::Library::getSongNameAt(int i) {
	if ( selectedAlbum && i < selectedAlbum->getSongs()->size() ) {
		return selectedAlbum->getSongs()->at(i)->getSongName();
	}
	return "";
}

string musicus::Library::getSongTrackAt(int i) {
    if ( selectedAlbum && i < selectedAlbum->getSongs()->size() ) {
        return selectedAlbum->getSongs()->at(i)->getTrack();
    }
    return "";
}

string musicus::Library::getSongDurationAt(int i) {
    if ( selectedAlbum && i < selectedAlbum->getSongs()->size() ) {
        return selectedAlbum->getSongs()->at(i)->getDuration();
    }
    return "";
}

void musicus::Library::setLibraryReadyEventHandler(void(*handler)()) {
	libraryReadyHandler = handler;
}

void musicus::Library::setLibraryLoadingEventHandler(void(*handler)()) {
	libraryLoadingHandler = handler;
}

void musicus::Library::selectArtistAt(int index) {
	if (index < artists->size()) {
		selectedArtist = artists->at(index);
		if (artistSelectedHandler) {
			artistSelectedHandler();
		}
	}
}

void musicus::Library::selectAlbumAt(int index) {
	if (selectedArtist != nullptr && index < selectedArtist->getAlbums()->size()) {
		selectedAlbum = selectedArtist->getAlbums()->at(index);
		if (albumSelectedHandler) {
			albumSelectedHandler();
		}
	}
}

void musicus::Library::selectSongAt(int index) {
	if (selectedAlbum != nullptr && index < selectedAlbum->getSongs()->size()) {
		selectedSong = selectedAlbum->getSongs()->at(index);
		if (songSelectedHandler) {
			songSelectedHandler(selectedSong);
		}
	}
}

Song* musicus::Library::getSongOfSelectedAlbumAt(int index) {
	if (selectedAlbum != nullptr && index < selectedAlbum->getSongs()->size()) {
		return selectedAlbum->getSongs()->at(index);
	}
	return nullptr;
}

string musicus::Library::getAlbumNameAt(int index) {
	if (selectedArtist != nullptr && index < selectedArtist->getAlbums()->size()) {
		return selectedArtist->getAlbums()->at(index)->getName();
	}
	return "";
}

bool musicus::Library::hasSelectedArtist() {
	return selectedArtist != nullptr;
}

bool musicus::Library::hasSelectedAlbum() {
	return selectedAlbum != nullptr;
}

Artist* musicus::Library::getSelectedArtist() {
	return selectedArtist;
}

Album* musicus::Library::getSelectedAlbum() {
	return selectedAlbum;
}

bool musicus::Library::hasSelectedSong() {
    return selectedSong != nullptr;
}

void musicus::Library::onArtistSelect(std::function<void()> handler) {
	this->artistSelectedHandler = handler;
}

void musicus::Library::onAlbumSelect(std::function<void()> handler) {
	this->albumSelectedHandler = handler;
}

void musicus::Library::onSongSelect(std::function<void(Song*)> handler) {
	this->songSelectedHandler = handler;
}

void musicus::Library::setDurationRequestHandler(std::function<double(string)> handler) {
    this->durationRequestHandler = handler;
}

bool musicus::Library::loadLibrary(string root_directory) {
    if (this->status != LibraryStatus::EMPTY) return false;
    simdjson::ondemand::parser parser;
    string fp = root_directory + "/.metadata.musicuslib";
    auto text = simdjson::padded_string::load(fp);
    auto document = parser.iterate(text);
    auto root_object = document.get_object();
    auto data = root_object.find_field("data");
    for (auto artist_object : data.get_array()) {
        Artist *artist = new Artist(string{artist_object.find_field("n").get_string().value()});
        for (auto album_object : artist_object.find_field("a").get_array()) {
            Album *album = new Album(string{album_object.find_field("n").get_string().value()});
            auto songs_array = album_object.find_field("s").get_array();
            for (auto song_object : songs_array) {
                Song *song = new Song();
                song->setSongName(string{song_object.find_field("n").get_string().value()});
                song->setArtist(artist->getName());
                song->setAlbum(album->getName());
                song->setFile(string{song_object.find_field("f").get_string().value()});
                song->setTrack(string{song_object.find_field("t").get_string().value()});
                song->setDuration(string{song_object.find_field("d").get_string().value()});
                song->setComposer(string{song_object.find_field("c").get_string().value()});
                album->addSong(song);
            }
            artist->addAlbum(album);
        }
        artists->push_back(artist);
    }
    return true;
}

bool musicus::Library::saveLibrary() {
    if ( this->status != LibraryStatus::MODIFIED) return false;
    simdjson::builder::string_builder builder;
    builder.start_object();
    builder.escape_and_append_with_quotes("data");
    builder.append_colon();
    builder.start_array();
    for ( auto it_a = artists->cbegin(); it_a != artists->cend(); ++it_a) {
        builder.start_object();
        builder.append_key_value("n", (*it_a)->getName());
        builder.append_comma();
        builder.escape_and_append_with_quotes("a");
        builder.append_colon();
        builder.start_array();
        auto albums = (*it_a)->getAlbums();
        for (auto it_al = albums->cbegin(); it_al != albums->cend(); ++it_al) {
            builder.start_object();
            builder.append_key_value("n", (*it_al)->getName());
            builder.append_comma();
            builder.escape_and_append_with_quotes("s");
            builder.append_colon();
            builder.start_array();
            auto songs = (*it_al)->getSongs();
            for (auto it_s = songs->cbegin(); it_s != songs->cend(); ++it_s ) {
                builder.start_object();
                builder.append_key_value("n", (*it_s)->getSongName());
                builder.append_comma();
                builder.append_key_value("f", (*it_s)->getFile());
                builder.append_comma();
                builder.append_key_value("t", (*it_s)->getTrack());
                builder.append_comma();
                builder.append_key_value("d", (*it_s)->getDuration());
                builder.append_comma();
                builder.append_key_value("c", (*it_s)->getComposer());
                builder.end_object();
                if ( next(it_s) != songs->cend() ) {
                    builder.append_comma();
                }
            }
            builder.end_array();
            builder.end_object();
            if ( next(it_al) != albums->cend() ) {
                builder.append_comma();
            }
        }
        builder.end_array();
        builder.end_object();
        if ( next(it_a) != artists->cend() ) {
            builder.append_comma();
        }
    }
    builder.end_array();
    builder.end_object();
    std::string_view contents{builder};
    string fp = root_directory.append("/.metadata.musicuslib");
    std::ofstream file{fp};
    file << string{contents.begin(), contents.end()};
    file.close();
    this->status = LibraryStatus::LOADED;
    return true;
}

musicus::SongImage musicus::Library::loadSongImage(Song *t) {
    ID3Tag tag(t->getFile());
    ID3_PictureFrame *frame = tag.getPicture();
    SongImage img;
    if ( frame != nullptr ) {
        img.data = frame->data;
        img.size = frame->size;
    } else {
        img.size = 0;
    }
    return img;
}

void musicus::Library::sortLibrary() {
    std::sort(artists->begin(), artists->end(), [](const Artist* a, const Artist* b) { return *a < *b; });
    std::for_each(artists->begin(), artists->end(), [](auto& x) { x->sortAlbumsAndSongs(); });
}

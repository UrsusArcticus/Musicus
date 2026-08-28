#include "Library.h"
#include "Song.h"
#include "ID3Tag.h"
#include "Utils.h"
#include "simdjson.h"

#include <string>
#include <filesystem>
#include <vector>
#include <fstream>

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
	string ending = musicus::get_file_ending(f);
	for (string ext : audio_file_extensions) {
		if (ext == ending) {
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

void readTagAndStore(Song* t) {
    ID3Tag tag(t->getFile());
    t->setArtist(tag.getFieldAsUtf8(ID3Tag_Field::ARTIST));
    t->setAlbum(tag.getFieldAsUtf8(ID3Tag_Field::ALBUM));
    t->setSongName(tag.getFieldAsUtf8(ID3Tag_Field::TITLE));
    t->setTrack(tag.getField(ID3Tag_Field::TRACK));
    t->setComposer(tag.getFieldAsUtf8(ID3Tag_Field::COMPOSER));
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
					album->addTrack(t);
					return;
				}
			}
			Album *newAlbum = new Album(t->getAlbum());
			artist->addAlbum(newAlbum);
			return;
		}
	}
	Album *newAlbum = new Album(t->getAlbum());
	newAlbum->addTrack(t);
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

bool musicus::Library::loadLibrary(string root_directory) {
    if (this->status != LibraryStatus::EMPTY) return false;
    simdjson::ondemand::parser parser;
    string fp = root_directory.append("/.metadata.musicuslib");
    auto text = simdjson::padded_string::load(fp);
    auto document = parser.iterate(text);
    auto root_object = document.get_object();
    auto data = root_object.find_field("data");
    for (auto artist_object : data.get_array()) {
        Artist *artist = new Artist(string{artist_object.find_field("n").get_string().value()});
        for (auto album_object : artist_object.find_field("a").get_array()) {
            Album *album = new Album(string{album_object.find_field("n").get_string().value()});
            for (auto song_object : album_object.find_field("s").get_array()) {
                Song *song = new Song();
                song->setSongName(string{song_object.find_field("n").get_string().value()});
                song->setFile(string{song_object.find_field("f").get_string().value()});
                song->setTrack(string{song_object.find_field("t").get_string().value()});
                song->setComposer(string{song_object.find_field("c").get_string().value()});
                album->addTrack(song);
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
    for (auto artist : *artists) {
        builder.start_object();
        builder.append_key_value("n", artist->getName());
        builder.append_comma();
        builder.escape_and_append_with_quotes("a");
        builder.append_colon();
        builder.start_array();
        for (auto album : *artist->getAlbums()) {
            builder.start_object();
            builder.append_key_value("n", album->getName());
            builder.append_comma();
            builder.escape_and_append_with_quotes("s");
            builder.append_colon();
            builder.start_array();
            for (auto song : *album->getSongs()) {
                builder.start_object();
                builder.append_key_value("n", song->getSongName());
                builder.append_comma();
                builder.append_key_value("f", song->getFile());
                builder.append_comma();
                builder.append_key_value("t", song->getTrack());
                builder.append_comma();
                builder.append_key_value("c", song->getComposer());
                builder.end_object();
                builder.append_comma();
            }
            builder.end_array();
            builder.append_comma();
        }
        builder.end_array();
        builder.append_comma();
    }
    builder.end_array();
    builder.end_object();
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
    img.data = frame->data;
    img.size = frame->size;
    return img;
}

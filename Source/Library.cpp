#include "Library.h"
#include "Track.h"
#include "ID3Tag.h"
#include "Utils.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

using std::vector;
using musicus::Artist;
using musicus::Album;
using musicus::Track;
using musicus::split_string;
using musicus::ID3Tag;
using musicus::ID3Tag_Field;

void readTagAndStore(Track*);
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
			this->addTrack(this->createTrackFromFile(f));
		}
	}
    return true;
}

bool musicus::Library::loadLibrary(string root_directory) {
	return true;
}

Track* musicus::Library::createTrackFromFile(string audio_file) {
	Track* t = new Track();
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

void readTagAndStore(Track* t) {
    ID3Tag tag(t->getFile());
    t->setArtist(tag.getFieldAsUtf8(ID3Tag_Field::ARTIST));
    t->setAlbum(tag.getFieldAsUtf8(ID3Tag_Field::ALBUM));
    t->setSongName(tag.getFieldAsUtf8(ID3Tag_Field::TITLE));
    t->setTrack(tag.getField(ID3Tag_Field::TRACK));
    t->setComposer(tag.getFieldAsUtf8(ID3Tag_Field::COMPOSER));
}

/*
void readTagAndStore(Track* t) {
	ID3_Tag tag(file.c_str());
	ID3_Frame* frame = tag.Find(ID3FID_COMPOSER);
	if (frame) {
        ID3_Field* field = frame->GetField(ID3FN_TEXT);
        char *str;
        field->Get(str, 1024);
        if ( !str ) {
        	std::cout << "str ist null" << std::endl;
        }

        std::cout << file << std::endl;
        // use an std::auto_ptr here to handle object cleanup automatically
		ID3_Frame::Iterator* field_iter = frame->CreateIterator();
		ID3_Field* myField = NULL;
		while (NULL != (myField = field_iter->GetNext()))
		{
			std::cout << "Test\n\n" << std::endl;
			if ( myField->GetRawText() != NULL ) {
		    	std::cout << myField->GetRawText() << std::endl;
			}
	    }
	   	delete field_iter;
    }
    ID3_Tag::Iterator* iter = tag.CreateIterator();
    ID3_Frame* frame = NULL;
    while (NULL != (frame = iter->GetNext())) {
    	std::cout << "process field" << std::endl;
    }
    delete iter;
}
*/

// Use File name for Track Info if an Id3 Tag is not available
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

void musicus::Library::addTrack(Track *t) {
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
	if ( selectedAlbum && i < selectedAlbum->getTracks()->size() ) {
		return selectedAlbum->getTracks()->at(i)->getSongName();
	}
	return "";
}

string musicus::Library::getSongTrackAt(int i) {
    if ( selectedAlbum && i < selectedAlbum->getTracks()->size() ) {
        return selectedAlbum->getTracks()->at(i)->getTrack();
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
	if (selectedAlbum != nullptr && index < selectedAlbum->getTracks()->size()) {
		selectedSong = selectedAlbum->getTracks()->at(index);
		if (songSelectedHandler) {
			songSelectedHandler(selectedSong);
		}
	}
}

Track* musicus::Library::getSongOfSelectedAlbumAt(int index) {
	if (selectedAlbum != nullptr && index < selectedAlbum->getTracks()->size()) {
		return selectedAlbum->getTracks()->at(index);
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

void musicus::Library::onSongSelect(std::function<void(Track*)> handler) {
	this->songSelectedHandler = handler;
}

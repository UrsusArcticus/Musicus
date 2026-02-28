#pragma once

#include <vector>
#include <functional>
#include "Artist.h"

namespace musicus {

	enum LibraryStatus {
		EMPTY,
		LOADING,
		READY
	};
	
	class Library {
	private:
		string root_directory;
		vector<Artist*>* artists;
		LibraryStatus status = LibraryStatus::EMPTY;
		Artist* selectedArtist = nullptr;
		Album* selectedAlbum = nullptr;
		Track* selectedSong = nullptr;
		void(*libraryReadyHandler)();
		void(*libraryLoadingHandler)();
		std::function<void()> artistSelectedHandler;
		std::function<void()> albumSelectedHandler;
		std::function<void(Track*)> songSelectedHandler;

		Track* createTrackFromFile(string);
		void addTrack(Track *t);

	public:
		Library();

		bool initializeNewLibrary(string root_directory);

		bool loadLibrary(string root_directory);

		bool saveLibrary();

		LibraryStatus getLibraryStatus();

		int getArtistCount();

		string getArtistNameAt(int);

		void setLibraryReadyEventHandler(void(*handler)());

		void setLibraryLoadingEventHandler(void(*handler)());

		void selectArtistAt(int);

		void selectAlbumAt(int);

		void selectSongAt(int);

		Track* getSongOfSelectedAlbumAt(int);

		string getAlbumNameAt(int);

		string getSongNameAt(int);
        
        string getSongTrackAt(int);

		bool hasSelectedArtist();

		bool hasSelectedAlbum();

		Artist* getSelectedArtist();

		Album* getSelectedAlbum();

		Track* getSelectedSong();

		bool hasSelectedSong();

		void onArtistSelect(std::function<void()> handler);

		void onAlbumSelect(std::function<void()> handler);

		void onSongSelect(std::function<void(Track*)> handler);
	};
}

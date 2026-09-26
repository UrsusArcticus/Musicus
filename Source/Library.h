#pragma once

#include <vector>
#include <functional>
#include "Artist.h"

namespace musicus {

	enum LibraryStatus {
		EMPTY,
		LOADING,
		MODIFIED,
        LOADED
	};
	
	class Library {
	private:
		string root_directory;
		vector<Artist*>* artists;
		LibraryStatus status = LibraryStatus::EMPTY;
		Artist* selectedArtist = nullptr;
		Album* selectedAlbum = nullptr;
		Song* selectedSong = nullptr;
		void(*libraryReadyHandler)();
		void(*libraryLoadingHandler)();
		std::function<void()> artistSelectedHandler;
		std::function<void()> albumSelectedHandler;
		std::function<void(Song*)> songSelectedHandler;
        std::function<double(string)> durationRequestHandler;

		Song* createSongFromFile(string);
		void addSong(Song*);
        void readTagAndStore(Song*);

	public:
		Library();

		bool initializeNewLibrary(string root_directory);

		LibraryStatus getLibraryStatus();

		int getArtistCount();

		string getArtistNameAt(int);

		void setLibraryReadyEventHandler(void(*handler)());

		void setLibraryLoadingEventHandler(void(*handler)());

		void selectArtistAt(int);

		void selectAlbumAt(int);

		void selectSongAt(int);

		Song* getSongOfSelectedAlbumAt(int);

		string getAlbumNameAt(int);

		string getSongNameAt(int);
        
        string getSongTrackAt(int);
        
        string getSongDurationAt(int);

		bool hasSelectedArtist();

		bool hasSelectedAlbum();

		Artist* getSelectedArtist();

		Album* getSelectedAlbum();

		Song* getSelectedSong();

		bool hasSelectedSong();

		void onArtistSelect(std::function<void()> handler);

		void onAlbumSelect(std::function<void()> handler);

		void onSongSelect(std::function<void(Song*)> handler);
        
        void setDurationRequestHandler(std::function<double(string)> handler);
        
        bool loadLibrary(string root_directory);

        bool saveLibrary();
        
        SongImage loadSongImage(Song *);
        
    };
}

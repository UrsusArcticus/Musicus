#pragma once

#include <string>

using std::string;

namespace musicus {
	
	class Track {
	private:
		string song_name;
		string artist;
		string album;
		string file;

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

		string getFile() {
			return file;
		}

		void setFile(string _file) {
			this->file = _file;
		}
	};

}
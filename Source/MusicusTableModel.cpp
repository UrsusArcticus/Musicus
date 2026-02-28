#include "MusicusTableModel.h"

void musicus::MusicusTableModel::setColours(juce::Colour textColour, juce::Colour backgroundColor) {
    this->textColour = textColour;
    this->backgroundColor = backgroundColor;
}

void musicus::MusicusTableModel::setLibrary(musicus::Library* library) {
	this->library = library;
}

int musicus::MusicusTableModel::getNumRows() {
    if (library == nullptr) {
    	return 0;
    }
    switch (type) {
        case musicus::MusicusTableType::ARTISTS:
            return library->getArtistCount();
        case musicus::MusicusTableType::ALBUMS:
            if (library->hasSelectedArtist()) {
                return (int)library->getSelectedArtist()->getAlbums()->size();
            }
            break;
        case musicus::MusicusTableType::SONGS: 
            if (library->hasSelectedAlbum()) {
                return (int)library->getSelectedAlbum()->getTracks()->size();
            }
    }
    return 0;
}

void musicus::MusicusTableModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    g.setColour (rowIsSelected ? juce::Colours::darkblue : this->textColour);
    g.setFont (font);
    switch (type) {
        case musicus::MusicusTableType::ARTISTS:
            g.drawText (library->getArtistNameAt(rowNumber), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
            break;
        case musicus::MusicusTableType::ALBUMS:
            if (library->hasSelectedArtist()) {
                g.drawText (library->getAlbumNameAt(rowNumber), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
            }
            break;
        case musicus::MusicusTableType::SONGS:
            if (library->hasSelectedAlbum()) {
                if (columnId == 1) {
                    g.drawText(library->getSongTrackAt(rowNumber), 2, 0, width - 4, height,
                               juce::Justification::centredLeft, true);
                } else {
                    g.drawText (library->getSongNameAt(rowNumber), 2, 0, width - 4, height,
                                juce::Justification::centredLeft, true);
                }
            }
    }
    //g.setColour (this->backgroundColor);
    //g.fillRect (width - 1, 0, 1, height);
}

void musicus::MusicusTableModel::paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) {
    if (rowIsSelected)
        g.fillAll (juce::Colours::lightblue);
    else
        g.fillAll(this->backgroundColor);
}

void musicus::MusicusTableModel::setType(MusicusTableType type) {
    this->type = type;
}

void musicus::MusicusTableModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent &e) {
    if (library == nullptr) {
        return;
    }
    switch (type) {
        case musicus::MusicusTableType::ARTISTS:
            library->selectArtistAt(rowNumber);
            break;
        case musicus::MusicusTableType::ALBUMS:
            library->selectAlbumAt(rowNumber);
            break;
        case musicus::MusicusTableType::SONGS:
            library->selectSongAt(rowNumber);
            break;
    }
}

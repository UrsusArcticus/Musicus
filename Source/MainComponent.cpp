#include "MainComponent.h"
#include <iostream>

using std::unique_ptr;
using std::make_unique;

using juce::FileChooser;
using juce::FileBrowserComponent;
using juce::Image;
using juce::ImageCache;

using namespace musicus;

//==============================================================================
MainComponent::MainComponent() {
    addAndMakeVisible(createNewLibBtn);
    musicLibrary = new Library();
    musicLibrary->onSongSelect([this](Song* s) { this->playSong(s); });
    createNewLibBtn.setButtonText ("Create new Library");
    createNewLibBtn.onClick = [this] { createNewLibrary(); };
    setSize (600, 400);
    formatManager.registerBasicFormats();

    juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                         [this] (bool granted)
                                         {
                                             int numInputChannels = granted ? 2 : 0;
                                             audioDeviceManager.initialise (numInputChannels, 2, nullptr, true, {}, nullptr);
                                         });

    audioDeviceManager.addAudioCallback (&audioSourcePlayer);
    audioSourcePlayer.setSource (&transportSource);
}

MainComponent::~MainComponent() {
    delete artistTableModel;
    delete albumTableModel;
    delete songTableModel;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g) {
    /*if ( musicLibrary->hasSelectedSong() ) {
        g.setColour(juce::Colour(100, 100, 100));
        g.fillRect(0, 0, 200, PLAYER_PANEL_HEIGHT);
    }*/
}

void MainComponent::resized() {
    if (view == MusicusViewType::EMPTY) {
        createNewLibBtn.setBounds(getBounds().getWidth() > 200 ? getBounds().getWidth() / 2 - 100 : 10,
            (getBounds().getHeight() / 2) - 5,
            getBounds().getWidth() > 200 ? 200 : getBounds().getWidth() - 20, 50);
    }
    if (view == MusicusViewType::LIBRARY) {
        int artistTableWidth = getBounds().getWidth() * 0.2;
        int albumTableWidth = getBounds().getWidth() * 0.5;
        int songTableWidth = getBounds().getWidth() * 0.3;
        int tableHeights = PLAYER_PANEL_HEIGHT + getBounds().getHeight();

        artistTable.setBounds(1, PLAYER_PANEL_HEIGHT, artistTableWidth, tableHeights);
        albumTable.setBounds(artistTableWidth + 1, PLAYER_PANEL_HEIGHT, albumTableWidth, tableHeights);
        songTable.setBounds(artistTableWidth + albumTableWidth + 2, PLAYER_PANEL_HEIGHT, songTableWidth, tableHeights);
        songImage.setBounds(0, 0, 200, PLAYER_PANEL_HEIGHT);
    }
}

void MainComponent::set_library_created_handler(std::function<void(string)> f) {
    this->library_created_event = f;
}

void MainComponent::showLibraryView() {
    addAndMakeVisible (artistTable);
    artistTableModel = new MusicusTableModel(MusicusTableType::ARTISTS);
    artistTableModel->setColours(getLookAndFeel().findColour(juce::ListBox::textColourId),
                                    getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    artistTableModel->setLibrary(musicLibrary);
    auto artistSelectionHandler = [this]() {
        this->albumTable.updateContent();
        this->albumTable.repaint();
    };
    musicLibrary->onArtistSelect(artistSelectionHandler);
    artistTable.getHeader().addColumn("Artist", 1, 200);
    artistTable.setModel(artistTableModel);

    addAndMakeVisible (albumTable);
    albumTableModel = new MusicusTableModel(MusicusTableType::ALBUMS);
    albumTableModel->setColours(getLookAndFeel().findColour(juce::ListBox::textColourId),
                                    getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    albumTableModel->setLibrary(musicLibrary);
    auto albumSelectionHandler = [this]() {
        this->songTable.updateContent();
        this->songTable.repaint();
    };
    musicLibrary->onAlbumSelect(albumSelectionHandler);
    albumTable.getHeader().addColumn("Album", 1, 200);
    albumTable.setModel(albumTableModel);

    addAndMakeVisible (songTable);
    songTableModel = new MusicusTableModel(MusicusTableType::SONGS);
    songTableModel->setColours(getLookAndFeel().findColour(juce::ListBox::textColourId),
                                    getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    songTableModel->setLibrary(musicLibrary);
    songTable.getHeader().addColumn("Track", 1, 40);
    songTable.getHeader().addColumn("Song Name", 2, 200);
    //songTable.getHeader().addColumn("Duration", 3, 100);
    songTable.setModel(songTableModel);
    addAndMakeVisible(songImage);

    view = MusicusViewType::LIBRARY;
    resized();
}

void MainComponent::createNewLibrary() {
    fc.reset (new FileChooser("Please select your music library home directory",
                                        juce::File::getSpecialLocation(juce::File::userHomeDirectory), 
                                        "*", false)); //true (use native look&feel) does not work on Linux with KDE
    auto librarySelectCallback = [this] (const FileChooser& chooser) {
        auto result = chooser.getURLResult();
        auto path = result.isLocalFile() ? result.getLocalFile().getFullPathName() : result.toString (true);
        if (path.compare("")) {
            removeChildComponent(&createNewLibBtn);
            musicLibrary->initializeNewLibrary(path.toStdString());
            if ( this->library_created_event != nullptr ) {
                library_created_event(path.toStdString());
            }
            showLibraryView();
        }
    };
    fc->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories, librarySelectCallback);
}

void MainComponent::playSong(Song* t) {
    addAndMakeVisible(songLabel);
    songLabel.setFont (juce::Font (20.0f, juce::Font::bold));
    songLabel.setBounds (205, 110, getWidth() - 205, 20);
    songLabel.setJustificationType (juce::Justification::centredLeft);
    songLabel.setText(juce::String(t->getSongName()), juce::dontSendNotification);

    addAndMakeVisible(pauseButton);
    pauseButton.setButtonText("Pause");
    pauseButton.setBounds (205, 140, 70, 20);
    
    SongImage imgData = musicLibrary->loadSongImage(t);
    Image image = ImageCache::getFromMemory(imgData.data, imgData.size);
    songImage.setImage(image);

    this->repaint();

    auto* reader = formatManager.createReaderFor (juce::File(juce::String(t->getFile())));
    if (reader != nullptr) {
        std::cout << reader->getFormatName() << std::endl;
        auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
        transportSource.start();
    }
}


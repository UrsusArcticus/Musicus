#include "MainComponent.h"
#include "Utils.h"

#include <iostream>
#include <filesystem>

using std::unique_ptr;
using std::make_unique;
using std::filesystem::directory_iterator;

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
    tm.setCallback([this](){this->updateSongView();});
    stopButton.setButtonText("Stop");
    addChildComponent(stopButton);
    musicLibrary->setDurationRequestHandler([this](string file) {
        auto* reader = formatManager.createReaderFor (juce::File(juce::String(file)));
        if (reader != nullptr) {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
            juce::AudioTransportSource transportSource;
            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource.reset (newSource.release());
            return transportSource.getLengthInSeconds();
        }
        return 0.0;
    });
}

MainComponent::~MainComponent() {
    delete artistTableModel;
    delete albumTableModel;
    delete songTableModel;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g) {
    if ( musicLibrary->hasSelectedSong() ) {
        paintProgressBar(g);
    }
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
        if (musicLibrary->hasSelectedSong()) {
            songLength.setBounds(getBounds().getWidth() - 35, 160, 30, 14);
            this->repaint();
        }
    }
}

void MainComponent::set_library_created_handler(std::function<void(string)> f) {
    this->library_created_event = f;
}

void MainComponent::showLibraryView() {
    removeChildComponent(&createNewLibBtn);
    addAndMakeVisible (artistTable);
    artistTableModel = new MusicusTableModel(MusicusTableType::ARTISTS);
    artistTableModel->setColours(getLookAndFeel().findColour(juce::ListBox::textColourId),
                                    getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    artistTableModel->setLibrary(musicLibrary);
    auto artistSelectionHandler = [this]() {
        this->albumTable.deselectAllRows();
        this->songTable.deselectAllRows();
        this->albumTable.updateContent();
        this->albumTable.repaint();
        this->songTable.repaint();
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
        this->songTable.deselectAllRows();
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
    songTable.getHeader().addColumn("Duration", 3, 100);
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
    songLabel.setBounds (205, 90, getWidth() - 205, 20);
    songLabel.setJustificationType (juce::Justification::centredLeft);
    songLabel.setText(juce::String(t->getSongName()), juce::dontSendNotification);
    addAndMakeVisible(trackLabel);
    trackLabel.setFont(juce::Font(8.0f));
    trackLabel.setBounds(205, 110, 50, 8);
    trackLabel.setText(juce::String(t->getTrack()), juce::dontSendNotification);
    addAndMakeVisible(albumLabel);
    albumLabel.setFont(juce::Font(12.0f));
    albumLabel.setBounds(205, 125, 300, 14);
    albumLabel.setJustificationType(juce::Justification::centredLeft);
    albumLabel.setText(juce::String(t->getAlbum()), juce::dontSendNotification);
    addAndMakeVisible(artistLabel);
    artistLabel.setFont (juce::Font(12.0f));
    artistLabel.setBounds(205, 140, 300, 14);
    artistLabel.setJustificationType(juce::Justification::centredLeft);
    artistLabel.setText(juce::String(t->getArtist()), juce::dontSendNotification);
    addAndMakeVisible(currentTime);
    currentTime.setFont(juce::Font(12.0f));
    currentTime.setBounds(205, 160, 50, 14);
    currentTime.setText("0:00", juce::dontSendNotification);
    addAndMakeVisible(songLength);
    songLength.setFont(juce::Font(12.0f));
    
    addAndMakeVisible(pauseButton);
    pauseButton.setButtonText("Pause");
    pauseButton.setBounds (205, 175, 70, 20);
    pauseButton.onClick = [this] {
        if ( s_status == SongStatus::PLAYING ) {
            this->transportSource.stop();
            pauseButton.setButtonText("Play");
            s_status = SongStatus::PAUSED;
            tm.stop();
        }
        else if ( s_status == SongStatus::PAUSED || s_status == SongStatus::STOPPED ) {
            transportSource.start();
            pauseButton.setButtonText("Pause");
            tm.start();
            s_status = SongStatus::PLAYING;
            stopButton.setVisible(true);
        }
    };
    stopButton.setVisible(true);
    stopButton.setBounds (275, 175, 70, 20);
    stopButton.onClick = [this] {
        transportSource.stop();
        transportSource.setPosition(0);
        s_status = SongStatus::STOPPED;
        tm.stop();
        currentTime.setText("00:00", juce::dontSendNotification);
        pauseButton.setButtonText("Play");
        stopButton.setVisible(false);
        repaint();
    };

    loadImage(t);
    
    this->repaint();

    auto* reader = formatManager.createReaderFor (juce::File(juce::String(t->getFile())));
    if (reader != nullptr) {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
        transportSource.start();
        s_status = SongStatus::PLAYING;
        songLength.setText(create_timestring(transportSource.getLengthInSeconds()), juce::dontSendNotification);
        this->resized();
        tm.start();
    }
}

void MainComponent::loadImage(Song* t) {
    SongImage imgData = musicLibrary->loadSongImage(t);
    if ( imgData.size == 0 ) {
        std::list<string> fn{"Cover", "cover", "Front", "front", "Folder", "folder"};
        std::list<string> image_files;
        for( auto& entry : directory_iterator(get_directory(t->getFile()))) {
            if ( has_image_extension(entry.path()) ) { image_files.push_back(entry.path()); }
        }
        auto r = std::find_if(image_files.begin(), image_files.end(), [fn] (string f) {
            auto r = std::find(fn.begin(), fn.end(), get_filename_without_extension(f));
            return r != fn.end();
        });
        if ( r != image_files.end() ) {
            songImage.setImage(ImageCache::getFromFile(juce::File(*r)));
        } else {
            if (image_files.size() > 0) {
                songImage.setImage(ImageCache::getFromFile(juce::File(image_files.front())));
            }
        }
    } else {
        Image image = ImageCache::getFromMemory(imgData.data, imgData.size);
        songImage.setImage(image);
    }
}

void MainComponent::paintProgressBar(juce::Graphics& g) {
    g.setColour(juce::Colours::aqua);
    int x = 240;
    int y = 168;
    int x2 = getBounds().getWidth() - 40;
    int l = x2 - x;
    int p = ((transportSource.getCurrentPosition() * 100) / transportSource.getLengthInSeconds()) * l / 100;
    g.drawLine(x, y, x + p, y, 6);
}

void MainComponent::updateSongView() {
    if ( transportSource.isPlaying() ) {
        currentTime.setText(create_timestring(transportSource.getCurrentPosition()), juce::dontSendNotification);
        repaint();
    }
    if ( transportSource.hasStreamFinished() ) {
        s_status = SongStatus::STOPPED;
        tm.stop();
    }
}


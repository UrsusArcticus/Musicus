#pragma once

#include <functional>
#include <JuceHeader.h>
#include "Library.h"
#include "MusicusTableModel.h"
#include "Config.h"

enum MusicusViewType {
    EMPTY,
    LIBRARY
};

class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void set_library_created_handler(std::function<void(string)> f);
    musicus::Library* getMusicLibrary() { return musicLibrary; }

private:
    juce::TableListBox artistTable;
    musicus::MusicusTableModel* artistTableModel;
    juce::TableListBox albumTable;
    musicus::MusicusTableModel* albumTableModel;
    juce::TableListBox songTable;
    musicus::MusicusTableModel* songTableModel;
    juce::TextButton createNewLibBtn;
    std::unique_ptr<juce::FileChooser> fc;
    MusicusViewType view = MusicusViewType::EMPTY;
    juce::Label songLabel;
    juce::Label artistLabel;
    juce::TextButton pauseButton;
    juce::TextButton stopButton;
    juce::ImageComponent songImage;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioTransportSource transportSource;
    
    musicus::Library* musicLibrary;
    
    std::function<void(string)> library_created_event = nullptr;

    int PLAYER_PANEL_HEIGHT = 200;

    void createNewLibrary();
    void playSong(musicus::Song* t);
    void showLibraryView();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

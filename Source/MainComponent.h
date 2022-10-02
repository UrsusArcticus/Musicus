#pragma once

#include <JuceHeader.h>
#include "Library.h"
#include "MusicusTableModel.h"

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

private:
    juce::TableListBox artistTable;
    musicus::MusicusTableModel* artistTableModel;
    juce::TableListBox albumTable;
    musicus::MusicusTableModel* albumTableModel;
    juce::TableListBox songTable;
    musicus::MusicusTableModel* songTableModel;
    juce::TextButton createNewLibBtn;
    std::unique_ptr<juce::FileChooser> fc;
    musicus::Library* musicLibrary;
    MusicusViewType view = MusicusViewType::EMPTY;
    juce::Label songLabel;
    juce::Label artistLabel;
    juce::TextButton pauseButton;
    juce::TextButton stopButton;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioTransportSource transportSource;

    int PLAYER_PANEL_HEIGHT = 200;

    void loadConfigFile();
    void createNewLibrary();
    void showLibraryView();
    void playSong(musicus::Track* t);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

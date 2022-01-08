#include <JuceHeader.h>
#include <functional>
#include "Library.h"

namespace musicus {

enum MusicusTableType {
    ARTISTS,
    ALBUMS,
    SONGS
};

class MusicusTableModel : public juce::TableListBoxModel {
private:
    juce::Font font{ 14.0f };
    juce::Colour textColour;
    juce::Colour backgroundColor;
    musicus::Library* library = nullptr;
    MusicusTableType type;
    std::function<void()> rowSelectionHandler;

public:
    MusicusTableModel(MusicusTableType type) {
        this->type = type;
    }

    void setLibrary(musicus::Library* library);

    void setColours(juce::Colour, juce::Colour);

    int getNumRows() override;

    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    void paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;

    void setType(MusicusTableType type);

    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent &) override;

};

}

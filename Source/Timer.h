/*
  ==============================================================================

    Timer.h
    Created: 31 Aug 2026 3:46:37pm
    Author:  Philipp Otte

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace musicus {

class Timer : juce::Timer {
    std::function<void()> cf;
public:
    void setCallback(std::function<void()> f) { cf = f; }
    void timerCallback() override { if (cf) cf(); };
    void start() { startTimer(100); }
    void stop() { stopTimer(); }
};

}

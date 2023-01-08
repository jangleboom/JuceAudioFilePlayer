//
//  PositionOverlay.hpp
//  JuceAudioFilePlayer - App
//
//  Created by Mr. Markuese on 08.01.23.
//

#ifndef PositionOverlay_h
#define PositionOverlay_h

#include <stdio.h>
#include <JuceHeader.h>


class PositionOverlay : public juce::Component,
                              private juce::Timer
{
public:
    PositionOverlay (juce::AudioTransportSource& transportSourceToUse);
    
    ~PositionOverlay() override;

    void paint (juce::Graphics& g) override;
    
    void mouseDown (const juce::MouseEvent& event) override;

private:
    void timerCallback() override;

    juce::AudioTransportSource& transportSource;
    juce::String positionString;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PositionOverlay)
};

#endif /* PositionOverlay_h */

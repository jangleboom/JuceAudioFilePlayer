//
//  PositionOverlay.cpp
//  JuceAudioFilePlayer - App
//
//  Created by Mr. Markuese on 08.01.23.
//

#include "PositionOverlay.h"

PositionOverlay::PositionOverlay (juce::AudioTransportSource& transportSourceToUse)
   : transportSource (transportSourceToUse)
{
    startTimer (40);
}

PositionOverlay::~PositionOverlay()
{
    stopTimer();
}

void PositionOverlay::paint (juce::Graphics& g)
{
    auto duration = (float) transportSource.getLengthInSeconds();

    if (duration > 0.0)
    {
        auto audioPosition = (float) transportSource.getCurrentPosition();
        auto drawPosition = (audioPosition / duration) * (float) getWidth();

        g.setColour (juce::Colours::green);
        g.drawLine (drawPosition, 0.0f, drawPosition, (float) getHeight(), 2.0f);
    }
    
    g.drawFittedText (positionString, getLocalBounds(), juce::Justification::centred, 1);
}

void PositionOverlay::mouseDown (const juce::MouseEvent& event)
{
    auto duration = transportSource.getLengthInSeconds();

    if (duration > 0.0)
    {
        auto clickPosition = event.position.x;
        auto audioPosition = (clickPosition / (float) getWidth()) * duration;

        transportSource.setPosition (audioPosition);
    }
}

void PositionOverlay::timerCallback() 
{
    if (transportSource.isPlaying())
    {
        juce::RelativeTime position (transportSource.getCurrentPosition());

        auto minutes = ((int) position.inMinutes()) % 60;
        auto seconds = ((int) position.inSeconds()) % 60;
        auto millis  = ((int) position.inMilliseconds()) % 1000;
        positionString = juce::String::formatted ("%02d:%02d:%04d", minutes, seconds, millis);

        repaint();
    }
    else
    {
        positionString = "Stopped";
    }
}

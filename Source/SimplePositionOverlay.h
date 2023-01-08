//
//  SimplePositionOverlay.hpp
//  JuceAudioFilePlayer - App
//
//  Created by Mr. Markuese on 08.01.23.
//

#ifndef SimplePositionOverlay_hpp
#define SimplePositionOverlay_hpp

#include <stdio.h>
#include <JuceHeader.h>


class SimplePositionOverlay : public juce::Component,
                              private juce::Timer
{
public:
    SimplePositionOverlay (const juce::AudioTransportSource& transportSourceToUse)
       : transportSource (transportSourceToUse)
    {
        addAndMakeVisible (&currentPositionLabel);
        currentPositionLabel.setText ("Stopped", juce::dontSendNotification);
        startTimer (40);
    }

    void paint (juce::Graphics& g) override
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
    
    


private:
    void timerCallback() override
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
            currentPositionLabel.setText ("Stopped", juce::dontSendNotification);
        }
    }

    const juce::AudioTransportSource& transportSource;
    juce::Label currentPositionLabel;
    juce::String positionString;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimplePositionOverlay)
};

#endif /* SimplePositionOverlay_hpp */

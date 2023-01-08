//
//  SimpleThumbnailComponent.hpp
//  JuceAudioFilePlayer - App
//
//  Created by Mr. Markuese on 08.01.23.
//

#ifndef SimpleThumbnailComponent_h
#define SimpleThumbnailComponent_h

#include <stdio.h>
#include <JuceHeader.h>


class SimpleThumbnailComponent : public juce::Component,
                                 private juce::ChangeListener
{
public:
    SimpleThumbnailComponent (int sourceSamplesPerThumbnailSample,
                              juce::AudioFormatManager& formatManager,
                              juce::AudioThumbnailCache& cache)
       : thumbnail (sourceSamplesPerThumbnailSample, formatManager, cache)
    {
        thumbnail.addChangeListener (this);
    }

    void setFile (const juce::File& file)
    {
        thumbnail.setSource (new juce::FileInputSource (file));
    }

    void paint (juce::Graphics& g) override
    {
        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded (g);
        else
            paintMonoIfFileLoaded (g);
    }

    void paintIfNoFileLoaded (juce::Graphics& g)
    {
        g.fillAll (juce::Colours::darkgrey);
        g.setColour (juce::Colours::white);
        g.drawFittedText ("No File Loaded", getLocalBounds(), juce::Justification::centred, 1);
    }

    void paintIfFileLoaded (juce::Graphics& g)
    {
        g.fillAll (juce::Colours::darkgrey);

        g.setColour (juce::Colours::red);
        thumbnail.drawChannels (g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
    }
    
    void paintMonoIfFileLoaded (juce::Graphics& g)
    {
        g.fillAll (juce::Colours::darkgrey);
        g.setColour (juce::Colours::red);

        thumbnail.drawChannel (g,
                                getLocalBounds(),
                                0.0,                                    // start time
                                thumbnail.getTotalLength(),             // end time
                                0,
                                0.8f);                                  // vertical zoom
        
    }

    void changeListenerCallback (juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            thumbnailChanged();
    }

private:
    void thumbnailChanged()
    {
        repaint();
    }

    juce::AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleThumbnailComponent)
};

#endif /* SimpleThumbnailComponent_h */

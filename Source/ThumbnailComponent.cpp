//
//  ThumbnailComponent.cpp
//  JuceAudioFilePlayer - App
//
//  Created by Mr. Markuese on 08.01.23.
//

#include "ThumbnailComponent.h"

ThumbnailComponent::ThumbnailComponent (int sourceSamplesPerThumbnailSample,
                          juce::AudioFormatManager& formatManager,
                          juce::AudioThumbnailCache& cache)
   : thumbnail (sourceSamplesPerThumbnailSample, formatManager, cache)
{
    thumbnail.addChangeListener (this);
}

ThumbnailComponent::~ThumbnailComponent()
{
    thumbnail.removeChangeListener (this);
}

void ThumbnailComponent::setFile (const juce::File& file)
{
    thumbnail.setSource (new juce::FileInputSource (file));
}

void ThumbnailComponent::paint (juce::Graphics& g)
{
    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded (g);
    else
        paintMonoIfFileLoaded (g);
}

void ThumbnailComponent::paintIfNoFileLoaded (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("No File Loaded", getLocalBounds(), juce::Justification::centred, 1);
}

void ThumbnailComponent::paintIfFileLoaded (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);

    g.setColour (juce::Colours::darkred);
    thumbnail.drawChannels (g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
}

void ThumbnailComponent::paintMonoIfFileLoaded (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::red);

    thumbnail.drawChannel (g,
                            getLocalBounds(),
                            0.0,                                    // start time
                            thumbnail.getTotalLength(),             // end time
                            0,
                            1.0f);                                  // vertical zoom
}

void ThumbnailComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail)
        thumbnailChanged();
}

void ThumbnailComponent::thumbnailChanged()
{
    repaint();
}

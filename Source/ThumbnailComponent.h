//
//  ThumbnailComponent.hpp
//  JuceAudioFilePlayer - App
//
//  Created by Mr. Markuese on 08.01.23.
//

#ifndef ThumbnailComponent_h
#define ThumbnailComponent_h

#include <stdio.h>
#include <JuceHeader.h>


class ThumbnailComponent : public juce::Component,
                                 private juce::ChangeListener
{
public:
    ThumbnailComponent (int sourceSamplesPerThumbnailSample,
                              juce::AudioFormatManager& formatManager,
                        juce::AudioThumbnailCache& cache);
    
    ~ThumbnailComponent() override;

    void setFile (const juce::File& file);

    void paint (juce::Graphics& g) override;

    void paintIfNoFileLoaded (juce::Graphics& g);

    void paintIfFileLoaded (juce::Graphics& g);
    
    void paintMonoIfFileLoaded (juce::Graphics& g);
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

private:
    void thumbnailChanged();

    juce::AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThumbnailComponent)
};

#endif /* ThumbnailComponent_h */

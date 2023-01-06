#include "MainComponent.h"


MainComponent::MainComponent()
    :   state (Stopped),
        thumbnailCache (5),
        thumbnail (512, formatManager, thumbnailCache)
{
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled (false);

    addAndMakeVisible (&stopButton);
    stopButton.setButtonText ("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled (false);

    addAndMakeVisible (&loopingToggle);
    loopingToggle.setButtonText ("Loop");
    loopingToggle.onClick = [this] { loopButtonChanged(); };

    addAndMakeVisible (&currentPositionLabel);
    currentPositionLabel.setText ("Stopped", juce::dontSendNotification);
    
    addAndMakeVisible (&currentAudioFileNameLabel);
    currentAudioFileNameLabel.setText ("", juce::dontSendNotification);

    setSize (300, 200);

    formatManager.registerBasicFormats();
    transportSource.addChangeListener (this);
    
    // Register to Thumbnail broadcaster
    thumbnail.addChangeListener (this);

    setAudioChannels (0, 2);
    startTimer (1000);
}

MainComponent::~MainComponent() 
{
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
    {
        transportSource.releaseResources();
    }

void MainComponent::resized()
{
#if defined JUCE_IOS || defined JUCE_ANDROID
    openButton          .setBounds      (20, 70,  getWidth() - 40, 40);
    playButton          .setBounds      (20, 120, getWidth() - 40, 40);
    stopButton          .setBounds      (20, 170, getWidth() - 40, 40);
    loopingToggle       .setBounds      (20, 220, getWidth() - 40, 40);
    currentPositionLabel.setBounds      (20, 250, getWidth() - 40, 40);
    currentAudioFileNameLabel.setBounds (80, 250, getWidth() - 20, 20);
#else
    openButton          .setBounds      (10, 10,  getWidth() - 20, 20);
    playButton          .setBounds      (10, 40,  getWidth() - 20, 20);
    stopButton          .setBounds      (10, 70,  getWidth() - 20, 20);
    loopingToggle       .setBounds      (10, 100, getWidth() - 20, 20);
    currentPositionLabel.setBounds      (10, 130, getWidth() - 20, 20);
    currentAudioFileNameLabel.setBounds (80, 130, getWidth() - 20, 20);
#endif
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &transportSource) transportSourceChanged();
    if (source == &thumbnail)       thumbnailChanged();
}

void MainComponent::transportSourceChanged()
{
    if (transportSource.isPlaying())
        changeState (Playing);
    else if ((state == Stopping) || (state == Playing))
        changeState (Stopped);
    else if (Pausing == state)
        changeState (Paused);
}

void MainComponent::thumbnailChanged()
{
    repaint();
}

void MainComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> thumbnailBounds (10, 150, getWidth() - 20, getHeight() - 170);

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded (g, thumbnailBounds);
    else
    {
        // paintIfFileLoaded (g, thumbnailBounds);
        paintMonoIfFileLoaded (g, thumbnailBounds);
    }
        
}

void MainComponent::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 0.8);
}

void MainComponent::paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::white);
    g.fillRect (thumbnailBounds);

    g.setColour (juce::Colours::red);

    thumbnail.drawChannels (g,
                            thumbnailBounds,
                            0.0,                                    // start time
                            thumbnail.getTotalLength(),             // end time
                            0.8f);                                  // vertical zoom
}

void MainComponent::paintMonoIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::white);
    g.fillRect (thumbnailBounds);

    g.setColour (juce::Colours::red);

    thumbnail.drawChannel (g,
                            thumbnailBounds,
                            0.0,                                    // start time
                            thumbnail.getTotalLength(),             // end time
                            0,
                            0.8f);                                  // vertical zoom
    
    auto audioLength = (float) thumbnail.getTotalLength();
    g.setColour (juce::Colours::green);

    auto audioPosition = (float) transportSource.getCurrentPosition();
    auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth()
                        + (float) thumbnailBounds.getX();
    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition,
                (float) thumbnailBounds.getBottom(), 2.0f);
}

void MainComponent::timerCallback()
{
    if (transportSource.isPlaying() || state == Paused)
    {
        juce::RelativeTime position (transportSource.getCurrentPosition());

        auto minutes = ((int) position.inMinutes()) % 60;
        auto seconds = ((int) position.inSeconds()) % 60;
        auto positionString = juce::String::formatted ("%02d:%02d", minutes, seconds);

        currentPositionLabel.setText (positionString, juce::dontSendNotification);
        
        repaint();
    }
    else
    {
        currentPositionLabel.setText ("Stopped", juce::dontSendNotification);
    }
}

void MainComponent::updateLoopState (bool shouldLoop)
{
    if (readerSource.get() != nullptr)
        readerSource->setLooping (shouldLoop);
}

void MainComponent::changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case Stopped:
                playButton.setButtonText ("Play");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (false);
                transportSource.setPosition (0.0);
                break;

            case Starting:
                transportSource.start();
                break;

            case Playing:
                playButton.setButtonText ("Pause");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (true);
                break;

            case Pausing:
                transportSource.stop();
                break;

            case Paused:
                playButton.setButtonText ("Resume");
                stopButton.setButtonText ("Back");
                break;

            case Stopping:
                transportSource.stop();
                break;
        }
    }
}

void MainComponent::openButtonClicked()
{
/*  Example:
    File audioFile = File::getSpecialLocation (File::currentExecutableFile).getChildFile (String("b1k_no.wav"));
    String path = audioFile.getFullPathName();
    DBG("path: ");DBG(path);
*/
    chooser = std::make_unique<juce::FileChooser> ("Select a sound file to play...",
                                                   juce::File{},
                                                   "*.wav;*.aif;*.aiff;*.mp3");
    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync (chooserFlags, [this] (const FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file != File{})
        {
            String path = file.getFullPathName();
            DBG("path: "); DBG(path);
            auto stream = chooser->getURLResult().createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress));

            if (stream == nullptr)
            {
                DBG("stream == nullptr");
                // Error handling
                // TODO: Error callout
            }
                
            auto reader = formatManager.createReaderFor (std::move (stream));

            if (reader != nullptr)
            {
                DBG("reader != nullptr");
                // Then grab the file name
                currentAudioFileName = file.getFileNameWithoutExtension();
                currentAudioFileNameLabel.setText(currentAudioFileName, juce::dontSendNotification);
                
                auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
                transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled (true);
                thumbnail.setSource (new juce::FileInputSource (file));
                readerSource.reset (newSource.release());
            }
            else
            {
                DBG("reader: nullptr");
                // Error handling
                // TODO: Error callout
            }
        }
    });
    
}

void MainComponent::playButtonClicked()
{
    updateLoopState (loopingToggle.getToggleState());
    
    if ( (state == Stopped) || (state == Paused) )
    {
        changeState (Starting);
    }
        
    else if (state == Playing)
    {
        changeState (Pausing);
    }
}

void MainComponent::stopButtonClicked()
{
//    changeState (Stopping);
    if (state == Paused)
        changeState (Stopped);
    else
        changeState (Stopping);
}

void MainComponent::loopButtonChanged()
{
    updateLoopState (loopingToggle.getToggleState());
}

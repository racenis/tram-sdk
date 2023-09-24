# Audio

Right now the library's audio system doesn't have that many features, but it's
possible to load up `.ogg` audio files and play them back.

## Initialization

The Core and the UI systems need to be initialized first.

Then `Audio::Init()` function will initialize the audio system. 
`Audio::Update()` needs to be called once a frame and `Audio::Uninit()` when
exiting the application.

## Sounds

Sounds are considered Resources and can be streamed in from disk.

To play them, use an AudioComponent.

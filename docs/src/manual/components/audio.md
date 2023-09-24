# Audio

The AudioComponent plays Sounds. 

## Usage

```
// create the component
Component<AudioComponent> component;
component.make();

// give it the sound that it will play
component->SetSound("sound-name");

// set the location of the component in the world
componnt->SetLocation({0.0f, 0.0f, 0.0f});

// initialize it
component->Init();

// play the sound
component->Play();
```



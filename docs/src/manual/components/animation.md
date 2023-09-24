# Animation

The AnimationComponent component plays Animations. As in 3D animations for
3D models. It can also be used for making procedural animations.

To actually see the animations, AnimationComponent needs to be linked with a
RenderComponent.

## Usage

```
// create the component
Component<AnimationComponent> component;
component.make();

// give it the model that it will play animations on
component->SetModel("model-name");

// initialize it
component->Init();

// play an animation
component->PlayAnimation("animation-name", 1, 1.0f, 1.0f);
```



## Notes

Right now, before playing animations, they need to be loaded first. They are not
automatically streamed in from disk.

```
Render::Animation::Find("animation-name")->LoadFromDisk();
```

Idk, might change this later.
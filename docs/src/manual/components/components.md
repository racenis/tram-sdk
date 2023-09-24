# Components

EntityComponents are reusable pieces of logic for entities, but most of them can
also be used on their own.

Some of them are wrappers around low-level APIs, others are completely new.

## How do they work

Most components need to be created in their respective Pools, which can be
done with the `Component` smart pointer (or by asking the Pool directly).

Then they get fed some parameters and their `Init()` method is called.

After they are no longer needed, they can be deleted.

```
// define a component smart-pointer
Component<ExampleComponent> component;

// create a new component instance
component.make();

// feed it some parameters
component->SetFoo("123");
component->SetBar(321);

// initialize it
component->Init();

// when not needed anymore, the component can be deleted
component.clear();
```
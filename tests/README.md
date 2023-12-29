
# Running tests

Set the directory to `/tests/`.
Then run `make test`. This will compile and run all of the tests.

# Todo list
- Add a flag thing to the test framework
- Make test text output nicer


# Ideas for tests

## Async/Resource

- Create a Resource Mock
- Put them in the Async loader
- Make sure to check ForceLoadResource and the order in which resources get loaded

## Entity/WorldCell

- Create Entity mocks
- Check if loading WorldCell loads its Entities
- Also check if all the special loading flags are working

## EntityComponent

- Create an EntityComponent mock
- Create a resource mock
- EntityComponent requests a resource
- Check that EntityComponent receives a response and starts

## Event/Message

- Define some custom Message/Event types
- Send Messages, Events
- Make sure that they are received


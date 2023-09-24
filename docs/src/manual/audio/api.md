# API

The low-level API of the audio system for the library.

I tried to make it modular, so that it's possible to swap out the back-end calls
for different platforms.

It's really basic right now, but I will add audio effects and other stuff to it
later.

The library currently supports OpenAL as the back-end. There's also a spatial
audio back-end, but it's only a prototype.
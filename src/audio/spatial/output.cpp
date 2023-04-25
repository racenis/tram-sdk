#include <audio/spatial/output.h>

#include <portaudio/portaudio.h>

#include <framework/logging.h>

namespace tram::Audio::Spatial {

size_t next_buffer = 0;
float output_buffer[4][4096] = {0.0f};

static size_t buffer_progress = 0;

static bool very_bad_error = false;

static PaStream* audio_stream;

static int PortaudioCallback (
    const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData
) {
    float* output = (float*) outputBuffer;
    size_t samples_left_in_buffer = 4096 - buffer_progress;
    
    if (samples_left_in_buffer > framesPerBuffer) {
        memcpy(output, &output_buffer[next_buffer][buffer_progress], framesPerBuffer);
        buffer_progress += framesPerBuffer;
        
        return 0;
    }
    
    memcpy(output, &output_buffer[next_buffer][buffer_progress], samples_left_in_buffer);
    
    output += samples_left_in_buffer;
    
    buffer_progress = 0;
    next_buffer += 1;
    
    if (next_buffer >= 4) next_buffer = 0;
    
    memcpy(output, &output_buffer[next_buffer][buffer_progress], framesPerBuffer - samples_left_in_buffer);
    
    
    
    Log("fuck EEE {}", framesPerBuffer);
    
    //memcpy(outputBuffer, &output_buffer[next_buffer], 4096);
    
    
    next_buffer = next_buffer == 0 ? 1 : 0;
    
    return 0;
}

void InitOutput() {
    Log("trying to initialized PORTAUDIO");
    
    
    auto error = Pa_Initialize();
    
    if (error != paNoError) {
        Log("portaudio did fucky wucky on startup: {}", Pa_GetErrorText(error));
        abort();
    }

    error = Pa_OpenDefaultStream(&audio_stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                44100,
                                512,
                                /*4096,*/        /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                PortaudioCallback, /* this is your callback function */
                                nullptr ); /*This is a pointer that will be passed to
                                                   your callback*/
    if (error != paNoError) {
        Log("portaudio did fucky wucky on making stream: {}", Pa_GetErrorText(error));
        abort();
    }
    
    error = Pa_StartStream( audio_stream );
    if (error != paNoError) {
        Log("portaudio did fucky wucky on starting stream: {}", Pa_GetErrorText(error));
        abort();
    }


}

void UpdateOutput() {
    if (very_bad_error) {
        Log("very bad error!");
        abort();
    }
}

void UninitOutput() {
    
    
     auto   error = Pa_StopStream( audio_stream );
    if (error != paNoError) {
        Log("portaudio did fucky wucky on stopping stream: {}", Pa_GetErrorText(error));
        abort();
    }
    
     error = Pa_Terminate();
    
    if (error != paNoError) {
        Log("portaudio did fucky wucky on shutdown: {}", Pa_GetErrorText(error));
    }
}

}
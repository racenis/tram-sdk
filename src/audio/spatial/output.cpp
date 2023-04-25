#include <audio/spatial/output.h>

#include <portaudio/portaudio.h>

#include <framework/logging.h>

#include <atomic>

namespace tram::Audio::Spatial {

static PaStream* audio_stream;

static int PortaudioCallback (
    const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData
) {
    LockRenderlist();
    
    float* output = (float*) outputBuffer;
    
    for (unsigned int i = 0; i < framesPerBuffer * 2; i++) {
        output[i] = 0.0f;
    }
    
    for (auto& source : audiosources) {
        source.lock.Lock();
        
        // if not playing, skip
        if (~source.flags & SOURCE_PLAYING) {
            source.lock.Unlock();
            continue;
        }
        
        size_t source_length = source.buffer->length;
        for (size_t sample = 0; sample < framesPerBuffer; sample++) {
            for (size_t path = 0; path < PATHS_FOR_SOURCE; path++) {
                float delay = source.paths[path].distance / 331.0f;
                int32_t sample_delay = delay * 44100.0f;
                
                int32_t sample_delayed = source.sample;
                sample_delayed -= sample_delay;
                
                if (sample_delayed < 0) {
                    sample_delayed += source.buffer->length;
                }
                
                float sample_value = source.buffer->data[sample_delayed] * source.paths[path].force;
                
                output[sample * 2] += sample_value;
                output[(sample * 2) + 1] += sample_value;
            }
            
            source.sample += sample % source.buffer->sample_rate == 0;
            source.sample = source.sample % source_length;
        }
        
        source.lock.Unlock();
    }
    
    UnlockRenderlist();
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
                                paFramesPerBufferUnspecified,
                                //512,
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

void LockRenderlist() {
    
}

void UnlockRenderlist() {
    
}

}
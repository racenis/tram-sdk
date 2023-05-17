#include <audio/spatial/output.h>

#include <portaudio/portaudio.h>

#include <framework/logging.h>

#include <atomic>

namespace tram::Audio::Spatial {

static PaStream* audio_stream;

float largest_sample = 0.0f;
float largest_panning = INFINITY;
float smallest_panning = -INFINITY;

static const size_t REVERB_SIZE = 44100 * 3;
static float reverb_buffer[REVERB_SIZE];
static size_t reverb_progress = 0;

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
    
    for (auto& source : audiorenders) {
        source.lock.Lock();
        
        // if not playing, skip
        if (~source.flags & SOURCE_PLAYING) {
            source.lock.Unlock();
            continue;
        }
        
        float total_force = 0.0f;
        for (size_t path = 0; path < PATHS_FOR_RENDERING; path++) total_force += source.paths[path].force;
        
        float force_equalizer = total_force < 1.0f ? 1.0f : 1.0f / total_force;
        
        size_t source_length = source.buffer->length;
        for (size_t sample = 0; sample < framesPerBuffer; sample++) {
            for (size_t path = 0; path < PATHS_FOR_RENDERING; path++) {
                
                
                //if (largest_panning < panning) largest_panning = panning;
                //if (smallest_panning > panning) smallest_panning = panning;
                
                
                
                int32_t sample_delayed_left = source.sample;
                int32_t sample_delayed_right = source.sample;
                
                sample_delayed_left += source.paths[path].distance_delay + source.paths[path].panning_delay;
                sample_delayed_right += source.paths[path].distance_delay - source.paths[path].panning_delay;
                
                
                if (sample_delayed_left < 0) {
                    sample_delayed_left += source.buffer->length;
                }
                
                if (sample_delayed_right < 0) {
                    sample_delayed_right += source.buffer->length;
                }
                
                
                //float sample_value_left = source.buffer->data[sample_delayed_left] * source.paths[path].force * force_equalizer;
                //float sample_value_right = source.buffer->data[sample_delayed_right] * source.paths[path].force * force_equalizer;
                
                //float sample_value_left = source.buffer->data[source.sample] * source.paths[path].force * ((panning * 0.5f) + 0.5f) * force_equalizer;
                //float sample_value_right = source.buffer->data[source.sample] * source.paths[path].force * ((-panning * 0.5f) + 0.5f)* force_equalizer;
                
                float sample_value_left = source.buffer->data[sample_delayed_left] * source.paths[path].force * ((source.paths[path].panning * 0.5f) + 0.5f) * force_equalizer;
                float sample_value_right = source.buffer->data[sample_delayed_right] * source.paths[path].force * ((source.paths[path].panning * -0.5f) + 0.5f)* force_equalizer;
                
                output[sample * 2] += sample_value_left;
                output[(sample * 2) + 1] += sample_value_right;
            }
            
            if (largest_sample < output[sample * 2]) largest_sample = output[sample * 2];
            
            source.sample += sample % source.buffer->sample_rate == 0;
            source.sample = source.sample % source_length;
        }
        
        source.lock.Unlock();
    }
    
    
    // copy mixed-down sound into reverb buffer
    for (size_t sample = 0; sample < framesPerBuffer; sample++) {
        reverb_buffer[(reverb_progress + sample) % REVERB_SIZE] = 0.2f * 
        (output[sample * 2] +
        output[(sample * 2) + 1]);
    }
        
    // convolve some more reverb
    for (size_t sample = 0; sample < framesPerBuffer; sample++) {
        float value = reverb_buffer[(reverb_progress + sample) % REVERB_SIZE];
        
        value += listener_reverb_normalized[0] * reverb_buffer[(reverb_progress + listener_reverb_delay[0] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[1] * reverb_buffer[(reverb_progress + listener_reverb_delay[1] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[2] * reverb_buffer[(reverb_progress + listener_reverb_delay[2] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[3] * reverb_buffer[(reverb_progress + listener_reverb_delay[3] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[4] * reverb_buffer[(reverb_progress + listener_reverb_delay[4] + sample) % REVERB_SIZE];
        
        reverb_buffer[(reverb_progress + sample) % REVERB_SIZE] = value;
    }
    
    // copy reverb into output
    for (size_t sample = 0; sample < framesPerBuffer; sample++) {
        output[sample * 2] += 5.0f * reverb_buffer[(reverb_progress + sample) % REVERB_SIZE];
        output[(sample * 2) + 1] += 5.0f *  reverb_buffer[(reverb_progress + sample) % REVERB_SIZE];
    }
    
    reverb_progress += framesPerBuffer;
    reverb_progress %= REVERB_SIZE;
    
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
    //Log("l sample: {} l panning: {} s panning: {}", largest_sample, largest_panning, smallest_panning);
    largest_sample = 0.0f;
     largest_panning = -INFINITY;
 smallest_panning = INFINITY;
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
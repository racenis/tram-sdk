// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

// prevent this file from compiling
#if false

#include <audio/spatial/output.h>

#include <portaudio/portaudio.h>

#include <framework/logging.h>

#include <atomic>

namespace tram::Audio::Spatial {

static PaStream* audio_stream;

static const size_t REVERB_SIZE = 44100 * 3;
static float reverb_buffer[REVERB_SIZE];
static size_t reverb_progress = 0;

static int PortaudioCallback(
    const void*, 
    void *output_buffer,
    unsigned long frames,
    const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*
) {
    LockRenderlist();
    
    float* output = (float*) output_buffer;
    
    for (unsigned int i = 0; i < frames * 2; i++) {
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
        for (size_t sample = 0; sample < frames; sample++) {
            for (size_t path = 0; path < PATHS_FOR_RENDERING; path++) {
                
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
                
                float sample_value_left = source.buffer->data[sample_delayed_left] * source.paths[path].force * glm::min((source.paths[path].panning * 1.0f) + 1.0f, 1.0f) * force_equalizer;
                float sample_value_right = source.buffer->data[sample_delayed_right] * source.paths[path].force * glm::min((source.paths[path].panning * -1.0f) + 1.0f, 1.0f)* force_equalizer;
                
                output[sample * 2] += sample_value_left;
                output[(sample * 2) + 1] += sample_value_right;
            }
            
            source.sample += sample % source.buffer->sample_rate == 0;
            source.sample = source.sample % source_length;
        }
        
        source.lock.Unlock();
    }
    
    
    // copy mixed-down sound into reverb buffer
    for (size_t sample = 0; sample < frames; sample++) {
        reverb_buffer[(reverb_progress + sample) % REVERB_SIZE] = 0.2f * 
        (output[sample * 2] +
        output[(sample * 2) + 1]);
    }
        
    // convolve some more reverb
    for (size_t sample = 0; sample < frames; sample++) {
        float value = reverb_buffer[(reverb_progress + sample) % REVERB_SIZE];
        
        value += listener_reverb_normalized[0] * reverb_buffer[(reverb_progress + listener_reverb_delay[0] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[1] * reverb_buffer[(reverb_progress + listener_reverb_delay[1] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[2] * reverb_buffer[(reverb_progress + listener_reverb_delay[2] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[3] * reverb_buffer[(reverb_progress + listener_reverb_delay[3] + sample) % REVERB_SIZE];
        value += listener_reverb_normalized[4] * reverb_buffer[(reverb_progress + listener_reverb_delay[4] + sample) % REVERB_SIZE];
        
        reverb_buffer[(reverb_progress + sample) % REVERB_SIZE] = value;
    }
    
    // copy reverb into output
    for (size_t sample = 0; sample < frames; sample++) {
        output[sample * 2] += 5.0f * reverb_buffer[(reverb_progress + sample) % REVERB_SIZE];
        output[(sample * 2) + 1] += 5.0f *  reverb_buffer[(reverb_progress + sample) % REVERB_SIZE];
    }
    
    reverb_progress += frames;
    reverb_progress %= REVERB_SIZE;
    
    UnlockRenderlist();
    return 0;
}

void InitOutput() {
    Log("trying to initialized PORTAUDIO");
    
    auto error = Pa_Initialize();
    
    if (error != paNoError) {
        Log("portaudio did NOT on startup: {}", Pa_GetErrorText(error));
        abort();
    }

    error = Pa_OpenDefaultStream(
        &audio_stream,
        0,  // no input
        2,  // stereo output
        paFloat32,
        44100,
        paFramesPerBufferUnspecified, // change to change buffer size
        PortaudioCallback,
        nullptr
    );
    
    if (error != paNoError) {
        Log("portaudio did NOT on making stream: {}", Pa_GetErrorText(error));
        abort();
    }
    
    error = Pa_StartStream( audio_stream );
    if (error != paNoError) {
        Log("portaudio did NOT on starting stream: {}", Pa_GetErrorText(error));
        abort();
    }


}

void UpdateOutput() {
    
}

void UninitOutput() {
    auto error = Pa_StopStream( audio_stream );
    if (error != paNoError) {
        Log("portaudio did NOT on stopping stream: {}", Pa_GetErrorText(error));
        abort();
    }

    error = Pa_Terminate();

    if (error != paNoError) {
        Log("portaudio did NOT on shutdown: {}", Pa_GetErrorText(error));
    }
}

void LockRenderlist() {
    
}

void UnlockRenderlist() {
    
}

}

#endif
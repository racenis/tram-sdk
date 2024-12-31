// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/core.h>
#include <entities/sound.h>
#include <components/audio.h>

#include <framework/serialization.h>

/**
 * @class tram::Sound entities/sound.h <entities/sound.h>
 * 
 * AudioComponent wrapper.
 * 
 * Can be used for placing environmental sounds, or for playing sounds for
 * Signal triggers, but since you're looking this up in the C++ docs, you are
 * probably are intereseted in the Sound entity as an self-yeeting 
 * AudioComponent. 
 * 
 * Which it can be used as.
 * 
 * Very successfully, might I add.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/entities/sound.html
 */

namespace tram {

enum {
    SOUND_LOOPING = 1,
    SOUND_START_ON_LOAD = 2,
};
    
enum {
    FIELD_FLAGS,
    FIELD_SOUND,
    FIELD_VOLUME
};

static const Type fields[3] = {
    TYPE_INT32,
    TYPE_NAME,
    TYPE_FLOAT32
}; 

void Sound::Register() {
    Entity::RegisterType(
        "sound", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Sound(a, b); },
        [](Entity* a) { delete a; },
        fields,
        3
    );
}

name_t Sound::GetType() {
    return "sound";
}

/// Regular map file constructor.
Sound::Sound(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    sound = field_array[FIELD_SOUND];
    volume = field_array[FIELD_VOLUME];
    sound_flags = field_array[FIELD_FLAGS];
}

/// PLays a sound, then removes itself.
Sound::Sound(name_t sound, float volume, vec3 position) : Entity("none") {
    Audio::Sound* s = Audio::Sound::Find(sound);
    
    this->sound = sound;
    this->volume = volume;
    this->sound_flags = SOUND_START_ON_LOAD;
    
    this->location = position;
    
    Message msg = {
        .type = Message::KILL,
        .sender = this->id,
        .receiver = this->id
    };
    
    Message::Send(msg, s->GetLength());
    
    flags |= NON_PERSISTENT;
    
    Load();
}

void Sound::UpdateParameters () {
    if (!IsLoaded()) return;
    audio->SetLocation(location);
}

void Sound::SetParameters () {
    UpdateParameters();
}

void Sound::Load () {
    audio.make();
    
    audio->SetParent(this);
    audio->SetSound(sound);
    audio->SetRepeating(sound_flags & SOUND_LOOPING);
    audio->SetLocation(location);
    
    audio->Init();
    
    if (sound_flags & SOUND_START_ON_LOAD) {
        audio->Play();
    }
    
    flags |= LOADED;
}

void Sound::Unload () {
    flags &= ~LOADED;

    Serialize();

    audio.clear();
}

void Sound::Serialize () {

}

void Sound::MessageHandler (Message& msg) {
    switch (msg.type) {
        case Message::TOGGLE:
            if (audio->IsPlaying()) {
                audio->Stop();
            } else {
                audio->Play();
            }
            break;
        case Message::KILL:
            Yeet();
            break;
        case Message::START:
            audio->Play();
            break;
        case Message::STOP:
            audio->Stop();
            break;
        default:
            return;
    }
}

}
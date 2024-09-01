// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <entities/sound.h>
#include <components/audio.h>

#include <framework/serialization.h>

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

static const uint32_t fields[3] = {
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
    
    Load();
}

void Sound::UpdateParameters () {
    if (!is_loaded) return;
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
    
    is_loaded = true;
}

void Sound::Unload () {
    is_loaded = false;

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
            delete this;
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
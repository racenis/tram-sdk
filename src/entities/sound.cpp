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

/// Regular map file constructor.
Sound::Sound(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    sound = field_array[FIELD_SOUND];
    volume = field_array[FIELD_VOLUME];
    flags = field_array[FIELD_FLAGS];
}

/// PLays a sound, then removes itself.
Sound::Sound(name_t sound, float volume, vec3 position) : Entity("none") {
    Audio::Sound* s = Audio::Sound::Find(sound);
    
    this->sound = sound;
    this->volume = volume;
    this->flags = SOUND_START_ON_LOAD;
    
    this->location = position;
    
    Message msg = {
        .type = Message::KILL,
        .sender = this->id,
        .receiver = this->id
    };
    
    Message::Send(msg, s->GetLength() * 60.0f);
    
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
    audio->SetRepeating(flags & SOUND_LOOPING);
    audio->SetLocation(location);
    
    audio->Init();
    
    if (flags & SOUND_START_ON_LOAD) {
        std::cout << "PLAYING!" << std::endl;
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
        case Message::KILL:
            delete this;
            break;
        default:
            return;
    }
}

}
#include <extensions/kitchensink/character.h>

#include <extensions/kitchensink/attributes.h>

#include <framework/entity.h>
#include <framework/file.h>

#include <templates/hashmap.h>
#include <templates/pool.h>

const size_t FACTION_COUNT = 50;
const size_t CHARACTER_COUNT = 50;
const size_t CLASS_COUNT = 50;

namespace tram {

template<> Pool<Ext::Kitchensink::Faction> PoolProxy<Ext::Kitchensink::Faction>::pool("Ext::Kitchensink::Faction Pool", FACTION_COUNT);
template<> Pool<Ext::Kitchensink::Character> PoolProxy<Ext::Kitchensink::Character>::pool("Ext::Kitchensink::Character Pool", CHARACTER_COUNT);
template<> Pool<Ext::Kitchensink::CharacterClass> PoolProxy<Ext::Kitchensink::CharacterClass>::pool("Ext::Kitchensink::CharacterClass Pool", CLASS_COUNT);

}

namespace tram::Ext::Kitchensink {

Hashmap<Faction*> factions("Ext::Kitchensink::Faction Hashmap", FACTION_COUNT);
Hashmap<CharacterClass*> character_classes("Ext::Kitchensink::Character Hashmap", CHARACTER_COUNT);
Hashmap<Character*> characters("Ext::Kitchensink::CharacterClass Hashmap", CLASS_COUNT);

class Character;
    
void Faction::AddCharacter(Character* character) {
    bool faction_marked = this->HasCharacter(character);
    bool character_marked = character->IsInFaction(this);
    
    if (!faction_marked) {
        FactionRank new_rank;
        
        new_rank.character = character;
        new_rank.rank = 0.0f;
        new_rank.loyalty = 0.0f;
        
        ranks.push_back(new_rank);
    }
    
    if (!character_marked) {
        character->factions.push_back(this);
    }
}

void Faction::RemoveCharacter(Character* character) {
    std::erase_if(this->ranks, [=](FactionRank& rank) { return rank.character == character;});
}

bool Faction::HasCharacter(Character* character) {
    for (auto& rank : ranks) {
        if (rank.character == character) return true;
    }
    
    return false;
}

void Faction::SetCharacterRank(Character* character, float rank_value) {
    for (auto& rank : ranks) {
        if (rank.character == character) {
            rank.rank = rank_value;
            return;
        }
    }
}

void Faction::SetCharacterLoyalty(Character* character, float loyalty) {
    for (auto& rank : ranks) {
        if (rank.character == character) {
            rank.loyalty = loyalty;
            return;
        }
    }
}

float Faction::GetCharacterRank(Character* character) {
    for (auto& rank : ranks) {
        if (rank.character == character) {
            return rank.rank;
        }
    }
    
    return 0.0f;
}

float Faction::GetCharacterLoyalty(Character* character) {
    for (auto& rank : ranks) {
        if (rank.character == character) {
            return rank.loyalty;
        }
    }
    
    return 0.0f;
}

void Faction::SetRelation(Faction* faction, float disposition) {
    if (disposition == 0.0f) {
        std::erase_if(this->relations, [=](FactionRelation& relation) { return relation.faction == faction;});
    } else if (this->GetRelation(faction) == 0.0f) {
        FactionRelation new_relation; 
        
        new_relation.faction = faction;
        new_relation.disposition = disposition;
        
        this->relations.push_back(new_relation);
    } else {
        for (auto& relation : this->relations) {
            if (relation.faction != faction) continue;
            relation.disposition = disposition;
            return;
        }
    }
}

float Faction::GetRelation(Faction* other) {
    for (auto& relation : this->relations) {
        if (relation.faction == other) {
            return relation.disposition;
        }
    }
    
    return 0.0f;
}

float Faction::GetDisposition(Character* character) {
    float disposition_sum = 0.0f;
    for (auto faction : character->factions) {
        disposition_sum += this->GetRelation(faction);
    }
    return disposition_sum;
}

Faction* Faction::Find(name_t name) {
    Faction* faction = factions.Find(name);
    
    if (!faction) {
        faction = PoolProxy<Faction>::New();
        factions.Insert(name, faction);
    }
    
    return faction;
}

struct InitialAttribute {
    name_t attribute;
    float value;
};

void CharacterClass::AddBaseClass(CharacterClass* character_class) {
    for (auto base_class : this->base_classes) {
        if (base_class == character_class) return;
    }
    this->base_classes.push_back(character_class);
}

bool CharacterClass::HasBaseClass(CharacterClass* character_class) {
    if (this == character_class) return true;
    for (auto base_class : this->base_classes) {
        if (base_class->HasBaseClass(character_class)) return true;
    }
    return false;
}


void CharacterClass::SetAttribute(name_t attribute_name, float value) {
    for (auto& attribute : this->base_attributes) {
        if (attribute.name == attribute_name) {
            attribute.value = value;
            return;
        }
    }
    
    BaseAttribute new_attribute;
    
    new_attribute.name = attribute_name;
    new_attribute.value = value;
    
    this->base_attributes.push_back(new_attribute);
}

float CharacterClass::GetAttribute(name_t attribute_name) {
    for (auto& attribute : this->base_attributes) {
        if (attribute.name == attribute_name) {
            return attribute.value;
        }
    }
    
    return 0.0f;
}

void CharacterClass::AddFaction(Faction* faction, float rank, float loyalty) {
    for (auto& existing_faction : this->base_factions) {
        if (existing_faction.faction == faction) {
            existing_faction.rank = rank;
            existing_faction.loyalty = loyalty;
            return;
        }
    }
    
    BaseFactionRank new_faction;
    
    new_faction.faction = faction;
    new_faction.rank = rank;
    new_faction.loyalty = loyalty;
    
    this->base_factions.push_back(new_faction);
}


void CharacterClass::GatherBaseAttributes(AttributeContainer& container) {
    for (auto& attribute : this->base_attributes) {
        const float base_value = container.GetAttributeBase(attribute.name);
        const float new_value = base_value + attribute.value;
        
        container.SetAttribute(attribute.name, new_value, 1.0f);
    }
    
    for (auto base_class : this->base_classes) {
        base_class->GatherBaseAttributes(container);
    }
}

CharacterClass* CharacterClass::Find(name_t name) {
    CharacterClass* character_class = character_classes.Find(name);
    
    if (!character_class) {
        character_class = PoolProxy<CharacterClass>::New();
        character_classes.Insert(name, character_class);
    }
    
    return character_class;
}


void Character::AddClass(CharacterClass* character_class) {
    this->character_class = character_class;
}

bool Character::HasClass(CharacterClass* character_class) {
    return this->character_class ? this->character_class->HasBaseClass(character_class) : false;
}


float Character::GetBaseDisposition(Character* other) {
    float base_disposition = 0.0f;
    for (auto& disposition : dispositions) {
        if (disposition.character == other) {
            base_disposition += disposition.disposition;
        }
    }

    return base_disposition;
}

float Character::GetDisposition(Character* other) {
    float base_disposition = this->GetBaseDisposition(other);
    
    float faction_disposition = 0.0f;
    for (auto faction : factions) {
        faction_disposition += faction->GetDisposition(other) * faction->GetCharacterLoyalty(this);
    }
    
    return base_disposition + faction_disposition;
}

void Character::SetBaseDisposition(Character* other, float disposition_value) {
    for (auto& disposition : dispositions) {
        if (disposition.character == other) {
            disposition.disposition = disposition_value;
        }
    }
}

bool Character::IsInFaction(Faction* check_faction) {
    for (auto faction : factions) {
        if (faction == check_faction) return true;
    }
    
    return false;
}

Character* Character::Find(Entity* entity) {
    return Character::Find(entity->GetID());
}

Character* Character::Find(id_t entity_id) {
    Character* character = characters.Find(entity_id);
    
    if (!character) {
        character = PoolProxy<Character>::New();
        characters.Insert(entity_id, character);
        character->entity_id = entity_id;
    }
    
    return character;
}

void Character::LoadFromDisk(const char* filename) {
    char path [100] = "data/";
    strcat(path, filename);
    strcat(path, ".char");

    File file (path, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't open dialog file '" << path << "'" << std::endl;
        abort();
    }

    name_t file_type = file.read_name();

    if (file_type != "DIALOGv1") {
        std::cout << "Invalid quest file type " << path << std::endl;
        abort();
    }
    
    std::cout << "Loading: " << filename << std::endl;

    while (file.is_continue()) {
        auto record = file.read_name();
        
        if (record == "character") {
            id_t entity_id = file.read_uint32();
            name_t character_class = file.read_name();
            
            Character* character = Character::Find(entity_id);
            
            if (character_class) {
                 character->character_class = CharacterClass::Find(character_class);
            }
            
        } else if (record == "disposition") {
            Character* from = Character::Find(file.read_uint32());
            Character* toward = Character::Find(file.read_uint32());
            
            from->SetBaseDisposition(toward, file.read_float32());
            
        } else if (record == "faction-member") {
            Character* character = Character::Find(file.read_uint32());
            Faction* faction = Faction::Find(file.read_name());
            
            faction->AddCharacter(character);
            faction->SetCharacterRank(character, file.read_float32());
            faction->SetCharacterLoyalty(character, file.read_float32());
            
        } else if (record == "attribute") {
            CharacterClass* character_class = CharacterClass::Find(file.read_name());
            
            name_t attribute_name = file.read_name();
            float value = file.read_float32();
            
            character_class->SetAttribute(attribute_name, value);
        } else if (record == "faction-member-class") {
            CharacterClass* character_class = CharacterClass::Find(file.read_name());
            Faction* faction = Faction::Find(file.read_name());
            
            float rank = file.read_float32();
            float loyalty = file.read_float32();
            
            character_class->AddFaction(faction, rank, loyalty);
            
        } else if (record == "base-class") {
            CharacterClass* character_class = CharacterClass::Find(file.read_name());
            CharacterClass* base_class = CharacterClass::Find(file.read_name());
            
            character_class->AddBaseClass(base_class);
            
        } else {
            std::cout << "unknown character record: " << record << std::endl;
            abort();
        }
    }
}

}
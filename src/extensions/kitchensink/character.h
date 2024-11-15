// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_CHARACTER_H
#define TRAM_SDK_EXTENSIONS_DESIGN_CHARACTER_H

#include <extensions/kitchensink/attributes.h>

#include <framework/core.h>

namespace tram {
    class Entity;
}

namespace tram::Ext::Kitchensink {

class Character;
class Faction;
    
struct FactionRelation {
    Faction* faction;
    float disposition;
};

struct FactionRank {
    Character* character;
    float rank;
    float loyalty;
};

struct BaseFactionRank {
    Faction* faction;
    float rank;
    float loyalty;
};
    
class Faction {
public:
    name_t name;
    
    void AddCharacter(Character*);
    void RemoveCharacter(Character*);
    bool HasCharacter(Character*);
    void SetCharacterRank(Character*, float);
    void SetCharacterLoyalty(Character*, float);
    float GetCharacterRank(Character*);
    float GetCharacterLoyalty(Character*);
    void SetRelation(Faction*, float);
    float GetRelation(Faction*);
    float GetDisposition(Character*);
    
    static Faction* Find(name_t name);
protected:
    std::vector<FactionRank> ranks;
    std::vector<FactionRelation> relations;
};

struct BaseAttribute {
    name_t name;
    float value;
};

class CharacterClass {
public:
    void AddBaseClass(CharacterClass*);
    bool HasBaseClass(CharacterClass*);
    
    void SetAttribute(name_t attribute, float value);
    float GetAttribute(name_t attribute);
    
    void AddFaction(Faction*, float, float);
    
    void GatherBaseAttributes(AttributeContainer&);
    void GatherFactions(Character*);

    static CharacterClass* Find(name_t name);
protected:
    std::vector<CharacterClass*> base_classes;
    std::vector<BaseAttribute> base_attributes;
    std::vector<BaseFactionRank> base_factions;
};

struct CharacterDisposition {
    Character* character;
    float disposition;
};

class Character {
public:
    void AddClass(CharacterClass*);
    bool HasClass(CharacterClass*);
    
    float GetDisposition(Character*);
    float GetBaseDisposition(Character*);
    void SetBaseDisposition(Character*, float);
    
    bool IsInFaction(Faction*);
    
    static void LoadFromDisk(const char*);
    static Character* Find(Entity* entity);
    static Character* Find(id_t entity_id);
protected:
    friend class Faction;
    
    id_t entity_id = 0;
    
    CharacterClass* character_class = nullptr;
    
    std::vector<CharacterDisposition> dispositions;
    std::vector<Faction*> factions;
};

}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_CHARACTER_H
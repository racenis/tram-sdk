// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entity.h>
#include <framework/serialization.h>

namespace tram {
    
class Marker : public Entity {
public:
    Marker(const SharedEntityData&, const SerializedFieldArray&);

    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    void Register();
    
    class Data: public SerializedEntityData {
    public:
        void ToString(std::string& str) {}

        void FromString(std::string_view& str) {}
        
        std::vector<FieldInfo> GetFieldInfo() {
            return std::vector<FieldInfo> {};
        }
        
        char const* GetType() {
            return "marker";
        }
    };
};

}
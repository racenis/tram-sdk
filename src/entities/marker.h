#include <framework/entity.h>
#include <framework/serializeddata.h>

namespace Core {
    
class Marker : public Entity {
public:
    Marker(std::string_view& str);

    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    
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
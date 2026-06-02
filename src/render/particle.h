// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_PARTICLE_H
#define TRAM_SDK_RENDER_PARTICLE_H

#include <render/model.h>
#include <render/sprite.h>

namespace tram::Render {

class Particle : public Resource {
public:
    Particle(name_t name) : Resource(name) {}

    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload();
    
    enum class DataType {
        SCALAR,
        VECTOR,
    };
    
    struct Data {
        name_t name;
        DataType type;
    };
    
    enum class OperationType {
        COPY,
        OSCILLATOR,
        NOISE,
        CLAMP,
        NORMALIZE
    };
    
    enum class MergeType {
        SET,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE
    };
    
    enum class MergeDest {
        ANY,
        X,
        Y,
        Z
    };
    
    struct LookupInfo {
        DataType type;
        bool array;
        int offset;
    };
    
    enum class ParamType {
        NONE,
        DATA,
        SCALAR,
        VECTOR
    };
    
    struct Parameter {
        ParamType type;
        name_t data;
        LookupInfo data_lookup;
        float scalar;
        vec3 vector;
    };
    
    struct Operation {
        OperationType type;
        MergeType merge;
        MergeDest dest;
        name_t target;
        LookupInfo target_lookup;
        
        Parameter param1;
        Parameter param2;
        Parameter param3;
        Parameter param4;
    };
    
    enum class ConstraintType {
        GREATER_THAN,
        LESSER_THAN,
    };
    
    struct Constraint {
        ConstraintType type;
        MergeDest dest;
        name_t property;
        LookupInfo property_lookup;
        
        Parameter param1;
        Parameter param2;
        Parameter param3;
        Parameter param4;
    };
    
    struct Emitter {
        Parameter rate;
        Parameter delay;
    };
    
    class System {
    public:
        void SetSprite(Sprite* sprite);
        void SetWire(Material* material);
        void SetModel(Model* model);
        
        void AddValue(Data data);
        void AddOperation(Operation operation);
        void AddInitializer(Operation operation);
        void AddConstraint(Constraint constraint);
        void AddEmitter(Parameter rate, Parameter delay);
        
        Sprite* GetSprite();
        Material* GetWire();
        Model* GetModel();
        
        const Data* GetValue(int index);
        const Operation* GetOperation(int index);
        const Operation* GetInitializer(int index);
        const Constraint* GetConstraint(int index);
        const Emitter* GetEmitter(int index);
        
        int GetPositionOffset() {return position_offset;}
        int GetRotationOffset() {return rotation_offset;}
        int GetColorOffset() {return color_offset;}
        int GetWidthOffset() {return width_offset;}
        int GetHeightOffset() {return height_offset;}
        int GetFrameOffset() {return frame_offset;}
        
        void SetParticleLimit(int limit) {particle_limit = limit;}
        int GetParticleLimit() {return particle_limit;}
    protected:
        std::vector<Data> vals;
        std::vector<Operation> ops;
        std::vector<Operation> inits;
        std::vector<Constraint> constrs;
        std::vector<Emitter> emits;
        
        Sprite* sprite = nullptr;
        Material* wire = nullptr;
        Model* model = nullptr;
        
        int position_offset = -1;
        int rotation_offset = -1;
        int color_offset = -1;
        int width_offset = -1;
        int height_offset = -1;
        int frame_offset = -1;
        
        std::vector<int> data_offsets;
        
        int particle_limit = 128;
        
        friend class Particle;
    };
    
    System* CreateSystem();
    
    int GetSystems(System** array, int array_size);
    System* GetBaseSystem();
    
    void AddControl(name_t name, DataType type);
    name_t GetControlName(int index);
    DataType GetControlType(int index);
    int GetControlOffset(int index);
    
    int GetDataSize() {return data_size;}
    
    static Particle* Find(name_t name);
protected:
    std::vector<System*> systems;
    System* base = nullptr;
    std::vector<Data> controls;
    std::vector<int> control_offsets;
    
    LookupInfo FindValueKey(name_t name, System* system);
    void FillKeys(Operation& op, System* system);
    void FillKeys(Constraint& op, System* system);
    
    void ActuallyLoadFromDisk();
    
    int data_size = 0;
};

}


#endif // TRAM_SDK_RENDER_PARTICLE_H
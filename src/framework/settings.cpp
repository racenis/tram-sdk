#include <framework/settings.h>

#include <templates/hashmap.h>

namespace tram::Settings {

Hashmap<ValuePtr> all_settings("settings map", 200);
    
/*void Register(ValuePtr ptr, name_t name) {
    all_settings.Insert(name, ptr);
}*/

void Register(Value value, name_t name) {
    Value* new_value = new Value(value);
    all_settings.Insert(name, *new_value);
}

void Register(bool* value, name_t name) {
    all_settings.Insert(name, value);
}

void Register(float* value, name_t name) {
    all_settings.Insert(name, value);
}


Value Get(name_t name) {
    return all_settings.Find(name);
}

void Set(name_t name, Value value) {
    all_settings.Find(name).SetValue(value);
}


void Parse(const char** argv, int argc) {
    for (int i = 1; i < argc; i++) {
        const char* name = argv[i];
        
        if (*name != '-') {
            std::cout << "unrexcoginzed parameter: " << name << std::endl;
            continue;
        }
        
        name++;
        
        ValuePtr value = all_settings.Find(name);
        
        if (value.GetType() == TYPE_BOOL) {
            value.SetValue(true);
            continue;
        }
        
        i++;
        if (i == argc) {
            std::cout << "Setting '" << name << "' not bool and has no parameter." << std::endl;
            return;
        }
        //assert(i != argc);
        
        switch (value.GetType()) {
            case TYPE_INT8:     value.SetValue((int8_t)atoi(argv[i])); break;
            case TYPE_INT16:    value.SetValue((int16_t)atoi(argv[i])); break;
            case TYPE_INT32:    value.SetValue((int32_t)atoi(argv[i])); break;
            case TYPE_INT64:    value.SetValue((int64_t)atoi(argv[i])); break;
            case TYPE_UINT8:    value.SetValue((uint8_t)atoi(argv[i])); break;
            case TYPE_UINT16:   value.SetValue((uint16_t)atoi(argv[i])); break;
            case TYPE_UINT32:   value.SetValue((uint32_t)atoi(argv[i])); break;
            case TYPE_UINT64:   value.SetValue((uint64_t)atoi(argv[i])); break;
            case TYPE_FLOAT32:  value.SetValue((float)atof(argv[i])); break;
            case TYPE_FLOAT64:  value.SetValue((double)atof(argv[i])); break;
            case TYPE_NAME:     value.SetValue((name_t)argv[i]); break;
            case TYPE_STRING:   value.SetValue(argv[i]); break;
            case TYPE_VEC2:     value.SetValue(vec2{atof(argv[i]), atof(argv[i+1])}); i += 1; break;
            case TYPE_VEC3:     value.SetValue(vec3{atof(argv[i]), atof(argv[i+1]), atof(argv[i+2])}); i += 2; break; 
            case TYPE_VEC4:     value.SetValue(vec4{atof(argv[i]), atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3])}); i += 3; break;
            case TYPE_QUAT:     value.SetValue(quat(vec3{atof(argv[i]), atof(argv[i+1]), atof(argv[i+2])})); i += 2; break;
            default: assert(false); abort();
        }
        
    }
}


void Save(const char* file) {
    // TODO: implement
}

void Load(const char* file) {
    // TODO: implement
}


}
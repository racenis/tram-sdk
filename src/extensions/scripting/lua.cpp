// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/scripting/lua.h>
#include <framework/script.h>
#include <framework/logging.h>

#include <lua.hpp>

#include <iostream>
#include <cstring>

/**
 * @namespace tram::Ext::Scripting::Lua
 * 
 * Lua bindings for the Script system.
 */

namespace tram::Ext::Scripting::Lua {

static lua_State* L = nullptr;

// Takes a Lua value from Lua stack and tries to find the best fit for it from
// the types that are supported by value_t.
static Type best_value(int index) {
    switch (lua_type(L, index)) {
        case LUA_TNUMBER:   return lua_isinteger(L, index) ? TYPE_INT32 : TYPE_FLOAT32;
        case LUA_TBOOLEAN:  return TYPE_BOOL;
        case LUA_TSTRING:   return TYPE_STRING;
        case LUA_TTABLE:    {
            lua_getfield(L, index, "x");
            lua_getfield(L, index, "y");
            lua_getfield(L, index, "z");
            lua_getfield(L, index, "w");
            
            bool has_x = lua_type(L, -4) == LUA_TNUMBER;
            bool has_y = lua_type(L, -3) == LUA_TNUMBER;
            bool has_z = lua_type(L, -2) == LUA_TNUMBER;
            bool has_w = lua_type(L, -1) == LUA_TNUMBER;
            
            lua_pop(L, 4);
            
            if (has_x && has_y && has_z && !has_w) {
                return TYPE_VEC3;
            }
            
            if (has_x && has_y && has_z && has_w) {
                return TYPE_QUAT;
            }
            
            return TYPE_UNDEFINED;
        }
        
        case LUA_TNIL:
        default: 
        
            return TYPE_UNDEFINED;
        }
}

// Converts and pushes a value_t to Lua stack.
static void push_value_to_stack(const value_t& value) {
    if (value.IsBool()) {
        lua_pushboolean(L, (bool)value);        return;
    }
    
    if (value.IsInt()) {
        lua_pushinteger(L, value.GetInt());     return;
    }
    
    if (value.IsFloat()) {
        lua_pushnumber(L, value.GetFloat());   return;
    }
    
    switch (value.GetType()) {
        case TYPE_NAME:     lua_pushstring(L, (name_t)value);       return;
        case TYPE_STRING:   lua_pushstring(L, (const char*)value);  return;
        case TYPE_VEC3:
            lua_createtable(L, 0, 3);
            lua_pushnumber(L, ((vec3)value).x); lua_setfield(L, -2, "x");
            lua_pushnumber(L, ((vec3)value).y); lua_setfield(L, -2, "y");
            lua_pushnumber(L, ((vec3)value).z); lua_setfield(L, -2, "z");
            return;
        case TYPE_QUAT:
            lua_createtable(L, 0, 4);
            lua_pushnumber(L, ((quat)value).x); lua_setfield(L, -2, "x");
            lua_pushnumber(L, ((quat)value).y); lua_setfield(L, -2, "y");
            lua_pushnumber(L, ((quat)value).z); lua_setfield(L, -2, "z");
            lua_pushnumber(L, ((quat)value).w); lua_setfield(L, -2, "w");
            return;
        default: lua_pushnil(L);
    }
}

// Takes a Lua value from the Lua stack and packs it into a value_t.
static value_t get_value_from_stack(int index, Type type) {
    switch (type) {
        case TYPE_UNDEFINED: {
            Type best_type = best_value(index);
            
            if (best_type == TYPE_UNDEFINED) {
                return value_t();
            }
            
            return get_value_from_stack(index, best_type);
        } break;
        
        case TYPE_BOOL:         return (bool) lua_toboolean(L, index);
        
        case TYPE_NAME:         return (name_t) lua_tostring(L, index);
        case TYPE_STRING:       return (const char*) lua_tostring(L, index);

        case TYPE_INT8:
        case TYPE_INT16:
        case TYPE_INT32:        return (int32_t) lua_tointeger(L, index);
        case TYPE_UINT8:
        case TYPE_UINT16:
        case TYPE_UINT32:       return (uint32_t) lua_tointeger(L, index);

        case TYPE_FLOAT32:      return (float) lua_tonumber(L, index);

        case TYPE_VEC3: {
            lua_getfield(L, index, "x");
            lua_getfield(L, index, "y");
            lua_getfield(L, index, "z");
            vec3 vec = {lua_tonumber(L, -3),
                        lua_tonumber(L, -2),
                        lua_tonumber(L, -1)};
            
            lua_pop(L, 3);
            return vec;
        }
        
        case TYPE_QUAT: {
            lua_getfield(L, index, "w");
            lua_getfield(L, index, "x");
            lua_getfield(L, index, "y");
            lua_getfield(L, index, "z");
            quat vec = {(float)lua_tonumber(L, -4),
                        (float)lua_tonumber(L, -3),
                        (float)lua_tonumber(L, -2),
                        (float)lua_tonumber(L, -1)};
            
            lua_pop(L, 4);
            return vec;
        }
        
        default: return value_t();
    }
}

const size_t MAX_LUA_FUNCTIONS = 200;

// I think that this could acutally be easilier useed and switch to std::vector
static struct {
    std::vector<Type> parameters;
    value_t (*function)(valuearray_t) = nullptr;
} registered_functions[MAX_LUA_FUNCTIONS];
static size_t registered_function_count = 0;

// this function will get called from the lua vm and in turn will then find and
// call whichever its name was registered to
extern "C" {
static int function_call_from_lua(lua_State* L) {
    auto& func = registered_functions[lua_tointeger(L, lua_upvalueindex(1))];

    std::vector<Value> params;
    
    if (func.parameters.size()) {
        assert(func.parameters.size() == (size_t)lua_gettop(L) && "function called with registered param count");
        for (size_t i = 0; i < func.parameters.size(); i++) {
            params.push_back(get_value_from_stack(i+1, func.parameters[i]));
        }
    } else {
        int param_count = lua_gettop(L);
        for (int i = 0; i < param_count; i++) {
            params.push_back(get_value_from_stack(i+1, TYPE_UNDEFINED));
        }
    }
    
    
    push_value_to_stack(func.function({params.data(), params.size()}));
    
    return 1;
}
}






static void set_global(name_t name, value_t value) {
    push_value_to_stack(value);
    lua_setglobal(L, name);
}
    
static value_t get_global(name_t name) {
    lua_getglobal(L, name);
    value_t result = get_value_from_stack(lua_gettop(L), best_value(lua_gettop(L)));
    lua_pop(L, 1);
    return result;
}

static void set_function(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t)) {
    assert(registered_function_count < MAX_LUA_FUNCTIONS);
    registered_functions[registered_function_count] = {parameters, function};
    lua_pushinteger(L, registered_function_count);
    lua_pushcclosure(L, function_call_from_lua, 1);
    lua_setglobal(L, name);
    registered_function_count++;
}

static value_t call_function(name_t name, std::vector<Value> parameters) {
    lua_getglobal(L, name);
	for (auto& val : parameters) push_value_to_stack(val);
	lua_call(L, parameters.size(), 1);
    value_t ret = get_value_from_stack(lua_gettop(L), best_value(lua_gettop(L)));
    lua_pop(L, 1);
    return ret;
}

static void load_script(const char* script) {
    std::string path = std::string("scripts/") + script + ".lua";
    
    if (luaL_loadfile(L, path.c_str())) {
        Log(Severity::ERROR, System::MISC, "Was an error in loading {} \n {}", path, lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
        Log(Severity::ERROR, System::MISC, "Was an error in executing {} \n {}", path, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

static value_t evaluate(const char* script) {
    if (luaL_loadbuffer(L, script, strlen(script), "eval_buffer")) {
        Log(Severity::ERROR, System::MISC, "Was an error in loading {} \n {}", script, lua_tostring(L, -1));
        lua_pop(L, 1);
        return value_t();
    }

    if (lua_pcall(L, 0, 1, 0)) {
        Log(Severity::ERROR, System::MISC, "Was an error in executing {} \n {}", script, lua_tostring(L, -1));
        lua_pop(L, 1);
        return value_t();
    }
    
    value_t ret = get_value_from_stack(lua_gettop(L), best_value(lua_gettop(L)));
    lua_pop(L, 1);
    return ret;
}

class Lua : public Script::Interface {
    name_t GetName() {
        return "lua";
    }
    
    value_t GetGlobal(name_t name) {
        return get_global(name);
    }
    
    void SetGlobal(name_t name, value_t value) {
        set_global(name, value);
    }
    
    void SetFunction(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t)) {
        set_function(name, parameters, function);
    }
    
    value_t CallFunction(name_t name, std::vector<Value> parameters) {
        return call_function(name, parameters);
    }
    
    void LoadScript(const char* path) {
        load_script(path);
    }
    
    value_t Evaluate(const char* code) {
        return evaluate(code);
    }
};

void Init() {
    if (L) {
        return; // TODO: add error
    }
    
    L = luaL_newstate();
    luaL_openlibs(L);

    Script::SetInterface(new Lua);
}

void Uninit() {
    if (!L) {
        return; // TODO: add error
    }
    
    lua_close(L);
    
    L = nullptr;
}

}
#include <extensions/scripting/lua.h>
#include <framework/script.h>
#include <framework/logging.h>

#include <lua.hpp>

#include <iostream>

namespace tram::Ext::Scripting::Lua {

static lua_State* L = nullptr;

static Type best_value(int index) {
    switch (lua_type(L, index)) {
        case LUA_TNUMBER:   return TYPE_FLOAT;
        case LUA_TBOOLEAN:  return TYPE_BOOL;
        case LUA_TSTRING:   return TYPE_NAME;
        case LUA_TTABLE:    {
            lua_getfield(L, index, "x");
            lua_getfield(L, index, "y");
            lua_getfield(L, index, "z");
            lua_getfield(L, index, "w");
            
            bool has_x = lua_tonumber(L, -4) == LUA_TNUMBER;
            bool has_y = lua_tonumber(L, -3) == LUA_TNUMBER;
            bool has_z = lua_tonumber(L, -2) == LUA_TNUMBER;
            bool has_w = lua_tonumber(L, -1) == LUA_TNUMBER;
            
            lua_pop(L, 4);
            
            if (has_x && has_y && has_z && !has_w) {
                std::cout << "returning vec3" << std::endl;
                return TYPE_VEC3;
            }
            
            if (has_x && has_y && has_z && has_w) {
                std::cout << "returning quat" << std::endl;
                return TYPE_QUAT;
            }
            
            std::cout << "returning other" << std::endl;
            
            return TYPE_UNDEFINED;
        }
        
        case LUA_TNIL:
        default: 
        
            return TYPE_UNDEFINED;
        }
}

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

static value_t get_value_from_stack(int index, Type type) {
    switch (type) {
        case TYPE_UNDEFINED:
            if (lua_isboolean(L, index)) {
                return (bool) lua_toboolean(L, index);
            } else if (lua_isstring(L, index)) {
                return (const char*) lua_tostring(L, index);
            } else if (lua_isinteger(L, index)) {
                return (int32_t) lua_tointeger(L, index);
            } else if (lua_isnumber(L, index)) {
                return (float) lua_tonumber(L, index);
            } else {
                return value_t();
            }
            break;
        
        case TYPE_BOOL:         return (bool) lua_toboolean(L, index);
        
        case TYPE_NAME:         return (name_t) lua_tostring(L, index);
        case TYPE_STRING:       return (const char*) lua_tostring(L, index);
        
        case TYPE_INT8:         return (int8_t) lua_tointeger(L, index);
        case TYPE_INT16:        return (int16_t) lua_tointeger(L, index);
        case TYPE_INT:
        case TYPE_INT32:        return (int32_t) lua_tointeger(L, index);
        case TYPE_INT64:        return (int64_t) lua_tointeger(L, index);
        case TYPE_UINT8:        return (uint8_t) lua_tointeger(L, index);
        case TYPE_UINT16:       return (uint16_t) lua_tointeger(L, index);
        case TYPE_UINT:
        case TYPE_UINT32:       return (uint32_t) lua_tointeger(L, index);
        case TYPE_UINT64:       return (uint64_t) lua_tointeger(L, index);
        
        case TYPE_FLOAT: 
        case TYPE_FLOAT32:      return (float) lua_tonumber(L, index);
        case TYPE_FLOAT64:      return (double) lua_tonumber(L, index);
        
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
    
/*static value_t get_global(name_t name) {
    idk if I will even ever use this one
}*/

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
        Log(System::SYSTEM_MISC, SEVERITY_ERROR, "Was an error in loading {} \n {}", path, lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
        Log(System::SYSTEM_MISC, SEVERITY_ERROR, "Was an error in executing {} \n {}", path, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}



void Init() {
    if (L) {
        return; // TODO: add error
    }
    
    L = luaL_newstate();
    luaL_openlibs(L);
    
    Script::Language lua;
    
    lua.name = "Lua";
    lua.load_script = load_script;
    lua.call_function = call_function;
    lua.set_function = set_function;
    lua.get_global = nullptr;
    lua.set_global = set_global;
    
    Script::SetLanguage(lua);
}

void Uninit() {
    if (!L) {
        return; // TODO: add error
    }
    
    lua_close(L);
    
    L = nullptr;
}

}
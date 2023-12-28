#include <extensions/scripting/lua.h>
#include <framework/script.h>

#include <lua/lua.hpp>



namespace tram::Ext::Scripting::Lua {

static lua_State* L = nullptr;

Type best_value(int index) {
    switch (lua_type(L, index)) {
        case LUA_TNUMBER:   return TYPE_FLOAT;
        case LUA_TBOOLEAN:  return TYPE_BOOL;
        case LUA_TSTRING:   return TYPE_NAME;
        case LUA_TTABLE:    return TYPE_VEC3;
        
        case LUA_TNIL:
        default: 
        
            return TYPE_UNDEFINED;
        }
}

void push_value_to_stack(const value_t& value) {
    if (value.IsBool()) {
        lua_pushboolean(L, (bool)value);        return;
    }
    
    if (value.IsInt()) {
        lua_pushinteger(L, value.GetInt());     return;
    }
    
    if (value.IsFloat()) {
        lua_pushinteger(L, value.GetFloat());   return;
    }
    
    switch (value.GetType()) {
        case TYPE_NAME:     lua_pushstring(L, (name_t)value);       return;
        case TYPE_STRING:   lua_pushstring(L, (const char*)value);  return;
        case TYPE_VEC3:
            lua_createtable(L, 0, 3);
            lua_pushnumber(L, ((vec3)value).x); lua_setfield(L, -2, "0");
            lua_pushnumber(L, ((vec3)value).y); lua_setfield(L, -2, "1");
            lua_pushnumber(L, ((vec3)value).z); lua_setfield(L, -2, "2");
            return;
        default: lua_pushnil(L);
    }
}

value_t get_value_from_stack(int index, Type type) {
    switch (type) {
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
            lua_rawgeti(L, index, 1);
            lua_rawgeti(L, index, 2);
            lua_rawgeti(L, index, 3);
            vec3 vec = {lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1)};
            lua_pop(L, 3);
            return vec;
        }
        default: return value_t();
    }
}

const size_t MAX_LUA_FUNCTIONS = 100;

struct {
    std::vector<Type> parameters;
    value_t (*function)(valuearray_t) = nullptr;
} registered_functions[MAX_LUA_FUNCTIONS];
static size_t registered_function_count = 0;

// this function will get called from the lua vm and in turn will then find and
// call whichever its name was registered to
extern "C" {
static int function_call_from_lua(lua_State* L) {
    return 0;
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
    // TODO: add check if registered_function_count not exceeding max functions
    registered_functions[registered_function_count] = {parameters, function};
    lua_pushinteger(L, registered_function_count);
    lua_pushcclosure(L, function_call_from_lua, 1);
    registered_function_count++;
}

static value_t call_function(name_t name, std::vector<Value> parameters) {
    lua_getglobal(L, name);
	for (auto& val : parameters) push_value_to_stack(val);
	lua_call(L, parameters.size(), 1);
    value_t ret = get_value_from_stack(lua_gettop(L), TYPE_BOOL);
    lua_pop(L, 1);
    return ret;
}




void Init() {
    if (L) {
        return; // TODO: add error
    }
    
    L = luaL_newstate();
    luaL_openlibs(L);
}

void LoadScript(const char* script) {

}

void Uninit() {
    if (!L) {
        return; // TODO: add error
    }
}

}
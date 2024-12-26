// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/kitchensink/ai.h>

#include <framework/script.h>
#include <render/render.h>
#include <entities/sound.h>

#include <templates/hashmap.h>
#include <templates/pool.h>

//template <> tram::Pool<tram::Ext::Kitchensink::AIPackage> tram::PoolProxy<tram::Ext::Kitchensink::AIPackage>::pool("ai_package_list pool", 100);
template <> tram::Pool<tram::Ext::Kitchensink::AIAgent> tram::PoolProxy<tram::Ext::Kitchensink::AIAgent>::pool("ai_agent_list pool", 100);

namespace tram::Ext::Kitchensink {

static Hashmap<ai_package_constr> ai_package_list("ai_package_list name list", 200);
static Hashmap<AIAgent*> ai_agent_list("ai_agent_list name list", 200);

void InitAI() {
    // TODO: register screipt functions for AIs
    // AIGetVariable()
    // AISetVariable()
    // AIPushPackage()
    // AIClearPackage()
    // AIHasPackages()
}

void UpdateAI() {
    for (auto& agent : PoolProxy<tram::Ext::Kitchensink::AIAgent>::GetPool()) agent.Update();
}

void AIPackage::Register(name_t name, ai_package_constr constr) {
    return ai_package_list.Insert(name, constr);
}

AIPackage* AIPackage::Make(name_t name, valuearray_t params) {
    ai_package_constr constr = ai_package_list.Find(name);
    
    if (!constr) {
        std::cout << "AIPackage " << name << " not found!" << std::endl;
        return nullptr;
    } else {
        return constr(params);
    }
}

void AIAgent::Update() {
    if (override_stack.size()) {
        override_stack.front()->Process();
        return;
    }
    
    if (package_stack.size()) {
        package_stack.front()->Process();
    }
    
    if (script) {
        // TODO: load varibales etc.
        Script::CallFunction(script, {});
    }
}
    
AIAgent* AIAgent::Find(id_t id) {
    AIAgent* ai_agent = ai_agent_list.Find(id);
    
    if (!ai_agent) {
        ai_agent = PoolProxy<AIAgent>::New();
        ai_agent_list.Insert(id, ai_agent);
    }
    
    return ai_agent;
}
}

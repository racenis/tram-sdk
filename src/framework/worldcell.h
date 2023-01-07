// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_WORLDCELL_H
#define FRAMEWORK_WORLDCELL_H

#include <vector>

#include <framework/uid.h>
#include <framework/math.h>

namespace Core {
    class Entity;
    class Path;
    class Navmesh;
    
    class WorldCell {
    public:
        class Transition {
        public:
            void AddPoint(const vec3& point);
            void GeneratePlanes(bool disp = false);
            bool IsInside(const vec3& point);
            void SetInto(WorldCell* new_into) { into = new_into; }
            WorldCell* GetInto() { return into; }
        protected:
            name_t name = 0;
            WorldCell* into;
            std::vector<vec3> points;
            std::vector<vec4> planes;
            friend class WorldCell;
        };
        class Loader {
        public:
            void SetLocation(const vec3& new_location) { location = new_location; current_cell = Find(new_location); }
            void UpdateLocation(const vec3& new_location) { location = new_location; auto n_trans = current_cell->FindTransition(location); if (n_trans) { current_cell = n_trans; } }
            static void LoadCells();
        private:
            vec3 location;
            WorldCell* current_cell;
        };
    protected:
        name_t name = 0;
        bool interior = false;
        bool interiorLights = false;    // why is this in camelCase
        bool loaded = false;
        bool draw = false;
        std::vector<Entity*> entities;
        std::vector<Transition*> trans_in;  // rename
        std::vector<Transition*> trans_out; // this one too?
        std::vector<Path*> paths;
        std::vector<Navmesh*> navmeshes;
        friend void LoadCells();
    public:
        WorldCell(){};

        bool IsLoaded() {return loaded;};

        bool IsInterior() {return interior;};
        bool HasInteriorLighting() {return interiorLights;};
        bool IsDrawn() {return draw;};
        
        void SetInterior(bool interior) { this->interior = interior; }
        void SetInteriorLights(bool interior_lights) { this->interiorLights = interior_lights; }
        
        name_t GetName(){return name;};   

        void SetName(name_t new_name);
        
        // what is this and why do we need this?
        void SetDrawn(bool drawn) { draw = drawn; }

        void Load();

        void Unload();

        void LoadFromDisk();
        
        void DrawTransitions();
        void DrawNavmeshes();
        void DrawPaths();

        void AddEntity(Entity* entPtr);

        void RemoveEntity(Entity* entPtr);
        
        size_t EntityCount() { return entities.size(); }

        // Adds a transition *into* the cell.
        void AddTransition(Transition* transPtr){
            trans_in.push_back(transPtr);
            transPtr->SetInto(this);
        };

        // Adds a transition *from* the cell.
        void AddTransitionFrom(Transition* transPtr){
            trans_out.push_back(transPtr);
        };

        void AddLink(WorldCell* cell){
            trans_out.insert(trans_out.end(), cell->trans_in.begin(), cell->trans_in.end());
        };

        // Find if a point is in a transition *out of* the cell.
        inline WorldCell* FindTransition(glm::vec3& point){
            for(size_t i = 0; i < trans_out.size(); i++){
                if(trans_out[i]->IsInside(point)) return trans_out[i]->GetInto();
            }
            return nullptr;
        }

        // Checks if a point is inside the cell.
        bool IsInside(const glm::vec3& point){
            for(size_t i = 0; i < trans_in.size(); i++){
                if(trans_in[i]->IsInside(point)) return true;
            }
            return false;
        }

        static WorldCell* Find (const glm::vec3& point);
        static WorldCell* Find (name_t name);
    };
}

#endif // FRAMEWORK_WORLDCELL_H
// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDER.CPP -- Rendering stuff.

#include <core.h>
#include <render/render.h>
#include <render/renderer.h>
#include <components/rendercomponent.h>
#include <components/spritecomponent.h>
#include <components/particlecomponent.h>

using namespace Core;

namespace Core::Render {
    glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat CAMERA_ROTATION = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    float CAMERA_PITCH = 0.0f;
    float CAMERA_YAW = -90.0f;

    float SCREEN_WIDTH = 800.0f;
    float SCREEN_HEIGHT = 600.0f;

    bool THIRD_PERSON = false;
    
    glm::vec3 SUN_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 SUN_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 AMBIENT_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

    // move these into gui.cpp?
    Material FONT_REGULAR;
    Material FONT_TITLE;
    Material FONT_SYMBOLS;
    Material GLYPH_GUI;
    Material GLYPH_TEXT;
    Material GLYPH_TEXT_BOLD;
    Material GLYPH_HEADERS;

    float FRAME_LIMIT = 60.0f;

    float time_of_day = 0.8f;


    Pool<RenderListObject> renderList("render list", 500, false);
    Pool<LightListObject> lightPool("lightpool", 100, true);
    Octree<uint32_t> lightTree;
    
    std::vector<GeometryBatch> GeometryBatch::geometry_batches;

    std::vector<LineVertex> colorlines;
    std::vector<SpriteVertex> textvertices;
    std::vector<SpriteVertex> glyphvertices;
    
    GeometryBatch* glyph_batch = nullptr;
    

    void Init(){
        OpenGL::Init();
        
        FONT_REGULAR = Material(UID("jost"), Material::TEXTURE_MSDF);  // futura knock-off
        FONT_TITLE = Material(UID("inter"), Material::TEXTURE_MSDF);    // helvetica
        FONT_SYMBOLS = Material(UID("symbols"), Material::TEXTURE_MSDF);  // ornamentation and other non-text glyphs
        GLYPH_GUI = Material(UID("glyph_symbols"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs
        GLYPH_TEXT = Material(UID("glyph_text"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs
        GLYPH_TEXT_BOLD = Material(UID("glyph_text_bold"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs
        GLYPH_HEADERS = Material(UID("glyph_headers"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs

        FONT_REGULAR.Load();
        FONT_TITLE.Load();
        FONT_SYMBOLS.Load();
        GLYPH_GUI.Load();
        GLYPH_TEXT.Load();
        GLYPH_TEXT_BOLD.Load();
        GLYPH_HEADERS.Load();
        
        uint32_t throwaway;
        GeometryBatch::Find(throwaway, glyph_batch, &GLYPH_GUI, Material::TEXTURE_GLYPH, Model::SPRITE_VERTEX);
        GeometryBatch::Find(throwaway, glyph_batch, &GLYPH_TEXT, Material::TEXTURE_GLYPH, Model::SPRITE_VERTEX);
        GeometryBatch::Find(throwaway, glyph_batch, &GLYPH_TEXT_BOLD, Material::TEXTURE_GLYPH, Model::SPRITE_VERTEX);
        GeometryBatch::Find(throwaway, glyph_batch, &GLYPH_HEADERS, Material::TEXTURE_GLYPH, Model::SPRITE_VERTEX);
        glyph_batch->render_object->flags = glyph_batch->render_object->flags | FLAG_NO_DEPTH_TEST;
    }

    void Render(){
        #ifndef ENGINE_EDITOR_MODE
        for (auto& it : PoolProxy<SpriteComponent>::GetPool()) it.Update();
        for (auto& it : PoolProxy<ParticleComponent>::GetPool()) it.Update();
        GeometryBatch::Update();
        #endif // ENGINE_EDITOR_MODE
        
        OpenGL::Render();
    }
    
    void ScreenSize(float width, float height) {
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        OpenGL::ScreenSize(width, height);
    }
    
    void RenderListObject::FillFromModel(Model* mdl, uint32_t eboIndex){
        //vbo = mdl->vbo;
        //ebo = mdl->ebo;
        vao = mdl->vao;
        eboLen = mdl->eboLen[eboIndex];
        eboOff = mdl->eboOff[eboIndex];
        shader = OpenGL::FindShader(mdl->vertForm, (Material::Type)mdl->eboMat[eboIndex]);
        texCount = mdl->materials.size();
        for (uint32_t i = 0; i < texCount; i++){
            textures[i] = mdl->materials[i]->GetTexture();
        }

        flags = FLAG_RENDER | FLAG_DRAW_INDEXED; // TODO: get the RFLAG_INTERIOR_LIGHTING in here somehow
    };
    
    void AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color){
        LineVertex vert;

        vert.co[0] = from[0];
        vert.co[1] = from[1];
        vert.co[2] = from[2];
        vert.color[0] = color[0];
        vert.color[1] = color[1];
        vert.color[2] = color[2];

        colorlines.push_back(vert);

        vert.co[0] = to[0];
        vert.co[1] = to[1];
        vert.co[2] = to[2];
        vert.color[0] = color[0];
        vert.color[1] = color[1];
        vert.color[2] = color[2];

        colorlines.push_back(vert);
    }
    
    void SetGlyph(float x, float y, float tex_x, float tex_y, float width, float height, float scale_x, float scale_y, float thickness, uint32_t texture, const glm::vec3& color){
        SpriteVertex tleft;   // top left
        SpriteVertex tright;  // top right
        SpriteVertex bleft;   // bottom left
        SpriteVertex bright;  // bottom right

        thickness = thickness - 1.0f;

        glm::vec2 scale;
        scale.x = scale_x * 2;
        scale.y = scale_y * 2;

        tleft.co.x = x;
        tleft.co.y = y;
        tleft.texco.x = tex_x;
        tleft.texco.y = tex_y;

        tright.co.x = x + width * scale_x;
        tright.co.y = y;
        tright.texco.x = tex_x + width;
        tright.texco.y = tex_y;

        bleft.co.x = x;
        bleft.co.y = y + height * scale_y;
        bleft.texco.x = tex_x;
        bleft.texco.y = tex_y + height;

        bright.co.x = x + width * scale_x;
        bright.co.y = y + height * scale_y;
        bright.texco.x = tex_x + width;
        bright.texco.y = tex_y + height;

        tleft.color = color;
        tleft.voffset = scale;
        tleft.texture = texture;
        tleft.verticality = thickness;
        tright.color = color;
        tright.voffset = scale;
        tright.texture = texture;
        tright.verticality = thickness;
        bleft.color = color;
        bleft.voffset = scale;
        bleft.texture = texture;
        bleft.verticality = thickness;
        bright.color = color;
        bright.voffset = scale;
        bright.texture = texture;
        bright.verticality = thickness;

        textvertices.push_back(bleft);
        textvertices.push_back(bright);
        textvertices.push_back(tleft);
        textvertices.push_back(bright);
        textvertices.push_back(tright);
        textvertices.push_back(tleft);
    }
    
    void SetGlyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex){
        SpriteVertex tleft;   // top left
        SpriteVertex tright;  // top right
        SpriteVertex bleft;   // bottom left
        SpriteVertex bright;  // bottom right

        tleft.co.x = x;
        tleft.co.y = y;
        tleft.texco.x = tex_x;
        tleft.texco.y = tex_y;
        
        tright.co.x = x + w;
        tright.co.y = y;
        tright.texco.x = tex_x + tex_w;
        tright.texco.y = tex_y;
        
        bleft.co.x = x;
        bleft.co.y = y + h;
        bleft.texco.x = tex_x;
        bleft.texco.y = tex_y + tex_h;
        
        bright.co.x = x + w;
        bright.co.y = y + h;
        bright.texco.x = tex_x + tex_w;
        bright.texco.y = tex_y + tex_h;
        
        tleft.color = color;
        tleft.texture = tex;
        tright.color = color;
        tright.texture = tex;
        bleft.color = color;
        bleft.texture = tex;
        bright.color = color;
        bright.texture = tex;

        //glyphvertices.push_back(bleft);
        //glyphvertices.push_back(bright);
        //glyphvertices.push_back(tleft);
        //glyphvertices.push_back(bright);
        //glyphvertices.push_back(tright);
        //glyphvertices.push_back(tleft);
        
        glyph_batch->sprite_vector->push_back(bleft);
        glyph_batch->sprite_vector->push_back(bright);
        glyph_batch->sprite_vector->push_back(tleft);
        glyph_batch->sprite_vector->push_back(bright);
        glyph_batch->sprite_vector->push_back(tright);
        glyph_batch->sprite_vector->push_back(tleft);
    }
}


namespace Core {
    void RenderComponent::Uninit(){
        is_ready = true;
        pose = nullptr;

        for (size_t i = 0; i < 7; i++){
            if(rLsObjPtr[i] == nullptr) continue;

            Render::renderList.Remove(rLsObjPtr[i]);

            rLsObjPtr[i] = nullptr;
        }
    };

    void RenderComponent::Start(){
        if(is_ready) return;


        uint32_t pointers = 0;
        for(uint32_t i = 0; i < 6; i++){
            if (model->IsEBOEmpty(i)) continue;
            rLsObjPtr[pointers] = Render::renderList.AddNew();
            rLsObjPtr[pointers]->FillFromModel(model.GetResource(), i);
            if(lightmap.GetResource() != nullptr) rLsObjPtr[pointers]->lightmap = lightmap->GetTexture();
            rLsObjPtr[pointers]->pose = pose;
            pointers++;

        }

        is_ready = true;

        UpdateRenderListObjs();

        return;
    }

    void RenderComponent::UpdateRenderListObjs(){
        if (!is_ready) return;

        for(uint32_t i = 0; rLsObjPtr[i] != nullptr; i++){
            rLsObjPtr[i]->location = location;
            rLsObjPtr[i]->rotation = rotation;

            rLsObjPtr[i]->pose = pose;

            rLsObjPtr[i]->flags &= -1 ^ Render::FLAG_INTERIOR_LIGHTING;
            rLsObjPtr[i]->flags |= Render::FLAG_INTERIOR_LIGHTING * isInterior;

            rLsObjPtr[i]->lights[0] = 0;
            rLsObjPtr[i]->lights[1] = 0;
            rLsObjPtr[i]->lights[2] = 0;
            rLsObjPtr[i]->lights[3] = 0;
            Render::lightTree.FindNearest(rLsObjPtr[i]->lights, rLsObjPtr[i]->location.x, rLsObjPtr[i]->location.y, rLsObjPtr[i]->location.z);
        }

    }

}

#include <gui.h>

namespace Core::GUI {
    struct FrameObject {
        float offset_x;
        float offset_y;
        float width;
        float height;
        float cursor_x;
        float cursor_y;
    };
    
    Stack<FrameObject> FrameStack ("GUI Frame stack", 100);
    
    void Glyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex) {
        auto& t = FrameStack.top();
        if (x < t.offset_x || y < t.offset_y || x + w > t.offset_x + t.width || y + h > t.offset_y + t.height) {
            if (x + w < t.offset_x || y + h < t.offset_y || x > t.offset_x + t.width || y > t.offset_y + t.height) {
                return;
            }
            float n_x = x, n_y = y, n_w = w, n_h = h, n_tex_x = tex_x, n_tex_y = tex_y, n_tex_w = tex_w, n_tex_h = tex_h;
            if (x < t.offset_x) {
                float off = t.offset_x - x;
                n_x += off; n_tex_x += off;
                n_w -= off; n_tex_w -= off;
            }
            if (y < t.offset_y) {
                float off = t.offset_y - y;
                n_y += off; n_tex_y += off;
                n_h -= off; n_tex_h -= off;
            }
            if (x + w > t.offset_x + t.width) {
                float off = (x + w) - (t.offset_x + t.width);
                n_w -= off; n_tex_w -= off;
            }
            if (y + h > t.offset_y + t.height) {
                float off = (y + h) - (t.offset_y + t.height);
                n_h -= off; n_tex_h -= off;
            }
            Render::SetGlyph(n_x, n_y, n_w, n_h, n_tex_x, n_tex_y, n_tex_w, n_tex_h, color, tex);
            return;
        }
        Render::SetGlyph(x, y, w, h, tex_x, tex_y, tex_w, tex_h, color, tex);
    }
    
    void Begin() {
        FrameStack.Reset();
        
        *FrameStack.AddNew() = FrameObject {0.0f, 0.0f, Render::SCREEN_WIDTH, Render::SCREEN_HEIGHT, 0.0f, 0.0f};
    }
    
    
    void End() {
        
        
        
    }
    
    void Frame(orientation frame_orient, float offset) {
        auto& last = FrameStack.top();
        switch (frame_orient) {
            case FRAME_LEFT:
                *FrameStack.AddNew() = FrameObject {last.offset_x, last.offset_y, offset, last.height, 0.0f, 0.0f};
                break;
            case FRAME_RIGHT:
                *FrameStack.AddNew() = FrameObject {last.offset_x + last.width - offset, last.offset_y, offset, last.height, 0.0f, 0.0f};
                break;
            case FRAME_TOP:
                *FrameStack.AddNew() = FrameObject {last.offset_x, last.offset_y, last.width, offset, 0.0f, 0.0f};
                break;
            case FRAME_BOTTOM:
                *FrameStack.AddNew() = FrameObject {last.offset_x, last.offset_y + last.height - offset, last.width, offset, 0.0f, 0.0f};
                break;
            default:
                abort();
        }
    }
    
    void Frame(orientation frame_orient, float width, float height) {
        // TODO: implement the rest of the orientations
        if (frame_orient != FRAME_CENTER) abort();
        
        float& pos_x = FrameStack.top().offset_x;
        float& pos_y = FrameStack.top().offset_y;
        float& size_x = FrameStack.top().width;
        float& size_y = FrameStack.top().height;
        
        *FrameStack.AddNew() = FrameObject {((size_x - width) / 2.0f) + pos_x, ((size_y - height) / 2.0f) + pos_y, width, height, 0.0f, 0.0f};
    }
    
    void FrameBorder() {
        auto& t = FrameStack.top();
        const float s = 0.001f;
        glm::vec3 tl(t.offset_x * s, t.offset_y * s, 0.0f);
        glm::vec3 tr((t.offset_x + t.width) * s, t.offset_y * s, 0.0f);
        glm::vec3 bl(t.offset_x * s, (t.offset_y + t.height) * s, 0.0f);
        glm::vec3 br((t.offset_x + t.width) * s, (t.offset_y + t.height) * s, 0.0f);
        Render::AddLine(tl, tr, Render::COLOR_PINK);
        Render::AddLine(tr, br, Render::COLOR_PINK);
        Render::AddLine(br, bl, Render::COLOR_PINK);
        Render::AddLine(bl, tl, Render::COLOR_PINK);
    }
    
        
    void EndFrame() {
        FrameStack.Remove();
    }
    
    
    
}
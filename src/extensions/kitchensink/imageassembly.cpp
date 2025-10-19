#include <extensions/kitchensink/imageassembly.h>

#include <framework/file.h>

namespace tram::Kitchensink::ImageAssembly {

enum {
    OP_NOOP,        // 8 op
    OP_1,
    OP_MOVE,        // 8 op, 4 dst 4 src
    OP_RETURN,      // 8 op
    OP_CONSTANT,    // 8 op, 4 dst 4 nul, 32 constant 
    OP_SAMPLE,      // 8 op, 4 dst 4 nul, 16 16 coords
    OP_6,
    OP_7,
    OP_ADD,         // 8 op, 4 dst 4 src
    OP_MULTIPLY,    // 8 op, 4 dst 4 src
};
    
class Operation {
public:
    virtual void Emit(File& file) = 0;
    virtual int GetLength() = 0;

    void SetRegisterSrc(uint8_t reg) {
        src_reg = reg;
    }
    
    void SetRegisterDst(uint8_t reg) {
        dst_reg = reg;
    }
    
protected:
    uint8_t GetPackedRegister() {
        uint8_t result = dst_reg & 0xF;
        result <<= 4;
        result |= src_reg & 0xF;
        return result;
    }

    uint8_t src_reg = 0;
    uint8_t dst_reg = 0;
};

class Move : public Operation {
public:
    void Emit(File& file) override {
        file.write_uint8(OP_MOVE);
        file.write_uint8(GetPackedRegister());
    }
    
    int GetLength() override {
        return 2;
    }
};

class Return : public Operation {
public:
    void Emit(File& file) override {
        file.write_uint8(OP_RETURN);
    }
    
    int GetLength() override {
        return 1;
    }
};

class Constant : public Operation {
public:
    void Emit(File& file) override {
        file.write_uint8(OP_CONSTANT);
        file.write_uint8(GetPackedRegister());
        
        vec4 clamped = glm::clamp(color, 0.0f, 1.0f);
        
        file.write_uint8(clamped.x * 255.0f);
        file.write_uint8(clamped.y * 255.0f);
        file.write_uint8(clamped.z * 255.0f);
        file.write_uint8(clamped.w * 255.0f);
    }
    
    void SetColor(vec4 color) {
        this->color = color;
    }
    
    int GetLength() override {
        return 6;
    }
    
private:
    vec4 color;
};

class Sample : public Operation {
public:
    void Emit(File& file) override {
        file.write_uint8(OP_SAMPLE);
        file.write_uint8(GetPackedRegister());
        
        file.write_uint16(x);
        file.write_uint16(y);
    }
    
    void SetCoordX(uint16_t x) {
        this->x = x;
    }
    
    void SetCoordY(uint16_t y) {
        this->y = y;
    }
    
    int GetLength() override {
        return 6;
    }
    
private:
    uint16_t x;
    uint16_t y;
};

class Addition : public Operation {
public:
    void Emit(File& file) override {
        file.write_uint8(OP_ADD);
        file.write_uint8(GetPackedRegister());
    }
    
    int GetLength() override {
        return 2;
    }
};

class Multiply : public Operation {
public:
    void Emit(File& file) override {
        file.write_uint8(OP_MULTIPLY);
        file.write_uint8(GetPackedRegister());
    }
    
    int GetLength() override {
        return 2;
    }
};
    
class Pixel {
public:
    Pixel(const std::vector<Render::API::AssemblyLayer>& layers) {
        // final pixel value in register 0
        // all registers are initialized to zeros
        
        for (const auto& layer : layers) {
            
            // if there is something in register 0 from previous raytrace
            // bounces and this next surface has some reflectivity, compute
            // how much of that previous color will be kept
            if (operations.size() && layer.sublayer_opacity < 1.0f) {
                Constant* opacity = new Constant;
                opacity->SetRegisterDst(1);
                opacity->SetColor({layer.sublayer_opacity,
                                   layer.sublayer_opacity,
                                   layer.sublayer_opacity,
                                   layer.sublayer_opacity});
                
                Multiply* multiply = new Multiply;
                multiply->SetRegisterSrc(1);
                multiply->SetRegisterDst(0);
                
                operations.push_back(opacity);
                operations.push_back(multiply);
            }
            
            // register 0 has colors from previous bounces, i.e. reflections
            
            // now we will load diffuse + texture into register 1
            
            if (layer.texture < 0) {
                Constant* diffuse = new Constant;
                diffuse->SetRegisterDst(1);
                diffuse->SetColor(layer.diffuse);
                
                operations.push_back(diffuse);
            } else {
                Constant* diffuse = new Constant;
                diffuse->SetRegisterDst(1);
                diffuse->SetColor(layer.diffuse);
                
                Sample* texture = new Sample;
                texture->SetRegisterDst(2);
                texture->SetRegisterSrc(layer.texture);
                texture->SetCoordX(layer.sample_x);
                texture->SetCoordY(layer.sample_y);
                
                Multiply* multiply = new Multiply;
                multiply->SetRegisterDst(1);
                multiply->SetRegisterSrc(2);
                
                operations.push_back(diffuse);
                operations.push_back(texture);
                operations.push_back(multiply);
            }
            
            // register 0 has reflections
            // register 1 has diffuse + texture
            
            Addition* addition = new Addition;
            addition->SetRegisterDst(0);
            addition->SetRegisterSrc(1);
            
            operations.push_back(addition);
            
            // register 0 has reflections + diffuse + texture
            
            Constant* specular = new Constant;
            specular->SetRegisterDst(1);
            specular->SetColor(layer.specular);
            
            operations.push_back(specular);
            operations.push_back(addition);
            
            // register 0 has reflections + diffuse + texture
        }
        
        // finally we can return the pixel value
        operations.push_back(new Return);
    }
    
    int GetLength() {
        int len = 0;
        for (auto operation : operations) len += operation->GetLength();
        return len;
    }
    
    void Emit(File& file) {
        for (auto operation : operations) operation->Emit(file);
    }

private:
    std::vector<Operation*> operations;
};

class Frame {
public:
    Frame(const Render::API::AssemblyLayers& layers) {
        width = layers.width;
        height = layers.height;
        
        for (auto& pixel : layers.pixels) {
            pixels.push_back(Pixel(pixel));
        }
    }
    
    void Emit(File& file) {
        
        // emit frame header
        file.write_uint8('F'); file.write_uint8('R'); file.write_uint8('M'); file.write_uint8('E');
        
        // emit frame dimensions
        file.write_uint16(width);
        file.write_uint16(height);
        
        // emit frame length
        int len = 0;
        for (auto& pixel : pixels) {
            len += pixel.GetLength();
        }
        file.write_int32(len);
        
        // emit pixels
        for (auto& pixel : pixels) {
            pixel.Emit(file);
        }
    }
    
private:
    int width = -1;
    int height = -1;
    
    std::vector<Pixel> pixels;
};
    
class Image {
public:
    void AddFrame(const Render::API::AssemblyLayers& layers) {
        if (width < 0) {
            width = layers.width;
            height = layers.height;
        } else if (width != layers.width && height != layers.height) {
            std::cout << "Image set to " << width << "x" << height << " but fed a frame " << layers.width << "x" << layers.height << std::endl;
        }

        textures = layers.textures;
        
        frames.push_back(Frame(layers));
    }
    
    void Emit(File& file) {
        
        // emit filetype header
        file.write_uint8('I'); file.write_uint8('M'); file.write_uint8('G'); file.write_uint8('B');
        file.write_uint8('C'); file.write_uint8('O'); file.write_uint8('D'); file.write_uint8('E');
        
        // emit filetype version
        file.write_uint16(1);
        
        // emit flags
        file.write_uint16(0);
        
        // emit input length
        file.write_uint16(textures.size() + 1);
        
        // emit frame count
        file.write_uint16(frames.size());
        
        // emit input metadata
        file.write_uint8(0);
        file.write_uint8(0);
        file.write_uint16(0);
        file.write_uint16(width);
        file.write_uint16(height);
        
        for (auto& texture : textures) {
            file.write_uint8(texture.index);
            file.write_uint8(0);
            file.write_uint16(0);
            file.write_uint16(texture.width);
            file.write_uint16(texture.height);
        }
        
        // emit all of the frames
        for (auto& frame : frames) {
            frame.Emit(file);
        }
    }

private:
    int width = -1;
    int height = -1;
    
    std::vector<Render::API::AssemblyTexture> textures;
    std::vector<Frame> frames;
};


    

    


Image* image = nullptr;

void Add(Render::API::AssemblyLayers layers) {
    if (!image) image = new Image;
    
    image->AddFrame(layers);
}

void Reset() {
    if (image) {
        delete image;
        image = nullptr;
    }
}

void Save(const char* filename) {
    File file(filename, File::WRITE | File::BINARY);

    if (image) {
        image->Emit(file);
    }
}

}

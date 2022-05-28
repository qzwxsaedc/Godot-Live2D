//
// Created by qzwxsaedc on 2022/5/27.
//

#ifndef LIBGDLIVE2D_TEXTUREMANAGER_H
#define LIBGDLIVE2D_TEXTUREMANAGER_H
#include <ImageTexture.hpp>
#include "../types.h"
#include "utils.h"

namespace Live2DWrapper {
    class TextureManager {
        godot::String root;
        godot::Ref<godot::ImageTexture>* textures;
        usize size;
    public:
        explicit TextureManager(godot::String& root_path, usize size);
        ~TextureManager();
        godot::Ref<godot::ImageTexture> load_texture_from_file(const godot::String& filename, usize idx);
        godot::Ref<godot::ImageTexture> get(usize idx);
    };
}


#endif //LIBGDLIVE2D_TEXTUREMANAGER_H

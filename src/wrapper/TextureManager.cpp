//
// Created by qzwxsaedc on 2022/5/27.
//

#include "TextureManager.h"
#include <Image.hpp>

using namespace Live2DWrapper;
TextureManager::TextureManager(godot::String& root_path, usize size) : root(std::move(root_path)), size(size) {
    textures = new godot::Ref<godot::ImageTexture>[size];
}

TextureManager::~TextureManager() {
    for(usize i = 0; i < size; ++i)
        textures[i].unref();
    delete textures;
}

godot::Ref<godot::ImageTexture> TextureManager::load_texture_from_file(const godot::String& path, usize idx) {
    if(!textures[idx].is_null())
        textures[idx].unref();
//    auto image = godot::Ref(godot::Image::_new());
    auto texture = godot::Ref(godot::ImageTexture::_new());
//    image->load(path);
    auto res = texture->load(path);
    godot_print("status={0}", static_cast<i32>(res));
    if(res == godot::Error::OK) {
        textures[idx] = texture;
        return texture;
    }
    godot_error("load texture failed. code={0}, path={1}", static_cast<i32>(res), path);
    return nullptr;
}

godot::Ref<godot::ImageTexture> TextureManager::get(usize idx) {
    if(idx > size) return nullptr;
    return textures[idx];
}

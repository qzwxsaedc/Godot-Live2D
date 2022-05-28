//
// Created by qzwxsaedc on 2022/5/26.
//

#ifndef LIBGDLIVE2D_UTILS_H
#define LIBGDLIVE2D_UTILS_H
#include "../types.h"
#include <String.hpp>
#include <Array.hpp>
#include "Variant.hpp"
#include <GodotGlobal.hpp>

using namespace Util;

namespace Live2DWrapper {
    bytes load_file_as_bytes(godot::String path, u32* size);
    void release_bytes(bytes& data);

    template <class... Args>
    inline godot::String format(const godot::String &fmt, Args... values) {
        return fmt.format(godot::Array::make(values...));
    }
    template<>
    inline godot::String format(const godot::String &fmt) {
        return fmt;
    }
}

#define godot_print(fmt, ...) godot::Godot::print(fmt, __VA_ARGS__)
#define godot_warning(fmt, ...) godot::Godot::print_warning(Live2DWrapper::format(fmt, __VA_ARGS__), __FUNCTIONW__, __FILE__, __LINE__)
#define godot_error(fmt, ...) godot::Godot::print_error(Live2DWrapper::format(fmt, __VA_ARGS__), __FUNCTIONW__, __FILE__, __LINE__)
#endif //LIBGDLIVE2D_UTILS_H

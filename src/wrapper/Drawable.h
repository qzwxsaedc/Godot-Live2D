//
// Created by qzwxsaedc on 2022/5/27.
//

#ifndef LIBGDLIVE2D_DRAWABLE_H
#define LIBGDLIVE2D_DRAWABLE_H
#include "../types.h"
using namespace Util;
namespace Live2DWrapper {
    // 不知道有些啥，干脆尽可能都拿过来
    struct Drawable {
        bool is_culling;
        bool inverted_mask;
        bool is_mask;
        i32 render_order;
        i32 draw_order;
        i32 texture_index;
        usize index;
        usize index_count;
        usize vertex_count;
        const u16* indices;
        const Live2D::Cubism::Core::csmVector2* positions;
        const Live2D::Cubism::Core::csmVector2* vertex_uvs;
        f32 opacity;
        Rendering::CubismBlendMode blend_mode;
    };
}

#endif //LIBGDLIVE2D_DRAWABLE_H

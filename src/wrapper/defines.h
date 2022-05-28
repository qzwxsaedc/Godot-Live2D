//
// Created by qzwxsaedc on 2022/5/26.
//

#ifndef LIBGDLIVE2D_DEFINES_H
#define LIBGDLIVE2D_DEFINES_H
#include "utils.h"
namespace Live2DWrapper::Define {
    const f32 ViewScale = 1.0f;
    const f32 ViewMaxScale = 2.0f;
    const f32 ViewMinScale = 0.8f;

    const f32 ViewLogicalLeft = -1.0f;
    const f32 ViewLogicalRight = 1.0f;
    const f32 ViewLogicalBottom = -1.0f;
    const f32 ViewLogicalTop = -1.0f;

    const f32 ViewLogicalMaxLeft = -2.0f;
    const f32 ViewLogicalMaxRight = 2.0f;
    const f32 ViewLogicalMaxBottom = -2.0f;
    const f32 ViewLogicalMaxTop = 2.0f;

    const c_str MotionGroupIdle = "Idle";
    const c_str MotionGroupTapBody = "TapBody";

    const c_str HitAreaNameHead = "Head";
    const c_str HitAreaNameBody = "Body";

    const i32 PriorityNone = 0;
    const i32 PriorityIdle = 1;
    const i32 PriorityNormal = 2;
    const i32 PriorityForce = 3;
}
#endif //LIBGDLIVE2D_DEFINES_H

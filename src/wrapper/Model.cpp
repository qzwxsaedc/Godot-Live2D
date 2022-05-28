//
// Created by qzwxsaedc on 2022/5/26.
//

#include "Model.h"
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Id/CubismIdManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include <random>
#include <ctime>
#include "defines.h"

using namespace Live2DWrapper;
using namespace Util;
using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;

Model::Model(const godot::String& root_dir, const godot::String& model_name)
: Csm::CubismUserModel()
, increment_accu_time_sec(.0f)
, root_dir(format("{0}/{1}", root_dir, model_name))
, model_name(model_name)
, model_setting(nullptr)
, texture_manager(nullptr) {
    auto ids = CubismFramework::GetIdManager();
    param_angel_x = ids->GetId(ParamAngleX);
    param_angel_y = ids->GetId(ParamAngleY);
    param_angel_z = ids->GetId(ParamAngleZ);
    param_body_angle_x = ids->GetId(ParamBodyAngleX);
    param_eye_ball_x = ids->GetId(ParamEyeBallX);
    param_eye_ball_y = ids->GetId(ParamEyeBallY);
}

Model::~Model() {
    release_model_setting();
    delete texture_manager;
}

godot::String Model::make_asset_path(const godot::String &filename) {
    return format("{0}/{1}", root_dir, filename);
}

void Model::load_asset(const godot::String &filename, const std::function<void(bytes, u32)> &callback) {
    u32 buffer_size = 0;
    bytes buffer = nullptr;

    if(filename.empty()) return;
    auto path = make_asset_path(filename);
    godot_print("try to load asset at {0}", path.alloc_c_string());
    buffer = load_file_as_bytes(make_asset_path(filename), &buffer_size);
    callback(buffer, buffer_size);
    release_bytes(buffer);
}

void Model::setup_model() {
    _updating = true;
    _initialized = false;

    load_asset(model_name + ".model3.json", [=](bytes buffer, u32 size) {
        model_setting = new Csm::CubismModelSettingJson(buffer, size);
    });
    load_asset(model_setting->GetModelFileName(), [=](bytes buffer, u32 size) {
        LoadModel(buffer, size);
    });

    for(i32 i = 0; i < model_setting->GetExpressionCount(); ++i)
        load_asset(model_setting->GetExpressionFileName(i), [=](bytes buffer, u32 size) {
            auto name = model_setting->GetExpressionName(i);
            auto motion = LoadExpression(buffer, size, name);
            if(expressions[name]) {
                ACubismMotion::Delete(expressions[name]);
                expressions[name] = nullptr;
            }
            expressions[name] = motion;
        });
    load_asset(model_setting->GetPoseFileName(), [=](bytes buffer, u32 size) {
        LoadPose(buffer, size);
    });
    load_asset(model_setting->GetPhysicsFileName(), [=](bytes buffer, u32 size) {
        LoadPhysics(buffer, size);
    });
    load_asset(model_setting->GetUserDataFile(), [=](bytes buffer, u32 size) {
        LoadUserData(buffer, size);
    });
    csmMap<csmString, f32> layout;
    model_setting->GetLayoutMap(layout);
    _modelMatrix->SetupFromLayout(layout);

    _model->SaveParameters();

    godot_print("there have {0} groups.", model_setting->GetMotionGroupCount());
    for(i32 i = 0; i < model_setting->GetMotionGroupCount(); ++i)
        preload_motion_group(model_setting->GetMotionGroupName(i));
    _motionManager->StopAllMotions();

    setup_textures();

    _updating = false;
    _initialized = true;
}

void Model::preload_motion_group(const char* group) {
    const i32 count = model_setting->GetMotionCount(group);
    godot_print("group {0} has {1} items.", group, count);
    for(i32 i = 0; i < count; ++i) {
        auto name = format("{0}_{1}", group, i);
        auto path = make_asset_path(model_setting->GetMotionFileName(group, i));
        godot_print("load motion: {0} => [{1}]", path, name);

        bytes buffer;
        u32 size;
        buffer = load_file_as_bytes(path, &size);
        auto tmp_motion = dynamic_cast<CubismMotion*>(LoadMotion(buffer, size, name.alloc_c_string()));
        f32 fade_time = model_setting->GetMotionFadeInTimeValue(group, i);
        godot_print("fade_in_time = {0}", fade_time);
        if(fade_time >= .0f)
            tmp_motion->SetFadeInTime(fade_time);

        fade_time = model_setting->GetMotionFadeOutTimeValue(group, i);
        godot_print("fade_out_time = {0}", fade_time);
        if(fade_time >= .0f)
            tmp_motion->SetFadeOutTime(fade_time);

        if(motions[name])
            ACubismMotion::Delete(motions[name]);
        motions[name] = tmp_motion;

        release_bytes(buffer);
    }
    godot_print("group {0} init finished.", group);
}

void Model::release_model_setting() {
    for(auto iter = motions.Begin(); iter != motions.End(); ++iter)
        Csm::ACubismMotion::Delete(iter->Second);
    motions.Clear();
    for(auto iter = expressions.Begin(); iter != expressions.End(); ++iter)
        Csm::ACubismMotion::Delete(iter->Second);
    expressions.Clear();
    delete model_setting;
}

void* Model::start_motion(const char* group, i32 no, i32 priority,
                          ACubismMotion::FinishedMotionCallback callback) {
    if(!model_setting->GetMotionCount(group))
        return Csm::InvalidMotionQueueEntryHandleValue;

    if(priority == Define::PriorityForce)
        _motionManager->SetReservePriority(priority);

    else if(!_motionManager->ReserveMotion(priority)) {
        godot_error("cannot start motion {0}_{1}", group, no);
        return InvalidMotionQueueEntryHandleValue;
    }

    const godot::String filename = model_setting->GetMotionFileName(group, no);

    auto name = format("{0}_{1}", group, no);
    auto motion = dynamic_cast<CubismMotion*>(motions[name]);
    bool auto_delete = false;

    if(! motion) {
        auto path = make_asset_path(filename);
        bytes buffer;
        u32 size;
        buffer = load_file_as_bytes(path, &size);
        motion = dynamic_cast<CubismMotion*>(LoadMotion(buffer, size, nullptr, callback));
        release_bytes(buffer);

        f32 fade_time = model_setting->GetMotionFadeInTimeValue(group, no);
        if(fade_time >= .0f)
            motion->SetFadeInTime(fade_time);
        fade_time = model_setting->GetMotionFadeOutTimeValue(group, no);
        if(fade_time >= .0f)
            motion->SetFadeOutTime(fade_time);
        auto_delete = true;
    }
    else
        motion->SetFinishedMotionHandler(callback);

    godot_print("start motion: {0}", name.alloc_c_string());
    return _motionManager->StartMotionPriority(motion, auto_delete, priority);
}

void* Model::start_random_motion(const char *group, i32 priority, Csm::ACubismMotion::FinishedMotionCallback callback) {
    auto count = model_setting->GetMotionCount(group);
    if(!count)
        return Csm::InvalidMotionQueueEntryHandleValue;
    static std::default_random_engine engine(time(nullptr));
    std::uniform_int_distribution<i32> distribution;
    return start_motion(group, distribution(engine) % count, priority, callback);
}

void Model::model_param_update(f32 delta) {
    increment_accu_time_sec += delta;
    bool updated = false;

    _model->LoadParameters();
    if(_motionManager->IsFinished())
        // 没有运动的时候播放idle运动
        start_motion(Define::MotionGroupIdle, 0, Define::PriorityIdle);
    else
        // 或者更新运动
        updated = _motionManager->UpdateMotion(_model, delta);

    _model->SaveParameters();

    // 没有更新运动的时候眨眼
    if(!updated && _eyeBlink)
        _eyeBlink->UpdateParameters(_model, delta);

    if(_expressionManager)
        _expressionManager->UpdateMotion(_model, delta);

    //TODO: 响应拖动
    // 之后再说
    // 参照官方示例CubismUserModelExtend.cpp#L369-L378，
    // CubismUserModelExtend::ModelParamUpdate()

    if(_breath)
        _breath->UpdateParameters(_model, delta);
    if(_physics)
        _physics->Evaluate(_model, delta);
    if(_pose)
        _pose->UpdateParameters(_model, delta);

    _model->Update();
}

void Model::setup_textures() {
    if(texture_manager) {
        godot_error("texture manager was inited.");
        return;
    }
    auto count = model_setting->GetTextureCount();
    godot_print("there have {0} textures.", count);
    texture_manager = new TextureManager(root_dir, count);
    for(i32 i = 0; i < count; ++i) {
        auto filename = make_asset_path(model_setting->GetTextureFileName(i));
        godot_print("texture {0} path = `{1}`", i, filename);
        if(filename.empty()) continue;
        texture_manager->load_texture_from_file(filename, i);
    }
}

TextureManager *Model::textures() {
    return texture_manager;
}

void Model::process_drawables(const std::function<void(usize, Drawable&)>& handler) {
    auto model = GetModel();
    usize count = model->GetDrawableCount();
    auto render_order = model->GetDrawableRenderOrders();
    auto sorted = new i32[count];
    for(i32 i = 0; i < count; ++i) sorted[render_order[i]] = i;
    u8* is_mask_list = new u8[count]; // 0 = 未初始化，1 = 是mask，2 = 不是mask
    auto masks_count_list = model->GetDrawableMaskCounts();
    auto masks_lists = model->GetDrawableMasks();
    for(usize i = 0; i < count; ++i){
        if(is_mask_list[i] == 0) is_mask_list[i] = 2;
        for(usize j = 0; j < masks_count_list[i]; ++j)
            is_mask_list[masks_lists[i][j]] = 1;
    }

    for(usize i = 0; i < count; ++i) {
        auto order = render_order[i];
        // 官方示例说的是不显示就跳过
        if(!model->GetDrawableDynamicFlagIsVisible(order))
            continue;
        //TODO: 暂时先不考虑高精度遮罩
        // 不知道这个所谓的高精度遮罩的索引和drawable的索引是不是一样的
        // 不如说，他这高精度遮罩到底是啥
        auto vertex_count = static_cast<usize>(model->GetDrawableVertexCount(order));
        Drawable drawable{};
        drawable.index = static_cast<usize>(order);
        drawable.render_order = render_order[order];
        drawable.texture_index = model->GetDrawableTextureIndices(order);
        drawable.opacity = model->GetDrawableOpacity(order);
        drawable.is_culling = model->GetDrawableCulling(order) != 0;
        drawable.inverted_mask = model->GetDrawableInvertedMask(order);
        drawable.blend_mode = model->GetDrawableBlendMode(order);
        drawable.indices = model->GetDrawableVertexIndices(order);
        drawable.index_count = static_cast<usize>(model->GetDrawableVertexIndexCount(order));
        drawable.positions = model->GetDrawableVertexPositions(order);
        drawable.vertex_uvs = model->GetDrawableVertexUvs(order);
        drawable.vertex_count = vertex_count;
        drawable.is_mask = is_mask_list[order];
        handler(order, drawable);
    }
}

void Model::get_canvas_info(csmVector2 *size_in_pixels, csmVector2 *origin_in_pixels, f32 *pixel_per_unit) {
    auto model = GetModel()->GetModel();
    Live2D::Cubism::Core::csmReadCanvasInfo(model, size_in_pixels, origin_in_pixels, pixel_per_unit);
}

void Model::start_expression(const godot::String& id, i32 priority) {
    auto expr = expressions[id];
    if(expr != nullptr)
        _expressionManager->StartMotionPriority(expr, false, priority);
}

void Model::start_random_expression(i32 priority) {
    auto size = expressions.GetSize();
    if(!size) return;
    static std::default_random_engine engine(time(nullptr));
    std::uniform_int_distribution<i32> distribution;
    csmMap<godot::String, ACubismMotion*>::const_iterator iter;
    i32 i = 0, number = distribution(engine) % size;
    for(iter = expressions.Begin(); iter != expressions.End() && i < number; ++iter, ++i);
    start_expression(iter->First, priority);
}

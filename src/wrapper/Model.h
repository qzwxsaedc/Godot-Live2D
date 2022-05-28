//
// Created by qzwxsaedc on 2022/5/26.
//

#ifndef LIBGDLIVE2D_MODEL_H
#define LIBGDLIVE2D_MODEL_H
#include "utils.h"
#include "TextureManager.h"
#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>

#include <functional>
#include "Drawable.h"

using namespace Live2D::Cubism::Core;
using namespace Util;
namespace Live2DWrapper {
    class Model : public Csm::CubismUserModel {
    public:
        Model(const godot::String& root_dir, const godot::String& model_name);
        virtual ~Model();
        void setup_model();
        TextureManager* textures();
        void model_param_update(f32 delta);
        void process_drawables(const std::function<void(usize, Drawable&)>& handler);
        void get_canvas_info(csmVector2* size_in_pixels, csmVector2* origin_in_pixels, f32* pixel_per_unit);

        Csm::CubismMotionQueueEntryHandle start_motion(const char* group, i32 no, i32 priority, Csm::ACubismMotion::FinishedMotionCallback callback = nullptr);
        Csm::CubismMotionQueueEntryHandle start_random_motion(const char* group, i32 priority, Csm::ACubismMotion::FinishedMotionCallback callback = nullptr);
        void start_expression(const godot::String&  id, i32 priority);
        void start_random_expression(i32 priority);
    private:
        godot::String make_asset_path(const godot::String& filename);
        void load_asset(const godot::String& filename, const std::function<void(bytes, u32)>& callback);
        void setup_textures();
        void preload_motion_group(const char* group);

        void release_model_setting();

        godot::String root_dir;
        godot::String model_name;

        f32 increment_accu_time_sec;
        Csm::ICubismModelSetting* model_setting;
        Csm::csmVector<Csm::CubismIdHandle> eye_bink_ids;
        Csm::csmMap<godot::String, Csm::ACubismMotion*> motions;
        Csm::csmMap<godot::String, Csm::ACubismMotion*> expressions;

        TextureManager* texture_manager;

        const Csm::CubismId* param_angel_x;
        const Csm::CubismId* param_angel_y;
        const Csm::CubismId* param_angel_z;
        const Csm::CubismId* param_body_angle_x;
        const Csm::CubismId* param_eye_ball_x;
        const Csm::CubismId* param_eye_ball_y;
    };
}


#endif //LIBGDLIVE2D_MODEL_H

#ifndef GDLIVE2D_H
#define GDLIVE2D_H

#include <Godot.hpp>
#include <Node2D.hpp>
#include <ImageTexture.hpp>
#include <Shader.hpp>
#include <ShaderMaterial.hpp>
#include <MeshInstance2D.hpp>
#include "wrapper/Model.h"
#include "wrapper/Allocator.h"

using namespace Live2DWrapper;
namespace godot
{
    class GDLive2D : public Node2D
    {
        struct MeshNode {
            Ref<ShaderMaterial> material;
            MeshInstance2D* mesh;
        };

        GODOT_CLASS(GDLive2D, Node2D)
    private:
        String root_path = "res://Resource";
        String model_name = "Hiyori";
        Ref<Shader> normal_shader{};
        Ref<Shader> additive_shader{};
        Ref<Shader> multiplicative_shader{};
        Ref<Shader> mask_normal_shader{};
        Ref<Shader> mask_inverted_shader{};

        Model* model{};
        Csm::CubismFramework::Option option{};
        Allocator allocator;

        std::vector<MeshNode> nodes;
    public:
        static void _register_methods();

        GDLive2D();
        ~GDLive2D();

        void _init(); // our initializer called by Godot
        void _ready();
        void _process(f32 delta);

        void on_play_motion(const String& group, i32 number, i32 priority);
        void on_play_random_motion(const String& group, i32 priority);
        void on_play_expression(const String& id, i32 priority);
        void on_play_random_expression(i32 priority);

        Ref<Shader> get_shader(Rendering::CubismBlendMode mode, bool is_mask);
//        void _process(float delta);
    };

}

#endif
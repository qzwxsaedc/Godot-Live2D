#include "gdlive2d.h"
#include <ArrayMesh.hpp>
#include <ProjectSettings.hpp>
using namespace godot;
using namespace Live2DWrapper;
using namespace Util;

static void static_log(const char* message) {
    godot::Godot::print(message);
}

godot::Variant vector2_csm_to_gd(const Live2D::Cubism::Core::csmVector2* vector2) {
    return {godot::Vector2(vector2->X, vector2->Y)};
}

void update_mesh(MeshInstance2D* mesh, Drawable& drawable, const std::function<Variant(const csmVector2*)>& process = vector2_csm_to_gd) {
    ArrayMesh* array_mesh = nullptr;
    auto mesh_mesh = mesh->get_mesh();

    if(mesh_mesh.is_null())
        array_mesh = ArrayMesh::_new();
    else
        array_mesh = Mesh::cast_to<ArrayMesh>(mesh_mesh.ptr());
    Array array;
    if(array_mesh->get_surface_count() > 0)
        array = array_mesh->surface_get_arrays(0);
    else
        array.resize(Mesh::ARRAY_MAX);
    array_mesh->clear_surfaces();
    PoolVector2Array vertices;
    PoolVector2Array uvs;
    PoolIntArray indices;

    // vertices
    for(usize i = 0; i < drawable.vertex_count; ++i)
        vertices.append(process(&drawable.positions[i]));

    // uvs
    for(usize i = 0; i < drawable.vertex_count; ++i)
        uvs.append(process(&drawable.vertex_uvs[i]));

    // indices
    for(usize i = 0; i < drawable.index_count; ++i)
        indices.append(static_cast<i32>(drawable.indices[i]));

    array[Mesh::ARRAY_VERTEX] = vertices;
    array[Mesh::ARRAY_TEX_UV] = uvs;
    array[Mesh::ARRAY_INDEX] = indices;
    if(indices.size() <= 0) godot_error("indices.size() <= 0. otherwise, drawable.index_count = {0}", drawable.index_count);
    array_mesh->add_surface_from_arrays(
            Mesh::PRIMITIVE_TRIANGLES,
            array
    );

    mesh->set_mesh(array_mesh);
}

MeshInstance2D* create_mesh_from_drawable(Drawable& drawable, f32 ppu) {
    MeshInstance2D* mesh = MeshInstance2D::_new();
    update_mesh(mesh, drawable);
    mesh->set_scale(mesh->get_scale() * ppu);
    return mesh;
}

void GDLive2D::_register_methods()
{
    register_method("_init", &GDLive2D::_init);
    register_method("_ready", &GDLive2D::_ready);
    register_method("_process", &GDLive2D::_process);

    register_method("_on_play_expression", &GDLive2D::on_play_expression);
    register_method("_on_play_random_expression", &GDLive2D::on_play_random_expression);
    register_method("_on_play_motion", &GDLive2D::on_play_motion);
    register_method("_on_play_random_motion", &GDLive2D::on_play_random_motion);

    register_property<GDLive2D, String>("root_path", &GDLive2D::root_path, "res://Resource");
    register_property<GDLive2D, String>("model_name", &GDLive2D::model_name, "Hiyori");
    register_property<GDLive2D, Ref<Shader>>("normal_shader",           &GDLive2D::normal_shader,         Ref<Shader>(nullptr));
    register_property<GDLive2D, Ref<Shader>>("additive_shader",         &GDLive2D::additive_shader,       Ref<Shader>(nullptr));
    register_property<GDLive2D, Ref<Shader>>("multiplicative_shader",   &GDLive2D::multiplicative_shader, Ref<Shader>(nullptr));
    register_property<GDLive2D, Ref<Shader>>("mask_normal_shader",      &GDLive2D::mask_normal_shader,    Ref<Shader>(nullptr));
    register_property<GDLive2D, Ref<Shader>>("mask_inverted_shader",    &GDLive2D::mask_inverted_shader,  Ref<Shader>(nullptr));
}

GDLive2D::GDLive2D() = default;

GDLive2D::~GDLive2D()
{
    normal_shader->free();
    additive_shader->free();
    multiplicative_shader->free();
    mask_normal_shader->free();
    mask_inverted_shader->free();

    for(auto & node : nodes){
        if(!node.material.is_null()) node.material.unref();
        if(node.mesh) node.mesh->free();
    }

    delete model;
}

void GDLive2D::_init() {
    option.LogFunction = static_log;
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;

    Csm::CubismFramework::StartUp(&allocator, &option);
    Csm::CubismFramework::Initialize();
    if(!Csm::CubismFramework::IsInitialized())
        godot_error("Cubism init failed.");

    godot_print("GDLive2D inited.");
}

void GDLive2D::_ready() {
    model = new Model(godot::ProjectSettings::get_singleton()->globalize_path(root_path), model_name);
    model->setup_model();
    godot_print("ready.");

    auto count = model->GetModel()->GetDrawableCount();
    nodes.resize(count);

    f32 ppu;
    csmVector2 a, b;
    model->get_canvas_info(&a, &b, &ppu);
    auto textures = model->textures();
    model->process_drawables([=](usize index, Drawable& drawable) {
        // 虽然我不知道为什么会有drawable的这个参数是0，但是确实有
        // 这个值是0的时候会影响mesh的创建，所以直接跳过
        if(drawable.index_count  <= 0){
            godot_warning("drawable {0}: idx_count={1}, vertex_count={2}. please check the model. create is ignored.", drawable.index, drawable.index_count, drawable.vertex_count);
            return;
        }
        /**********************
           material setting
        **********************/
        auto material = Ref(ShaderMaterial::_new());
        auto shader = get_shader(drawable.blend_mode, false);
        if(drawable.inverted_mask)
            material->set_shader(mask_inverted_shader);
        else
            material->set_shader(get_shader(drawable.blend_mode, drawable.is_mask));

        /******************
           mesh setting
        ******************/
        auto mesh = create_mesh_from_drawable(drawable, ppu);
        mesh->set_texture(textures->get(drawable.texture_index));
        mesh->set_z_as_relative(false);
        mesh->set_z_index(drawable.render_order);
        // TODO: 非mask对象设置透明
        {
            auto modulate = mesh->get_modulate();
            modulate.a = 0;
            mesh->set_modulate(modulate);
        }
        auto name = format("mesh_{0}", index);
        mesh->set_name(name.alloc_c_string());


        /***************************
           insert into root node
        ***************************/
        add_child(mesh);

        /************
           record
        ************/
        nodes[index].material = material;
        nodes[index].mesh = mesh;
    });
}

void GDLive2D::_process(f32 delta) {
    if(!model) {
        godot_error("model is null.");
        return;
    }
    model->model_param_update(delta);
    model->process_drawables([=](usize index, Drawable& drawable) {
        auto mesh = nodes[drawable.index].mesh;
        auto material = nodes[drawable.index].material;
        if(!mesh) return;
        {
            auto modulate = mesh->get_modulate();
            modulate.a = drawable.opacity;
            mesh->set_modulate(modulate);
        }
        update_mesh(mesh, drawable, [=](const csmVector2* vector2) {
            return godot::Variant(godot::Vector2(vector2->X, -vector2->Y));
        });
    });
}

Ref<Shader> GDLive2D::get_shader(Rendering::CubismBlendMode mode, bool is_mask) {
    switch (mode) {
        case Rendering::CubismBlendMode::Normal:
            return is_mask ? mask_normal_shader : normal_shader;
        case Rendering::CubismBlendMode::Additive:
            return additive_shader;
        case Rendering::CubismBlendMode::Multiplicative:
            return multiplicative_shader;
        default:
            return normal_shader;
    }
}

void GDLive2D::on_play_motion(const String& group, i32 number, i32 priority) {
    if(model)
        model->start_motion(group.alloc_c_string(), number, priority);
    else
        godot_error("model is null");
}

void GDLive2D::on_play_random_motion(const String& group, i32 priority) {
    if(model)
        model->start_random_motion(group.alloc_c_string(), priority);
    else
        godot_error("model is null");
}

void GDLive2D::on_play_expression(const String &id, i32 priority) {
    if(model)
        model->start_expression(id, priority);
    else
        godot_error("model is null");
}

void GDLive2D::on_play_random_expression(i32 priority) {
    if(model)
        model->start_random_expression(priority);
    else
        godot_error("model is null");
}


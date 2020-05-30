
#include "default.hpp"

#include <random>

#ifdef SCENE_DEFAULT_3D_GRAPHICS
#include <iostream>
#include <time.h>
#include <random>
#include "scenes/3D_graphics/00_default/Character.hpp"

using namespace vcl;
using namespace std;


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene , gui_structure& )
{
    srand(time(NULL));
    scene.camera.translation = {- camera_offset, - field_length / 2,  - field_height / 2};

    scene.camera.orientation = mat3({
            {0, 1, 0},  // red is x ; blue is z; green is y
            {0, 0, 1},
            {1, 0, 0}, 
            });

    load_mesh_building();
    load_mesh_background();
    character.load();
    character.location = {field_length /2, field_height /2};

    // building_positions = { {- 20.f, 0.f}, {- 20.f, 2.f}, {- 20.f, 5.f}};
    // generate_wave();

    timer.scale = 0.5f;
    timer_generate_wave.scale = 0.5f;
    timer_generate_wave.periodic_event_time_step = wave_interval;

    
}




/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    set_gui();
    auto shader = gui_scene.wireframe ? shaders["wireframe"] : shaders["mesh"];

    auto draw_building = [&shader, &scene, this](vec2 position){
        // assert(0 <= position.y &&  position.y <= field_length - building_side);
        glBindTexture(GL_TEXTURE_2D, texture_building);
        // glBindTexture(GL_TEXTURE_2D, scene.texture_white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        vec3 position3 = {position.x, position.y, earth_curve(position.x) };
        mesh_building.uniform.transform.translation = position3;
        draw(mesh_building, scene.camera, shader);
        mesh_building.uniform.transform.translation = {0,0,0};

        glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    };

    auto draw_background = [&shader, &scene, this](){
        glBindTexture(GL_TEXTURE_2D, texture_background);
        draw(mesh_background, scene.camera, shader);
        glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    };

    // // Enable use of alpha component as color blending for transparent elements
    // //  new color = previous color + (1-alpha) current color
    //
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    // // Disable depth buffer writing
    // //  - Transparent elements cannot use depth buffer
    // //  - They are supposed to be display from furest to nearest elements
    // glDepthMask(false);


    float dt = timer.update();
    timer_generate_wave.update();

    update_positions(dt);
    if (timer_generate_wave.event) generate_wave();
    garbage_collect();

    draw_background();

    for (auto position : building_positions)
        draw_building(position);


    character.draw(scene, shader); // handles texturing, animation etc. for the character

    // glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    // glDepthMask(true);
}


void scene_model::load_mesh_building(){
    mesh bd = mesh_load_file_obj("resources/modeles/immeuble.obj");

    mesh_building = bd;

    mesh_building.uniform.shading.specular = 0.05f; 
    // mesh_building.unifortm.transform.rotation = rotation_from_axis_angle_mat3({1, 0, 0}, M_PI / 2); 
    texture_building = create_texture_gpu( image_load_png("resources/textures/immeuble-texture2.png") );

    hitbox_building = Hitbox({0, 0, -building_depth}, 
            {building_side, building_side, building_height + building_depth});

}

void scene_model::load_mesh_background(){
    mesh_background = mesh_primitive_quad(
            {horizon, -background_side, background_height},
            {horizon, background_side, background_height}, 
            {horizon, background_side, -background_height}, 
            {horizon, -background_side, -background_height}
            );

    mesh_background.uniform.shading.specular = 0; 
    mesh_background.uniform.shading.diffuse = 0; 
    mesh_background.uniform.shading.ambiant = 1; 
    
    texture_background = create_texture_gpu( image_load_png("resources/textures/landscape_city2.png") );

}


void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}

void scene_model::update_positions(float dt){
    for (auto& position : building_positions){
        position.x += speed * dt;
    }
}

void scene_model::garbage_collect(){
    // assume x is growing in the position array
    while(!building_positions.empty() && get_building_hitbox(0).out_of_x_bound(-camera_offset))
        building_positions.pop_front();
}


void scene_model::generate_wave(){
    cerr << "generate wave" << endl;
    const float appearance_probability = .75;
    static default_random_engine generator;
    static bernoulli_distribution distribution(appearance_probability);
    for(float pos_y = 0; pos_y <= field_length - building_side; pos_y += building_side){
        if(distribution(generator)){
            const vec2 position = {horizon, pos_y};
            building_positions.push_back(position);
        }
    }
    for (auto i : building_positions){
        cerr << i << " ";
    }
}

Hitbox scene_model::get_building_hitbox(int i){
    vec2 position = building_positions[i];
    Hitbox box = hitbox_building;
    box.pos.x = position.x;
    box.pos.y = position.y;
    return box;
}

float scene_model::earth_curve(float x){
    return - curve * (x*x) ;
}


#endif


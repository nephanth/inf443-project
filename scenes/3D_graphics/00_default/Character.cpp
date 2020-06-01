/*
 * =====================================================================================
 *
 *       Filename:  Character.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/05/2020 18:54:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <iostream>
#include "main/helper_scene/helper_scene.hpp"
#include "scenes/3D_graphics/00_default/Character.hpp"
#include "vcl/opengl/texture/texture_gpu/texture_gpu.hpp"
#include "vcl/shape/mesh/mesh_loader/obj/obj.hpp"
#include "vcl/shape/mesh/mesh_primitive/mesh_primitive.hpp"
#include "vcl/wrapper/lodepng/lodepng.hpp"

using namespace vcl;
using namespace std;


void Character::load() {

    // mesh body_ = mesh_load_file_obj("resources/modeles/pigeon/body.obj"); 
    cerr << "loaded character";

    wing_base_rotation = rotation_from_axis_angle_mat3({0, 1, 0}, - .89011791852f) ; // rotation = 51 degrees
    // mat3 wing_base_rotation = mat3::identity();
    
    body = mesh_load_file_obj("resources/modeles/pigeon/body.obj");
    texture_body =create_texture_gpu( image_load_png("resources/textures/body.png") );
    body.texture_id = texture_body;

    backwing = mesh_load_file_obj("resources/modeles/pigeon/backwing.obj");
    texture_backwing = create_texture_gpu( image_load_png("resources/textures/wing.png") );
    backwing.texture_id = texture_backwing;

    forewing = mesh_load_file_obj("resources/modeles/pigeon/forewing.obj");
    texture_forewing = create_texture_gpu( image_load_png("resources/textures/wing2.png") );
    forewing.texture_id = texture_forewing;

    tail = mesh_load_file_obj("resources/modeles/pigeon/tail.obj");
    texture_tail = create_texture_gpu( image_load_png("resources/textures/Tail.png") );
    tail.texture_id = texture_tail;

    position_backwing_r = affine_transform( {- 0.3f,  0.1f, 0}, wing_base_rotation);
    position_forewing_r = affine_transform( {0,  0.2f, 0.3f}, mat3::identity());
    position_backwing_l = affine_transform( {- 0.3f,  -0.1f, 0}, wing_base_rotation, 1, {1, -1, 1 });
    position_forewing_l = affine_transform( {0,  0.2f, 0.3f}, mat3::identity());
    position_tail = affine_transform({-0.05,-0.01,0}, rotation_from_axis_angle_mat3({0,1,0}, M_PI/6));


    this->add(body, "body");
    this->add(backwing, "backwing_r", "body", position_backwing_r);
    this->add(forewing, "forewing_r", "backwing_r", position_forewing_r);
    this->add(backwing, "backwing_l", "body", position_backwing_l);
    this->add(forewing, "forewing_l", "backwing_l", {0, .2f, .3f});
    this->add(tail, "tail", "body", position_tail);
    
    
    timer.start();
    time_begun_movement = -10;

}



void Character::animate(){
    const float t = timer.t - time_begun_movement;
    const float omega =  M_PI / jump_duration;
    const float theta  = t <= (2 * jump_duration) ? omega * t : 0; //animation lasts twice the effective jump time

    (*this)["body"].transform.scaling = global_scale;
    (*this)["body"].transform.rotation = rotation_from_axis_angle_mat3({1, 0, 0}, angle);

    (*this)["backwing_r"].transform.rotation =  
        rotation_from_axis_angle_mat3( {1, 0, 0}, backwing_movement_amplitude * cos(theta) ) 
        * wing_base_rotation ;
    (*this)["backwing_l"].transform.rotation =
        rotation_from_axis_angle_mat3( {1, 0, 0}, - backwing_movement_amplitude * cos(theta) ) 
        * wing_base_rotation;
    (*this)["forewing_r"].transform.rotation = 
        rotation_from_axis_angle_mat3( {0, 0, 1}, - forewing_movement_amplitude * cos(theta) );
    (*this)["forewing_l"].transform.rotation = 
        rotation_from_axis_angle_mat3( {0, 0, 1}, forewing_movement_amplitude * cos(theta) );
}

void Character::get_input(float dt, GLFWwindow* window){
    if (glfwGetKey(window,  key_left) == GLFW_PRESS){
        angle = min(angle_bound, angle + turning_speed * dt);
    }
    if (glfwGetKey(window,  key_right) == GLFW_PRESS){
        angle = max(- angle_bound, angle - turning_speed * dt);
    }
    if (glfwGetKey(window,  key_jump) == GLFW_PRESS && (timer.t >= time_begun_movement + jump_duration)){
        time_begun_movement = timer.t;
    }
}

void Character::move(float dt){
    vec2 acceleration = g;
    const vec2 ey = {1, 0};
    const vec2 ez = {0, 1};

    const float t_jump = timer.t - time_begun_movement;

    if (t_jump <= jump_duration){
        const vec2 jump_direction = cos(angle) * ez - sin(angle) * ey;
        acceleration += jump_acceleration * jump_direction;
    }

    acceleration -= friction_factor * speed;

    const vec2 dv = dt * acceleration;
    speed += dv;
    location += dt * speed;

    const float y = location.x;
    const float z = location.y;
    (*this)["body"].transform.translation = {0, y, z};
}

Hitbox Character::hitbox() const{
    // no exact formulae, just good approximations 
    const float t = timer.t - time_begun_movement;
    const float omega =  M_PI / jump_duration;
    const float theta  = t <= (2 * jump_duration) ? omega * t : 0; //animation lasts twice the effective jump time
    const float ly_anim = ly * ( 1 + abs(sin(theta))) / 2;
    const float z_anim = location.y - lz* sin( theta / 2); 
    const float y_offset = sin(angle) / 4 * ly;

    return Hitbox(vec3(0,location.x - ly_anim/2 - y_offset, z_anim), vec3(lx, ly_anim, lz));
}

void Character::draw(scene_structure& scene, GLuint shader, GLFWwindow* window) {
    const float dt = timer.update();
    get_input(dt, window);
    move(dt);
    animate();
    update_local_to_global_coordinates();
    vcl::draw(*this, scene.camera, shader);
}

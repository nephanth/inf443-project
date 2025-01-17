/*
 * =====================================================================================
 *
 *       Filename:  Character.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/05/2020 18:54:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#pragma once
#include "scenes/3D_graphics/00_default/Hitbox.hpp"
#include "main/helper_scene/helper_scene.hpp"
#include "vcl/shape/hierarchy_mesh/hierarchy_mesh_drawable/hierarchy_mesh_drawable.hpp"

using namespace vcl;

class Character : public hierarchy_mesh_drawable {
    public:
        void load();

        void animate();
        void move(float dt);
        void get_input(float dt, GLFWwindow* window);

        void draw(scene_structure& scene, GLuint shader, GLFWwindow* window);

        Hitbox hitbox() const;

        vec2 location;

        float angle = 0;

    protected:


        vcl::timer_basic timer;
        float time_begun_movement;
        vec2 speed;


        mesh_drawable body;
        mesh_drawable backwing;
        mesh_drawable forewing;
        mesh_drawable tail;

        GLuint texture_body;
        GLuint texture_backwing;
        GLuint texture_forewing;
        GLuint texture_tail;


        affine_transform position_backwing_r;
        affine_transform position_forewing_r;
        affine_transform position_backwing_l;
        affine_transform position_forewing_l;
        affine_transform position_tail;

        mat3 wing_base_rotation;

        const float global_scale = .1;

        const float lx = global_scale * .5;
        const float ly = global_scale * 2.;
        const float lz = global_scale * 1.;

        const vec2 g = {0, -1}; // downwards acceleration (so-called gravity)
        const float jump_acceleration = 4;
        const float friction_factor = .4;


        const float backwing_movement_amplitude = M_PI / 4;
        const float forewing_movement_amplitude = M_PI / 6;

        const float jump_duration = .3;

        const float angle_bound = M_PI / 4;
        const float turning_speed = M_PI / 2;

        const int key_right = GLFW_KEY_RIGHT;
        const int key_left = GLFW_KEY_LEFT;
        const int key_jump = GLFW_KEY_SPACE;

};

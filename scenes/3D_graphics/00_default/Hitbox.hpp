/*
 * =====================================================================================
 *
 *       Filename:  Hitbox.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  25/05/2020 12:11:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tristan Brugere
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once

#include "vcl/vcl.hpp"
#include "main/helper_scene/helper_scene.hpp"

using namespace vcl;


/**
 * @brief hit box for collisions , parallell to axes. defined by a point at pos, and a vector v = x, y, z  which is the length
 */
struct Hitbox{ // 

    Hitbox();

    Hitbox(vec3 pos_, vec3 v) ;
    Hitbox(vec3 v ); 

    // Hitbox(Hitbox& h ) : pos(h.pos), x(h.x), y(h.y), z(h.z) {}; // implicit

    vec3 pos;

    vec3 v;



    bool contains (vec3 a);
    bool collision(Hitbox h);

    bool out_of_x_bound(float x);

    void draw(camera_scene& camera, GLuint shader);

    private:
        mesh_drawable as_mesh;
        bool mesh_defined = false;
};


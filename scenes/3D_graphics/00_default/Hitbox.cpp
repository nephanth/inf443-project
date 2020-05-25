/*
 * =====================================================================================
 *
 *       Filename:  Hitbox.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  25/05/2020 12:28:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "scenes/3D_graphics/00_default/Hitbox.hpp"


static float operator<=(vec3 a, vec3 b){
    return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z);
}

Hitbox::Hitbox(): pos(), v() {};
Hitbox::Hitbox(vec3 pos_, vec3 v_) : pos(pos_),  v(v_) {};
Hitbox::Hitbox(vec3 v_) : pos({0,0,0}), v(v_) {};

bool Hitbox::contains (vec3 a){
    a -= pos;
    return (a <= v) && (vec3() <= a);
}



bool Hitbox::collision(Hitbox h){
    return this->contains(h.pos) || h.contains(pos);
}

bool Hitbox::out_of_x_bound(float x){
    return pos.x > x;
}


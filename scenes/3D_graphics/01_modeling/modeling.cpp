
#include "modeling.hpp"


#ifdef SCENE_3D_GRAPHICS

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <algorithm>


// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
using namespace std;

template<class T>
T sqr(T x) { return x * x; }

// template<size_t N>
// float euclidean_distance(vec<N> x, vec<N> y){
//     float d = 0.;
//     for(uint i=0; i<N; i++) d += sqr(x[i] - y[i]);
//     return d;
// }


float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();
mesh create_cylinder(float radius, float height);
mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset, float z0 = 0);
// mesh create_tree(float radius_trunk, float height_trunk, float radius_fol);
mesh create_tree(float radius_trunk, float height_trunk, float radius_fol, float height_fol, float offset_fol);
mesh create_billboard(float length=.6f, float height=0.6f, bool flat=false);

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    srand(time(NULL));
    // Create visual terrain surface
    terrain = create_terrain();
    // terrain.uniform.color = {0.6f,0.85f,0.5f};
    terrain.uniform.shading.specular = 0.0f; // non-specular terrain material

    terrain_texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/textures/grass.png") );
    plant_texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/02_texture/assets/billboard_grass.png"), GL_REPEAT, GL_REPEAT );
    flower_texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/textures/flower.png"), GL_REPEAT, GL_REPEAT );

    tree_number = 12;
    plant_number = 42;
    flower_number = 12;

    update_positions();

    float height_trunk = 1;
    trunk = create_cylinder(.2, height_trunk);
    trunk.uniform.color = { .5f, .25f, .1f };
    foliage = create_tree_foliage(.6,.7,.5, height_trunk );
    foliage.uniform.color = {0.6f,0.95f,0.5f};
    trunk.uniform.shading.specular = 0.05f; 
    foliage.uniform.shading.specular = 0.05f; 

    plant = create_billboard();
    flower = create_billboard(.6f, 1.2f, true);
    plant.uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
    flower.uniform.shading = {1,0,0}; 

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

}




/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();

    const mat3 Identity = mat3::identity();
    const mat3 R = rotation_from_axis_angle_mat3({0,0,1}, 3.14f/2.0f); // orthogonal rotation
    const mat3 R2 = rotation_from_axis_angle_mat3({1,0,0}, - 3.14f/2.0f); // orthogonal rotation

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    auto shader = gui_scene.wireframe ? shaders["wireframe"] : shaders["mesh"];

    // Display terrain
    glPolygonOffset( 1.0, 1.0 );

    // set texture to terrain’s texture
    glBindTexture(GL_TEXTURE_2D, terrain_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    draw(terrain, scene.camera, shader);

    //go back to default texture
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    for(auto position : tree_positions ){
        trunk.uniform.transform.translation = position;
        foliage.uniform.transform.translation = position;
        draw(trunk, scene.camera, shader);
        draw(foliage, scene.camera, shader);
    }

    // if( gui_scene.wireframe ){ // wireframe if asked from the GUI
    //     glPolygonOffset( 1.0, 1.0 );
    //     draw(terrain, scene.camera, shaders["wireframe"]);
    //     draw(trunk, scene.camera, shaders["wireframe"]);
    //     draw(foliage, scene.camera, shaders["wireframe"]);
    // }
    //

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);

    glBindTexture(GL_TEXTURE_2D, plant_texture_id);

    // draw(plant, scene.camera, shader);
    for(auto position : plant_positions){
        plant.uniform.transform.translation = position;
        plant.uniform.transform.rotation = Identity;
        draw(plant, scene.camera, shader);
        plant.uniform.transform.rotation = R;
        draw(plant, scene.camera, shader);
    }

    glBindTexture(GL_TEXTURE_2D, flower_texture_id);
    // flower.uniform.transform.rotation = scene.camera.orientation ;
    // draw(flower, scene.camera, shader);
    for(auto position : flower_positions){
        flower.uniform.transform.translation = position;
        flower.uniform.transform.rotation = scene.camera.orientation;
        draw(flower, scene.camera, shader);
    }

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    glDepthMask(true);

}

void scene_model::update_positions(){
    tree_positions.clear();
    while(tree_positions.size()<tree_number){
        const float u = rand_interval();
        const float v = rand_interval();
        const vec3 x = evaluate_terrain(u,v);

        auto too_close = [x](vec3 v){ 
            const vec2 a = {x[0], x[1]};
            const vec2 b = {v[0], v[1]};
            return norm(a-b) < 2;
        };
        
        if (any_of(tree_positions.begin(), tree_positions.end(), too_close)) continue;

        tree_positions.push_back(x);
    }

    while(plant_positions.size()<plant_number){
        const float u = rand_interval();
        const float v = rand_interval();
        const vec3 x = evaluate_terrain(u,v);

        plant_positions.push_back(x);
    }

    while(flower_positions.size()<flower_number){
        const float u = rand_interval();
        const float v = rand_interval();
        const vec3 x = evaluate_terrain(u,v);

        flower_positions.push_back(x);
    }

    
}



// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    // const vec2 u0 = {0.5f, 0.5f};
    // const float h0 = 2.0f;

    // const float d = norm(vec2(u,v)-u0)/sigma0;

    vec2 x = {u,v};

    // const float z = h0*std::exp(-d*d);
    // const float z = h0*std::exp(-d*d);
    const size_t n = 4;
    const array<float, n> h  = {3,-1.5,1,2};
    const array<vec2, n> p  = { vec2({0.,0.}),  vec2({.5, .5}), vec2({.2, .7}), vec2({.8, .7}) };
    const array<float, n> sigma  = {0.5,0.15,0.2,0.2};
    

    float z = 0;
    for (uint i=0;i<n; i++)
        z += h[i] * exp( - sqr(norm(x - p[i])/sigma[i] ));

    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v);

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;
    const float scale_texture = 10;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {

            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Evaluate Perlin noise
            const float height = 0.3f;
            const float scaling = 3.0f;
            const int octave = 3;
            const float persistency = 0.4f;
            const float noise = perlin(scaling*u, scaling*v, octave, persistency);

            // Compute coordinates
            terrain.position[kv+N*ku] = noise * height + evaluate_terrain(u,v);
            terrain.texture_uv[kv+N*ku] = {scale_texture* u, scale_texture*v};
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const uint3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}

mesh create_cylinder(float radius, float height, uint N){
    mesh cylinder ;
    cylinder.position.resize(2*N);
    cylinder.connectivity.resize(2*N);
    
    for (uint i = 0; i< N; i++){
        const float angle = i * 2 * M_PI / N;
        float x = radius * cos (angle);
        float y = radius * sin (angle);
        cylinder.position[2 * i] = {x,y,0 };
        cylinder.position[2 * i + 1] = {x,y,height };
        cylinder.connectivity[2 * i] = {2*i + 1, 2*i, 2 * ( (i + 1) % N ) };
        cylinder.connectivity[2 * i + 1] = {2 * i, 2*i + 1, ( (2*i - 1 + 2*N) % (2*N) ) };

    }

    return cylinder;
}

mesh create_cylinder(float radius, float height){
    return create_cylinder(radius, height, 16);
}

mesh create_cone(float radius, float height, float z_offset, uint N){
    mesh cone;
    cone.position.resize(N + 2);
    cone.connectivity.resize(2* N);

    cone.position[N ] = {0, 0,  z_offset };
    cone.position[N  + 1] = {0, 0, z_offset + height};
    
    for (uint i = 0; i< N; i++){
        const float angle = i * 2 * M_PI / N;
        float x = radius * cos (angle);
        float y = radius * sin (angle);
        cone.position[i] = {x,y,z_offset };
        cone.connectivity[2 * i] = {i, (i + 1) % N, N  };
        cone.connectivity[2 * i] = {i, (i + 1) % N, N +1 };
    }

    return cone;
}

mesh create_cone(float radius, float height, float z_offset){
    return create_cone(radius, height, z_offset, 16);
}



mesh create_tree_foliage(float radius, float height, float z_offset, float z0)
{
    mesh m = create_cone(radius, height, z0);
    m.push_back( create_cone(radius, height,z0 + z_offset) );
    m.push_back( create_cone(radius, height, z0 +  2*z_offset) );

    return m;
}

// mesh create_tree(float radius_trunk, float height_trunk, float radius_fol, float height_fol, float offset_fol)
// {
//     mesh m = create_cylinder(radius_trunk, height_trunk);
//     mesh foliage = create_tree_foliage(radius_fol, height_fol, offset_fol, height_trunk);
//     m.push_back(foliage);
//     return m;
// }

mesh create_billboard(float length, float height, bool flat){
    const float l = length / 2.f;

    mesh surface_cpu;

    if(flat){
        surface_cpu.position     = {{-l,0,0}, { l ,0,0}, {l, height,0}, {-l, height, 0}};
        surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
        surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

    }
    else {
        surface_cpu.position     = {{0,-l,0}, { 0 ,l,0}, {0, l,height}, {0, -l, height}};
        surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
        surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

    }

    // mesh surface_cpu;
    // surface_cpu.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0.4f,0}, {-0.2f, 0.4f,0}};
    // surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    // surface_cpu.connectivity = {{0,1,2}, {0,2,3}};
    return surface_cpu;
}


void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);

}




#endif



#include "default.hpp"

#include <random>

#ifdef SCENE_DEFAULT_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& , gui_structure& )
{
    // Create mesh structure (stored on CPU)
    // *************************************** //
    mesh quadrangle;
    // Fill position buffer to model a unit quadrangle
    quadrangle.position = {{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0}};
    // Set the connectivity (quadrangle made of two triangles)
    quadrangle.connectivity = {{0,1,2}, {0,2,3}};


    // Transfert mesh onto GPU (ready to be displayed)
    // *************************************** //

    // Convert a mesh to mesh_drawable structure
    //  = mesh_drawable stores VBO and VAO, and allows to set uniform parameters easily
    surface = mesh_drawable(quadrangle);

    // Can attach a default shader to the mesh_drawable element
    surface.shader = shaders["mesh"];

    // Example of uniform parameter setting: color of the shape (used in the shader)
    surface.uniform.color = {1.0f, 1.0f, 0.6f};

    //create a mesh sphere
    mesh sphere_cpu = mesh_primitive_sphere();
    sphere = mesh_drawable(sphere_cpu);
    sphere.uniform.color = {1, 0, 0};
    sphere.uniform.transform.translation = {-.1f,.5f,.25f};
    sphere.uniform.transform.scaling = .1f;
    sphere.shader = shaders["mesh"];

}




/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    // Drawing call: need to provide the camera information (use the default shader if it has been set previously)
    draw(surface, scene.camera);

    sphere.uniform.color = {1,1,0};
    sphere.uniform.transform.translation = {0,0,0.5};
    draw(sphere, scene.camera);

    sphere.uniform.color = {0,0,1};
    sphere.uniform.transform.translation = {1,0,0.5};
    draw(sphere, scene.camera);

}






#endif


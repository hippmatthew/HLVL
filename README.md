# High Level Vulkan Library

HLVL is a lightweight graphics library with the goal of providing a quick and easy high level interface to vulkan

#### Dependencies
- VulkanSDK 1.3.296.0 (macOS) / 1.4.304.1 (linux/windows)
- GLFW 3.4.0
- Catch2 3.7.1 (for building tests)

> MoltenVK has yet to be updated to work with vulkan 1.4, but as soon as it is HLVL macos support will be tested
> and updated

## Building HLVL

> It is recommended to build HLVL using CMake. One can use their own build system to accomplish this,
> however all steps below will assume the use of CMake since it is just plain easier

1. Create a build directory in the root of your project directory:
    `mkdir <build_directory>` (common practice is to call this directory `build`)
2. Navigate to the build directory: `cd <build_directory>`
3. Create a CMake build configuration (see list of options below): `cmake .. <build_options>`
4. Build the library with the generated build files using your respective build system
5. The library will be located in `<build_directory>/lib`
6. If headers were built, they will be located in `<build_directory>/include`

> As of right now there is no local install functionality, but there are plans to implement it in the future

## Using HLVL

> A master header file generator will be made in the future. For now, you must include each of the HLVL headers as needed

A general workflow for using this library goes as such:
1. Initialize the `hlvl::Context`
2. Add materials to the hlvl materials manager using the `hlvl_materials` macro
3. Add objects to be renderered to the hlvl objects manager using the `hlvl_objects` macro
4. Run the main loop with `hlvl::Context::run()`. Note that you can pass in a lambda function to this
to run your own code during the main loop

> There is no documentation yet besides this, so if you don't want to read through the source code
> it is best to have some sort of code intelligence active in your editor that way you can
> see a list of functions avaliable to you when working with the HLVL objects

## How HLVL Works

#### Settings

Accessed with the `hlvl_settings` macro, this object controls all of the variables that control hlvl internally.
This is where you can modify your application name and version, image format and color space, specify whether you
want hlvl to do double or triple buffering, etc.

It is best to change these settings before initializing the context as the context relies on them to set up the
window and vulkan objects.

> **settings list**
> window settings: `window_width`, `window_height`, `window_title`
> application settings: `application_name`, `application_version`
> renderer settings:
> - `buffer_mode`: Whether to single, double, or triple buffer
> - `format` and `color_space`: the image format and color space to prioritize. If unavailable, will choose the vulkan defaults
> - `present_mode`: How the renderer delivers the image to the screen. Will choose FIFO mode if chosen is unavailable
> - `extent`: The rendering area. Defaults to the size of the window but you can technially change this
> - `background_color`: an array of 4 floats designating the background color of the window

#### Context

This object controls the meat and potatoes of vulkan. There can only be a single instance of this as it sets up
everything once you call it. If you try to create more than one, it will throw an error!

#### Materials

Materials in HLVL are defined as a class that stores all of the information related to a set of shaders.
This would be a the pipelines, uniform buffers, texture samplers, push constants, etc. They are separated from
the renderable objects as to allow for multiple objects to use the same material.

Materials have a builder class to help with their creation. Use `Material::builder()` ot retrieve a material builder.

**`MaterialBuilder` methods**

`add_shader(vk::ShaderStageFlagBits stage, std::string path)`
: tells the builder that the spir-v file at `path` is a shader for the pipeline stage specified by `stage`.

`add_texture(std::string path)`
: tells the builder that there is a texture at `path`. Textures are all in descriptor set 0 and have binding
numbers in order of addition to the material. Textures are only available in the fragment shader.

`add_resource({ type = Storage/Uniform, vk::PipelineShaderStageFlags stages, Resource * resource })`
: add a storage or uniform buffer to the shader to be accesible by the stages specified in `stages`. All buffers
share the same descriptor set and have binding numbers in order of addition to the material. If there
are textures in the material, buffers will be on descriptor set 0. Otherwise, they will be on descriptor set 1.

`add_constants(unsigned int size, void * data)`
: add push constants to the material. Data should be a struct containing every constant you want in the material.
Push constants are available in every graphics pipeline stage.

To create the material, pass the material builder into the `hlvl_materials.create()` function.

<details>
  <summary>Click here for information on how to add these objects to your glsl shaders</summary>

  Vulkan expects to recieve data matching exactly to what shaders are asking for, so it is important that the textures,
  buffers, and push constants you add to a material reflect what is happening in the material's shaders

  **glsl textures**
  ```glsl
  layout(set = 0, binding = N) uniform sampler2D  variable_name;
  ```

  **glsl buffers**
  ```glsl
  layout(set = 0/1, binding = N) buffer/uniform Class_Name {
    // class data
  } variable_name_if_not_global;
  ```

  **glsl push constants**
  ```glsl
  layout(push_constant) uniform Class_Name {
    // class data
  } variable_name_if_not_global;
  ```
</details>

**`hlvl::Resource<T>` Info**

These are specialized objects that are solely to be used in material buffers. Every time you assign a new value to
a resource, the resource automatically updates its associated device buffer through a persistant memory mapping. Due
to this, resources should not exist without being assigned to a material. Should you try to assign a value to a resource
that has not been added to a material, a segmentation fault will occur since there is no mapped memory to copy to.

#### Objects

HLVL Objects are classes that have a vertex buffer, index buffer, and material. They are created in a smiliar way to
creating materials. First, create the builder with `Object::builder()` and then add your vertex buffer, index buffer,
and material with `.add_vertices()`, `.add_indices()`, and `.add_material()`. Then, pass your object builder into
`hlvl_objects.add()`

> HLVL specifies a vertex as a `vec<3>` position and a `vec<2>` uv coordinate

> There are plans to have importable object files in the future. For now, specifiying the vertices is all you can do

#### Main Loop

HLVL's main loop is structured as such:
1. poll window events
2. run user defined code
3. render objects

The main loop is invoked by using `hlvl::Context()::run()`. If you want to add your code to the main loop, this function takes
in a lambda function.

The default close condition is when the window closes, but you can also break early by invoking `hlvl::Context::close()`
Just make sure that the lambda function has access to your context.
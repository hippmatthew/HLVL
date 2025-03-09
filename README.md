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
> - `buffer_mode`: Whether to double or triple buffer,
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

Materials have a builder class to help with their creation. Use `Material::builder()` to get a new builder object
and then the `add_shader()` function to add shaders to the builder.

> This will be expanded upon in the near future to allow for the addition of uniform buffers and push constants
> and what not. If a custom shader language is created, materials might even be automatically produced while parsing
> the shader file

To create the material, pass the material builder into the `hlvl_materials.create()` function.

#### Objects

HLVL Objects are classes that have a vertex buffer, index buffer, and material. They are created in a smiliar way to
creating materials. First, create the builder with `Object::builder()` and then add your vertex buffer, index buffer,
and material with `.add_vertices()`, `.add_indices()`, and `.add_material()`. Then, pass your object builder into
`hlvl_objects.add()`

#### Main Loop

HLVL's main loop is structured as such:
1. poll window events
2. run user defined code
3. render objects

The main loop is invoked by using `hlvl::Context()::run()`. If you want to add your code to the main loop, this function takes
in a lambda function.

The default close condition is when the window closes, but you can also break early by invoking `hlvl::Context::close()`
Just make sure that the lambda function has access to your context.
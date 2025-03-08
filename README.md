# High Level Vulkan Library

HLVL is currently being refactored. The original project was too out of scope for what HLVL wants to achieve.
So, HLVL is being downscaled and crafted into something new and perfected!

## Idea Behind Downscaling

THe current HLVL project is trying to take into account every possible usecase -- from custom window
implementations to custum renderers, etc. etc. This is way beyond what HLVL should be doing. It is supposed to
be a lightweight engine with the goal of providing a quick and easy high level interface to vulkan in order to
create physics simulations.

Now, instead of ECS, you have 2 global singletons that control materials and objects.
Instead of having the option to define your own rendering system or use HLVL's defaut renderer, systems no longer
exist and HLVL will have a built in renderer that is used for everything.

In general, the refactored HLVL is cleaner, smaller, and more efficient.

## Goals

[x] create the hlvl context with all the vulkan stuffs
[x] create a basic material system
[x] create a basic object system
[x] create a renderer
[] flesh out the material system
  [] generalized descriptor sets
  [] generalized push constants
  [] generalized texture system
[] add depth resources to renderer
[] flesh out object creation
  [] import models from a standard model format (proabably fbx)
[] Create a header generator (for ease of use of library)
[] Create a shader compiler with a custom shader language that processes shaders into glsl and then into spir-v (for ease of use of library)

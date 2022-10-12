# Arche-cpp - WebGPU Graphics Engine

This repository is about the implementation of Native WebGPU ([Dawn](https://dawn.googlesource.com/dawn)) Engine.
The goal of this project is not a general-purpose game engine, but to demonstrate engine technologies and tools that
provide cross-platform based on WebGPU. Therefore, the development of the project is aimed at flattening and avoiding
excessive encapsulation. You can refer to the internal technical implementation and apply it to your own project. For
more thoughts on the project, visit [the project blog](https://arche.graphics/blog/).

## Motivation

This project is very similar to Unity's component entity pattern (not ECS). Based on entity and component, it is easy to
combine other open-source ability:

1. [ImGui](https://github.com/ocornut/imgui): GUI system
2. [OZZ-Animation](https://github.com/guillaumeblanc/ozz-animation): Animation system
3. [JoltPhysics](https://github.com/jrouwe/JoltPhysics): Physics system
4. [recastnavigation](https://github.com/recastnavigation/recastnavigation): NavMesh and path-finding system

The asset pipeline is based on ozz-animation and assimp:

1. All skeletal animation should use asset pipeline to generate OZZ files, which will compress raw-animation data and
   make runtime more efficiently. Now the skin only support FBX format, we will support GLTF convert in the future.
2. All static model is loaded by using [assimp](https://github.com/assimp/assimp) which support a lot of data format and
   include geometry preprocessor.

## Cloning && Install

This repository contains submodules for external dependencies, so when doing a fresh clone you need to clone
recursively:

```
git clone --recursive https://github.com/ArcheGraphics/Arche-cpp.git
```

Existing repositories can be updated manually:

```
git submodule init
git submodule update
```

Because this project uses the SPIRV toolchain to compile shaders and implement shader reflections, and the KTX texture
format requires Vulkan support, be sure to install the [Vulkan toolchain](https://vulkan.lunarg.com/sdk/home) (even on a
Mac). At the same time, since the
interface for reading SPIRV by Tint requires Vulkan support, you need to open the switch on `CMakeLists.txt` of dawn
before compiling Tint, and install [depot_tools](https://dawn.googlesource.com/dawn/+/HEAD/docs/building.md) at the same
time.

```
cd third_party/dawn && vim CMakeLists.txt
set(ENABLE_VULKAN ON)
```

After all those work, use the script in third_party folder to build them all:

```
./build.sh
```

I recommend use Clion as main IDE(which can load cmake directly), but I also add xcode project which can capture metal
frame when debug graphics feature.

## Feature

You can read code in [apps](https://github.com/ArcheGraphics/Arche-cpp/tree/main/apps) to find the feature in this
code, like physics and shadow(WIP):

![PhysX](https://github.com/yangfengzzz/DigitalVoxEffect/raw/main/doc/img/physx.gif "PhysX")

You can also redirect to [repos](https://github.com/yangfengzzz/DigitalVoxEffect) to see more effects.

# Arche-Cpp - WebGPU Graphics Engine

This project is a part of its WebGPU development, if you want to know about the implementation of web
WebGPU ([WebGPU](https://github.com/gpuweb/types))
Engine, please refer to [Arche.js](https://github.com/ArcheGraphics/Arche.js) and is currently mainly adapted to
the [Canary version of Chrome](https://www.google.com/intl/zh-CN/chrome/canary/). This repository is about the
implementation of Native WebGPU ([Dawn](https://dawn.googlesource.com/dawn)) Engine.

Since the WGSL shading language cannot meet the needs of advanced features, a cross-platform engine [DigitalVox5](https://github.com/yangfengzzz/DigitalVox5) is re-implemented based on Vulkan. The engine is based on the capabilities provided by SPIRV as a shader for cross-platform compilation and reflection, and is built with Cmake, which truly enables cross-platform compilation and execution.

**In the future, Vulkan will be used as the core bottom layer of the Native engine, and WebGPU will continue to be developed as the core bottom layer of the Web engine.**

## Motivation

This project is inspired by [Oasis](https://github.com/oasis-engine) which is an ECS-liked based engine(not very strict)
. Based on entity and component, it is easy to combine other open-source ability:

1. [ImGui](https://github.com/ocornut/imgui): GUI system
2. [OZZ-Animation](https://github.com/guillaumeblanc/ozz-animation): CPU Animation System
3. [PhysX](https://github.com/NVIDIAGameWorks/PhysX): Physical System

Which can load a lot of model format including:

1. [FBX](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2016-1-2): FBX loader with Ozz
   animation
2. [GLTF](https://github.com/syoyo/tinygltf): GLTF Loader with GPU-based Skinning Animation
3. [Other](https://developer.apple.com/documentation/modelio/mdlasset/1391813-canimportfileextension): OBJ and other
   format loaded by ModelIO Framework

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

After clone all third-party dependencies, use the script in third_party folder to build them all:

```
./build.sh
```

## Feature

You can read code in [apps](https://github.com/ArcheGraphics/Arche-cpp/tree/main/apps) to find the feature in this
code, like physx and shadow(WIP):
![PhysX](https://github.com/yangfengzzz/DigitalVoxEffect/raw/main/doc/img/physx.gif "PhysX")

You can also redirect to [repos](https://github.com/yangfengzzz/DigitalVoxEffect) to see more effects.

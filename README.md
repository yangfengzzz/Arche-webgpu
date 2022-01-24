# DigitalArche - WebGPU Graphics Engine

## Cloning && Install

This repository contains submodules for external dependencies, so when doing a fresh clone you need to clone
recursively:

```
git clone --recursive https://github.com/yangfengzzz/DigitalVox4.git
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

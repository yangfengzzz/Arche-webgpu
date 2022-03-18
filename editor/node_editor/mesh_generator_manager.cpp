//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_generator_manager.h"
#include "gui_entry.h"
#include "ui_font_manager.h"
#include <thread>

namespace vox {
editor::MeshGeneratorManager *editor::MeshGeneratorManager::getSingletonPtr(void) {
    return msSingleton;
}

editor::MeshGeneratorManager &editor::MeshGeneratorManager::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}
namespace editor {
MeshGeneratorManager::MeshGeneratorManager() {
    isRemeshing = &GUIEntry::getSingleton().states.remeshing;
    clearMeshGen = std::make_unique<ClearMeshGenerator>();
    loadKernels();
}

void MeshGeneratorManager::generate() {
    if (*isRemeshing) {
        return;
    }

    auto appState = GUIEntry::getSingletonPtr();
    if (appState->mode == ApplicationMode::TERRAIN) {
        // appState->models.coreTerrain->UploadToGPU();
    } else if (appState->mode == ApplicationMode::CUSTOM_BASE) {
        // appState->models.customBase->UploadToGPU();
    }

    *isRemeshing = true;
    std::thread worker([this, appState] {
        if (!clearMeshGen->useGPU) {
            clearMeshGen->generate();
            executeCPUGenerators();
            executeKernels();
        } else {
            executeKernels();
            executeCPUGenerators();
        }

        if (!appState->states.useGPUForNormals) {
            if (appState->mode == ApplicationMode::TERRAIN) {
                // appState->models.coreTerrain->mesh->RecalculateNormals();
            } else if (appState->mode == ApplicationMode::CUSTOM_BASE) {
                // appState->models.customBase->mesh->RecalculateNormals();
            }
        }
        *isRemeshing = false;
    });
    worker.detach();
}

void MeshGeneratorManager::generateSync() {
    auto appState = GUIEntry::getSingletonPtr();
    while (appState->states.remeshing);

    *isRemeshing = true;
    executeKernels();
    executeCPUGenerators();

    if (appState->mode == ApplicationMode::TERRAIN) {
        // appState->models.coreTerrain->mesh->RecalculateNormals();
        // appState->models.coreTerrain->UploadToGPU();
    } else if (appState->mode == ApplicationMode::CUSTOM_BASE) {
        // appState->models.customBase->mesh->RecalculateNormals();
        // appState->models.customBase->UploadToGPU();
    }

    *isRemeshing = false;
}

void MeshGeneratorManager::showSettings() {
    if (windowStat) {
        ImGui::Begin("Mesh Generators", &windowStat);
        ImGui::PushFont(getUIFont("OpenSans-Semi-Bold"));
        ImGui::Text("Base Mesh Generators");
        ImGui::PopFont();

        if (ImGui::CollapsingHeader("Auto Base Mesh Gen (GPU)", clearMeshGen->uiActive)) {
            clearMeshGen->showSettings();
        }

        ImGui::Separator();
        ImGui::PushFont(getUIFont("OpenSans-Semi-Bold"));
        ImGui::Text("CPU Noise Layer Generators");
        ImGui::PopFont();

        for (int i = 0; i < cpuNoiseLayers.size(); i++) {
            if (ImGui::CollapsingHeader((cpuNoiseLayers[i]->name + "##CPUNL" + std::to_string(i)).c_str())) {
                cpuNoiseLayers[i]->showSetting(i);

                if (ImGui::Button(("Delete##CPUNOUSELAYER" + std::to_string(i)).c_str())) {
                    while (*isRemeshing);

                    cpuNoiseLayers.erase(cpuNoiseLayers.begin() + i);
                    break;
                }
            }

            ImGui::Separator();
        }

        if (ImGui::Button("Add##CPULAYER")) {
            while (*isRemeshing);

            cpuNoiseLayers.emplace_back(std::make_unique<CPUNoiseLayersGenerator>());
        }

        ImGui::Separator();
        ImGui::PushFont(getUIFont("OpenSans-Semi-Bold"));
        ImGui::Text("GPU Noise Layer Generators");
        ImGui::PopFont();

        for (int i = 0; i < gpuNoiseLayers.size(); i++) {
            if (ImGui::CollapsingHeader((gpuNoiseLayers[i]->name + "##GPUNL" + std::to_string(i)).c_str())) {
                gpuNoiseLayers[i]->showSetting(i);

                if (ImGui::Button(("Delete##GPUNOUSELAYER" + std::to_string(i)).c_str())) {
                    while (*isRemeshing);

                    gpuNoiseLayers.erase(gpuNoiseLayers.begin() + i);
                    break;
                }
            }

            ImGui::Separator();
        }

        if (ImGui::Button("Add##GPUNL")) {
            while (*isRemeshing);

            gpuNoiseLayers.emplace_back(std::make_unique<GPUNoiseLayerGenerator>());
        }

        ImGui::Separator();
        ImGui::PushFont(getUIFont("OpenSans-Semi-Bold"));
        ImGui::Text("CPU Node Editor Generators");
        ImGui::PopFont();

        for (int i = 0; i < cpuNodeEditors.size(); i++) {
            if (ImGui::CollapsingHeader((cpuNodeEditors[i]->name + "##CPUNE" + std::to_string(i)).c_str())) {
                cpuNodeEditors[i]->showSetting(i);

                if (ImGui::Button(("Delete##CPNE" + std::to_string(i)).c_str())) {
                    while (*isRemeshing);

                    cpuNodeEditors.erase(cpuNodeEditors.begin() + i);
                    break;
                }
            }

            ImGui::Separator();
        }

        if (ImGui::Button("Add##CPUNE")) {
            while (*isRemeshing);

            cpuNodeEditors.emplace_back(std::make_unique<CPUNodeEditor>());
        }

        ImGui::Separator();
        ImGui::Text("Time : %lf ms", time);
        ImGui::End();
    }

    for (int i = 0; i < cpuNoiseLayers.size(); i++) {
        cpuNoiseLayers[i]->update();
    }

    for (int i = 0; i < gpuNoiseLayers.size(); i++) {
        gpuNoiseLayers[i]->update();
    }

    for (int i = 0; i < cpuNodeEditors.size(); i++) {
        cpuNodeEditors[i]->update();
    }
}

void MeshGeneratorManager::generateForTerrain() {
}

void MeshGeneratorManager::generateForCustomBase() {
}

void MeshGeneratorManager::executeKernels() {}

void MeshGeneratorManager::executeCPUGenerators() {
    auto appState = GUIEntry::getSingletonPtr();

    if (appState->mode == ApplicationMode::TERRAIN) {
        generateForTerrain();
    } else if (appState->mode == ApplicationMode::CUSTOM_BASE) {
        generateForCustomBase();
    }
}

void MeshGeneratorManager::loadKernels() {}


}
}

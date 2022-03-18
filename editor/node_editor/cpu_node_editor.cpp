//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpu_node_editor.h"
#include "editor_utils.h"
#include "nodes/abs_node.h"
#include "nodes/output_node.h"

namespace vox {
namespace editor {
static char *stristr4(const char *str, const char *pattern) {
    size_t i;

    if (!*pattern) {
        return (char *) str;
    }

    for (; *str; str++) {
        if (toupper((unsigned char) *str) == toupper((unsigned char) *pattern)) {
            for (i = 1;; i++) {
                if (!pattern[i]) {
                    return (char *) str;
                }

                if (toupper((unsigned char) str[i]) != toupper((unsigned char) pattern[i])) {
                    break;
                }
            }
        }
    }

    return NULL;
}

#define NODE_MAKER_COND(x) length == 0 || stristr4(x, data)
#define NODE_MAKER_SHOW(x, y) if (NODE_MAKER_COND(y)) {if (ImGui::Button((y + std::string("##CPUNE") + uid).c_str())) { editor->addNode(new x()); ImGui::CloseCurrentPopup(); } }

static int count = 1;

static void showNodeMaker(std::string &uid, NodeEditor *editor) {
    static char data[1000];
    ImGui::InputTextWithHint("##SearchMeshNodes", "Search ...", data, sizeof(data));
    size_t length = strlen(data);
    ImGui::BeginChild(("##CPUNE" + uid).c_str(), ImVec2(200, 250));
//    NODE_MAKER_SHOW(DummyNode, "Dummy");
//    NODE_MAKER_SHOW(MeshCoordinatesNode, "Mesh Coordinates");
//    NODE_MAKER_SHOW(MaxMeshCoordinatesNode, "Maximum Mesh Coordinates");
//    NODE_MAKER_SHOW(TextureCoordinatesNode, "Texture Coordinates");
//    NODE_MAKER_SHOW(MinMeshCoordinatesNode, "Minimum Mesh Coordinates");
//    NODE_MAKER_SHOW(TimeBasedSeedNode, "Time Based Seed");
//    NODE_MAKER_SHOW(PixelateNode, "Pixelate");
//    NODE_MAKER_SHOW(TextureNode, "Texture");
//    NODE_MAKER_SHOW(HillNode, "Hill");
//    NODE_MAKER_SHOW(AddNode, "Add");
//    NODE_MAKER_SHOW(SubNode, "Subtract");
//    NODE_MAKER_SHOW(MulNode, "Multiply");
//    NODE_MAKER_SHOW(DivNode, "Divide");
    NODE_MAKER_SHOW(AbsNode, "Absolute Value");
//    NODE_MAKER_SHOW(SinNode, "Sin");
//    NODE_MAKER_SHOW(CosNode, "Cos");
//    NODE_MAKER_SHOW(TanNode, "Tan");
//    NODE_MAKER_SHOW(SquareNode, "Square");
//    NODE_MAKER_SHOW(BlendNode, "Blend");
//    NODE_MAKER_SHOW(ClampNode, "Clamp");
//    NODE_MAKER_SHOW(CurveNode, "Curve Editor");
//    NODE_MAKER_SHOW(RandomNumberNode, "Random Number");
//    NODE_MAKER_SHOW(NoisePerlinNode, "Perlin Noise");
//    NODE_MAKER_SHOW(NoiseCellularNode, "Cellular Noise");
//    NODE_MAKER_SHOW(NoiseValueNode, "Value Noise");
//    NODE_MAKER_SHOW(NoiseOpenSimplex2Node, "Open Simplex 2 Noise");
//    NODE_MAKER_SHOW(NoiseOpenSimplex2SNode, "Open Simplex 2S Noise");
//    NODE_MAKER_SHOW(NoiseValueCubicNode, "Value Cubic Noise");
//    NODE_MAKER_SHOW(DuplicateNode, "Duplicate");
//    NODE_MAKER_SHOW(MathFunctionNode, "Custom Math Funcion");
//    NODE_MAKER_SHOW(MinValNode, "MinVal");
//    NODE_MAKER_SHOW(VisualizerNode, "Visualizer");
    ImGui::EndChild();
}

CPUNodeEditor::CPUNodeEditor() {
    uid = generateId(32);
    name.reserve(1024);
    name = "CPU Node Editor " + std::to_string(count++);
    
    editor = std::make_unique<NodeEditor>();
    editor->name = name;
    editor->setOutputNode(new OutputNode());
}

void CPUNodeEditor::update() {
    if (windowStat) {
        ImGui::Begin((name + "##" + uid).c_str(), &windowStat);

        if (ImGui::Button(("Add Node##CPUNE" + uid).c_str())) {
            ImGui::OpenPopup("NodeMakerDropped");
        }

        ImGui::SameLine();

        if (ImGui::Button(("Reset##CPUNE" + uid).c_str())) {
            editor->reset();
        }

        ImGui::SameLine();

        if (ImGui::Button(("Export##CPUNE" + uid).c_str())) {
//            std::string file = showSaveFileDialog("*.terr3d");
//
//            if (file.size() > 3) {
//                if (file.find(".terr3d") == std::string::npos) {
//                    file += ".terr3d";
//                }
//
//                SaveToFile(file, editor->save().dump(4));
//            }
        }

        ImGui::SameLine();

        if (ImGui::Button(("Import##CPUNE" + uid).c_str())) {
//            std::string file = showOpenFileDialog("*.terr3d");
//
//            if (file.size() > 3) {
//                bool tmp = false;
//                editor->reset();
//                editor->Load(nlohmann::json::parse(ReadShaderSourceFile(file, &tmp)));
//            }
        }

        editor->render();

//        if (ImGui::IsWindowFocused() && (((IsKeyDown(TERR3D_KEY_RIGHT_SHIFT) || IsKeyDown(TERR3D_KEY_LEFT_SHIFT)) && IsKeyDown(TERR3D_KEY_A)) || IsMouseButtonDown(TERR3D_MOUSE_BUTTON_MIDDLE))) {
//            ImGui::OpenPopup("NodeMakerDropped");
//        }

        if (ImGui::BeginPopup("NodeMakerDropped")) {
            showNodeMaker(uid, editor.get());

            if (ImGui::Button(("Close##CPUNE" + uid).c_str())) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::End();
    }
}

float CPUNodeEditor::evaluateAt(NodeInputParam param) {
    return editor->outputNode->evaluate(param, nullptr).value;
}

void CPUNodeEditor::showSetting(int id) {
    ImGui::InputText(("Name##CPUNE" + uid).c_str(), name.data(), 1024);
    ImGui::Checkbox(("Enabled##CPUNE" + uid).c_str(), &enabled);

    if (ImGui::Button(("Edit##CPUNE" + uid).c_str())) {
        windowStat = true;
    }

    ImGui::Text("UID : %s", uid.data());
    ImGui::Text("Time : %lf ms", time);
}

}
}

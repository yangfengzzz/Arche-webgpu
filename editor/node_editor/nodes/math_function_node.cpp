//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "math_function_node.h"
#include "cpu_node_editor.h"
#include <locale>
#include <codecvt>
#include <string>

namespace vox {
namespace editor {
MathFunctionNode::MathFunctionNode() {
    headerColor = ImColor(VALUE_NODE_COLOR);
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
//    parser = new mu::Parser();
    memset(inputExpression, 0, sizeof(inputExpression));
    memset(varname, 0, sizeof(varname));
    x = y = z = 0;
    factor = 1;
    mathInputWidth = 150;
    compiled = false;
    setupParser(parser);
#ifdef TERR3D_WIN32
    parser->DefineVar(L"x", &x);
    parser->DefineVar(L"y", &y);
    parser->DefineVar(L"z", &z);
#else
    parser->DefineVar("x", &x);
    parser->DefineVar("y", &y);
    parser->DefineVar("z", &z);
#endif

}

NodeOutput MathFunctionNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    try {
        if (input.maxZ == 0) {
            input.maxZ = 1;
        }

        x = (input.x / input.maxX) * 2 - 1;
        y = (input.y / input.maxY) * 2 - 1;
        z = (input.z / input.maxZ) * 2 - 1;
        return NodeOutput({(float) parser->Eval() * factor});
    }

    catch (...) {
    }

    return NodeOutput({0.0f});
}

void MathFunctionNode::onRender() {
    drawHeader("Custom Math Function");
    ImGui::Dummy(ImVec2(mathInputWidth + 20, 10));
    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    ImGui::NewLine();
    ImGui::Text("Math Input Size : ");
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::DragInt(MAKE_IMGUI_ID(outputPins[0]->id), &mathInputWidth, 0.5f);
    ImGui::PopItemWidth();
    inputPins[0]->render();

    if (inputPins[0]->isLinked()) {
        ImGui::Text("Factor");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[0]->id), &factor, 0.01f);
        ImGui::PopItemWidth();
    }

    ImGui::NewLine();
    /*
    ImGui::Text("Varaibles : ");
    for (int i = 0;i<vars.size();i++)
    {
        ImGui::PushItemWidth(200);
        ImGui::InputDouble(MAKE_IMGUI_LABEL(i, vars[i].first.c_str()), &vars[i].second, 0.01f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            parser->RemoveVar(s2ws(vars[i].first));
            vars.erase(vars.begin() + i);
            break;
        }
    }

    ImGui::Text("Add Variable : ");
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::InputTextWithHint(MAKE_IMGUI_ID(tid), "Name", varname, sizeof(varname));
    ImGui::SameLine();
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Add") && strlen(varname) >= 1)
    {
        vars.push_back(std::make_pair<std::string, double>(varname, 0.0f));
        parser->DefineVar(s2ws(std::string(varname)), &vars.back().second);
        memset(varname, 0, sizeof(varname));
    }

    */
    ImGui::Text("Expression : ");
    ImGui::SameLine();
    ImGui::PushItemWidth(mathInputWidth);

    if (ImGui::InputText(MAKE_IMGUI_ID(id), inputExpression, 1024 * 4)) {
        compiled = false;
    }

    ImGui::PopItemWidth();

    if (!compiled) {
        if (ImGui::Button("Compile")) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wide = converter.from_bytes(inputExpression);

            try {
#ifdef TERR3D_WIN32
                parser->SetExpr(wide);
#else
                parser->SetExpr(std::string(inputExpression));
#endif
                compiled = true;
            }

            catch (...) {
            }
        }
    }
}

}
}

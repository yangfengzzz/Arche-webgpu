//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "serializer.h"

namespace vox {
// MARK: - SERIALIZATION_HELPERS
void Serializer::serializeBoolean(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name, bool p_value) {
    tinyxml2::XMLElement *element = p_doc.NewElement(p_name.c_str());
    element->SetText(p_value);
    p_node->InsertEndChild(element);
}

void Serializer::serializeString(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, const std::string &p_value) {
    tinyxml2::XMLElement *element = p_doc.NewElement(p_name.c_str());
    element->SetText(p_value.c_str());
    p_node->InsertEndChild(element);
}

void Serializer::serializeFloat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, float p_value) {
    tinyxml2::XMLElement *element = p_doc.NewElement(p_name.c_str());
    element->SetText(p_value);
    p_node->InsertEndChild(element);
}

void Serializer::serializeDouble(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, double p_value) {
    tinyxml2::XMLElement *element = p_doc.NewElement(p_name.c_str());
    element->SetText(p_value);
    p_node->InsertEndChild(element);
}

void Serializer::serializeInt(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                              const std::string &p_name, int p_value) {
    tinyxml2::XMLElement *element = p_doc.NewElement(p_name.c_str());
    element->SetText(p_value);
    p_node->InsertEndChild(element);
}

void Serializer::serializeUint(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, unsigned p_value) {
    tinyxml2::XMLElement *element = p_doc.NewElement(p_name.c_str());
    element->SetText(p_value);
    p_node->InsertEndChild(element);
}

void Serializer::serializeInt64(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, int64_t p_value) {
    tinyxml2::XMLElement *element = p_doc.NewElement(p_name.c_str());
    element->SetText(p_value);
    p_node->InsertEndChild(element);
}

void Serializer::serializeVec2(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, const Vector2F &p_value) {
    tinyxml2::XMLNode *element = p_doc.NewElement(p_name.c_str());
    p_node->InsertEndChild(element);
    
    tinyxml2::XMLElement *x = p_doc.NewElement("x");
    x->SetText(p_value.x);
    element->InsertEndChild(x);
    
    tinyxml2::XMLElement *y = p_doc.NewElement("y");
    y->SetText(p_value.y);
    element->InsertEndChild(y);
}

void Serializer::serializeVec3(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, const Vector3F &p_value) {
    tinyxml2::XMLNode *element = p_doc.NewElement(p_name.c_str());
    p_node->InsertEndChild(element);
    
    tinyxml2::XMLElement *x = p_doc.NewElement("x");
    x->SetText(p_value.x);
    element->InsertEndChild(x);
    
    tinyxml2::XMLElement *y = p_doc.NewElement("y");
    y->SetText(p_value.y);
    element->InsertEndChild(y);
    
    tinyxml2::XMLElement *z = p_doc.NewElement("z");
    z->SetText(p_value.z);
    element->InsertEndChild(z);
}

void Serializer::serializeVec4(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, const Vector4F &p_value) {
    tinyxml2::XMLNode *element = p_doc.NewElement(p_name.c_str());
    p_node->InsertEndChild(element);
    
    tinyxml2::XMLElement *x = p_doc.NewElement("x");
    x->SetText(p_value.x);
    element->InsertEndChild(x);
    
    tinyxml2::XMLElement *y = p_doc.NewElement("y");
    y->SetText(p_value.y);
    element->InsertEndChild(y);
    
    tinyxml2::XMLElement *z = p_doc.NewElement("z");
    z->SetText(p_value.z);
    element->InsertEndChild(z);
    
    tinyxml2::XMLElement *w = p_doc.NewElement("w");
    w->SetText(p_value.w);
    element->InsertEndChild(w);
}

void Serializer::serializeQuat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, const QuaternionF &p_value) {
    tinyxml2::XMLNode *element = p_doc.NewElement(p_name.c_str());
    p_node->InsertEndChild(element);
    
    tinyxml2::XMLElement *x = p_doc.NewElement("x");
    x->SetText(p_value.x);
    element->InsertEndChild(x);
    
    tinyxml2::XMLElement *y = p_doc.NewElement("y");
    y->SetText(p_value.y);
    element->InsertEndChild(y);
    
    tinyxml2::XMLElement *z = p_doc.NewElement("z");
    z->SetText(p_value.z);
    element->InsertEndChild(z);
    
    tinyxml2::XMLElement *w = p_doc.NewElement("w");
    w->SetText(p_value.w);
    element->InsertEndChild(w);
}

void Serializer::serializeColor(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, const Color &p_value) {
    tinyxml2::XMLNode *element = p_doc.NewElement(p_name.c_str());
    p_node->InsertEndChild(element);
    
    tinyxml2::XMLElement *r = p_doc.NewElement("r");
    r->SetText(p_value.r);
    element->InsertEndChild(r);
    
    tinyxml2::XMLElement *g = p_doc.NewElement("g");
    g->SetText(p_value.g);
    element->InsertEndChild(g);
    
    tinyxml2::XMLElement *b = p_doc.NewElement("b");
    b->SetText(p_value.b);
    element->InsertEndChild(b);
    
    tinyxml2::XMLElement *a = p_doc.NewElement("a");
    a->SetText(p_value.a);
    element->InsertEndChild(a);
}

//void Serializer::serializeMaterial(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                   const std::string &p_name, OvCore::Resources::Material *p_value) {
//    SerializeString(p_doc, p_node, p_name.c_str(), p_value ? p_value->path : "?");
//}
//
//void Serializer::serializeSound(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                const std::string &p_name, OvAudio::Resources::Sound *p_value) {
//    SerializeString(p_doc, p_node, p_name.c_str(), p_value ? p_value->path : "?");
//}
//
//void Serializer::serializeModel(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                const std::string &p_name, OvRendering::Resources::Model *p_value) {
//    SerializeString(p_doc, p_node, p_name.c_str(), p_value ? p_value->path : "?");
//}
//
//void Serializer::serializeTexture(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                  const std::string &p_name, OvRendering::Resources::Texture *p_value) {
//    SerializeString(p_doc, p_node, p_name.c_str(), p_value ? p_value->path : "?");
//}
//
//void Serializer::serializeShader(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                 const std::string &p_name, OvRendering::Resources::Shader *p_value) {
//    SerializeString(p_doc, p_node, p_name.c_str(), p_value ? p_value->path : "?");
//}

// MARK: -  DESERIALIZATION_HELPERS
void Serializer::deserializeBoolean(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name, bool &p_out) {
    if (auto element = p_node->FirstChildElement(p_name.c_str()); element)
        element->QueryBoolText(&p_out);
}

void Serializer::deserializeString(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                   const std::string &p_name, std::string &p_out) {
    if (auto element = p_node->FirstChildElement(p_name.c_str()); element) {
        const char *result = element->GetText();
        p_out = result ? result : "";
    }
}

void Serializer::deserializeFloat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name, float &p_out) {
    if (auto element = p_node->FirstChildElement(p_name.c_str()); element)
        element->QueryFloatText(&p_out);
}

void Serializer::deserializeDouble(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                   const std::string &p_name, double &p_out) {
    if (auto element = p_node->FirstChildElement(p_name.c_str()); element)
        element->QueryDoubleText(&p_out);
}

void Serializer::deserializeInt(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, int &p_out) {
    if (auto element = p_node->FirstChildElement(p_name.c_str()); element)
        element->QueryIntText(&p_out);
}

void Serializer::deserializeUint(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, unsigned &p_out) {
    if (auto element = p_node->FirstChildElement(p_name.c_str()); element)
        element->QueryUnsignedText(&p_out);
}

void Serializer::deserializeInt64(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name, int64_t &p_out) {
    if (auto element = p_node->FirstChildElement(p_name.c_str()); element)
        element->QueryInt64Text(&p_out);
}

void Serializer::deserializeVec2(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, Vector2F &p_out) {
    if (auto node = p_node->FirstChildElement(p_name.c_str()); node) {
        if (auto element = node->FirstChildElement("x"); element)
            element->QueryFloatText(&p_out.x);
        
        if (auto element = node->FirstChildElement("y"); element)
            element->QueryFloatText(&p_out.y);
    }
}

void Serializer::deserializeVec3(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, Vector3F &p_out) {
    if (auto node = p_node->FirstChildElement(p_name.c_str()); node) {
        if (auto element = node->FirstChildElement("x"); element)
            element->QueryFloatText(&p_out.x);
        
        if (auto element = node->FirstChildElement("y"); element)
            element->QueryFloatText(&p_out.y);
        
        if (auto element = node->FirstChildElement("z"); element)
            element->QueryFloatText(&p_out.z);
    }
}

void Serializer::deserializeVec4(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, Vector4F &p_out) {
    if (auto node = p_node->FirstChildElement(p_name.c_str()); node) {
        if (auto element = node->FirstChildElement("x"); element)
            element->QueryFloatText(&p_out.x);
        
        if (auto element = node->FirstChildElement("y"); element)
            element->QueryFloatText(&p_out.y);
        
        if (auto element = node->FirstChildElement("z"); element)
            element->QueryFloatText(&p_out.z);
        
        if (auto element = node->FirstChildElement("w"); element)
            element->QueryFloatText(&p_out.w);
    }
}

void Serializer::deserializeQuat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, QuaternionF &p_out) {
    if (auto node = p_node->FirstChildElement(p_name.c_str()); node) {
        if (auto element = node->FirstChildElement("x"); element)
            element->QueryFloatText(&p_out.x);
        
        if (auto element = node->FirstChildElement("y"); element)
            element->QueryFloatText(&p_out.y);
        
        if (auto element = node->FirstChildElement("z"); element)
            element->QueryFloatText(&p_out.z);
        
        if (auto element = node->FirstChildElement("w"); element)
            element->QueryFloatText(&p_out.w);
    }
}

void Serializer::deserializeColor(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name, Color &p_out) {
    if (auto node = p_node->FirstChildElement(p_name.c_str()); node) {
        if (auto element = node->FirstChildElement("r"); element)
            element->QueryFloatText(&p_out.r);
        
        if (auto element = node->FirstChildElement("g"); element)
            element->QueryFloatText(&p_out.g);
        
        if (auto element = node->FirstChildElement("b"); element)
            element->QueryFloatText(&p_out.b);
        
        if (auto element = node->FirstChildElement("q"); element)
            element->QueryFloatText(&p_out.a);
    }
}

//void Serializer::deserializeModel(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                  const std::string &p_name, OvRendering::Resources::Model *&p_out) {
//    if (std::string path = DeserializeString(p_doc, p_node, p_name.c_str()); path != "?" && path != "")
//        p_out = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>().GetResource(path);
//    else
//        p_out = nullptr;
//}
//
//void Serializer::deserializeTexture(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                    const std::string &p_name, OvRendering::Resources::Texture *&p_out) {
//    if (std::string path = DeserializeString(p_doc, p_node, p_name.c_str()); path != "?" && path != "")
//        p_out = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().GetResource(path);
//    else
//        p_out = nullptr;
//}
//
//void Serializer::deserializeShader(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                   const std::string &p_name, OvRendering::Resources::Shader *&p_out) {
//    if (std::string path = DeserializeString(p_doc, p_node, p_name.c_str()); path != "?" && path != "")
//        p_out = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>().GetResource(path);
//    else
//        p_out = nullptr;
//}
//
//void Serializer::deserializeMaterial(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                     const std::string &p_name, OvCore::Resources::Material *&p_out) {
//    if (std::string path = DeserializeString(p_doc, p_node, p_name.c_str()); path != "?" && path != "")
//        p_out = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().GetResource(path);
//    else
//        p_out = nullptr;
//}
//
//void Serializer::deserializeSound(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                  const std::string &p_name, OvAudio::Resources::Sound *&p_out) {
//    if (std::string path = DeserializeString(p_doc, p_node, p_name.c_str()); path != "?" && path != "")
//        p_out = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>().GetResource(path);
//    else
//        p_out = nullptr;
//}

bool Serializer::deserializeBoolean(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name) {
    bool result;
    deserializeBoolean(p_doc, p_node, p_name, result);
    return result;
}

std::string Serializer::deserializeString(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                          const std::string &p_name) {
    std::string result;
    deserializeString(p_doc, p_node, p_name, result);
    return result;
}

float Serializer::deserializeFloat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                   const std::string &p_name) {
    float result;
    deserializeFloat(p_doc, p_node, p_name, result);
    return result;
}

double Serializer::deserializeDouble(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                     const std::string &p_name) {
    double result;
    deserializeDouble(p_doc, p_node, p_name, result);
    return result;
}

int Serializer::deserializeInt(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name) {
    int result;
    deserializeInt(p_doc, p_node, p_name, result);
    return result;
}

unsigned Serializer::deserializeUint(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                     const std::string &p_name) {
    unsigned result;
    deserializeUint(p_doc, p_node, p_name, result);
    return result;
}

int64_t Serializer::deserializeInt64(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                     const std::string &p_name) {
    int64_t result;
    deserializeInt64(p_doc, p_node, p_name, result);
    return result;
}

Vector2F Serializer::deserializeVec2(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                     const std::string &p_name) {
    Vector2F result;
    deserializeVec2(p_doc, p_node, p_name, result);
    return result;
}

Vector3F Serializer::deserializeVec3(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                     const std::string &p_name) {
    Vector3F result;
    deserializeVec3(p_doc, p_node, p_name, result);
    return result;
}

Vector4F Serializer::deserializeVec4(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                     const std::string &p_name) {
    Vector4F result;
    deserializeVec4(p_doc, p_node, p_name, result);
    return result;
}

QuaternionF Serializer::deserializeQuat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                        const std::string &p_name) {
    QuaternionF result;
    deserializeQuat(p_doc, p_node, p_name, result);
    return result;
}

Color Serializer::deserializeColor(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                   const std::string &p_name) {
    Color result;
    deserializeColor(p_doc, p_node, p_name, result);
    return result;
}

//OvRendering::Resources::Model *Serializer::deserializeModel(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                            const std::string &p_name) {
//    OvRendering::Resources::Model *result;
//    DeserializeModel(p_doc, p_node, p_name, result);
//    return result;
//}
//
//OvRendering::Resources::Texture *Serializer::deserializeTexture(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                                const std::string &p_name) {
//    OvRendering::Resources::Texture *result;
//    DeserializeTexture(p_doc, p_node, p_name, result);
//    return result;
//}
//
//OvRendering::Resources::Shader *Serializer::deserializeShader(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                              const std::string &p_name) {
//    OvRendering::Resources::Shader *result;
//    DeserializeShader(p_doc, p_node, p_name, result);
//    return result;
//}
//
//OvCore::Resources::Material *Serializer::deserializeMaterial(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                             const std::string &p_name) {
//    OvCore::Resources::Material *result;
//    DeserializeMaterial(p_doc, p_node, p_name, result);
//    return result;
//}
//
//OvAudio::Resources::Sound *Serializer::deserializeSound(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                        const std::string &p_name) {
//    OvAudio::Resources::Sound *result;
//    DeserializeSound(p_doc, p_node, p_name, result);
//    return result;
//}


}

//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef serializer_hpp
#define serializer_hpp

#include "color.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"

#include <tinyxml2.h>
#include <string>

namespace vox {
class Serializer {
public:
    // MARK: - SERIALIZATION_HELPERS
    static void serializeBoolean(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, bool p_value);
    
    static void serializeString(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, const std::string &p_value);
    
    static void serializeFloat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, float p_value);
    
    static void serializeDouble(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, double p_value);
    
    static void serializeInt(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                             const std::string &p_name, int p_value);
    
    static void serializeUint(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                              const std::string &p_name, unsigned p_value);
    
    static void serializeInt64(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, int64_t p_value);
    
    static void serializeVec2(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                              const std::string &p_name, const Vector2F &p_value);
    
    static void serializeVec3(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                              const std::string &p_name, const Vector3F &p_value);
    
    static void serializeVec4(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                              const std::string &p_name, const Vector4F &p_value);
    
    static void serializeQuat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                              const std::string &p_name, const QuaternionF &p_value);
    
    static void serializeColor(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, const Color &p_value);
    
//    static void serializeModel(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                               const std::string &p_name, OvRendering::Resources::Model *p_value);
//
//    static void serializeTexture(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                 const std::string &p_name, OvRendering::Resources::Texture *p_value);
//
//    static void serializeShader(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                const std::string &p_name, OvRendering::Resources::Shader *p_value);
//
//    static void serializeMaterial(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                  const std::string &p_name, OvCore::Resources::Material *p_value);
//
//    static void serializeSound(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                               const std::string &p_name, OvAudio::Resources::Sound *p_value);
    
    // MARK: -  DESERIALIZATION_HELPERS
    static void deserializeBoolean(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                   const std::string &p_name, bool &p_out);
    
    static void deserializeString(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name, std::string &p_out);
    
    static void deserializeFloat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, float &p_out);
    
    static void deserializeDouble(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name, double &p_out);
    
    static void deserializeInt(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                               const std::string &p_name, int &p_out);
    
    static void deserializeUint(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, unsigned &p_out);
    
    static void deserializeInt64(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, int64_t &p_out);
    
    static void deserializeVec2(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, Vector2F &p_out);
    
    static void deserializeVec3(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, Vector3F &p_out);
    
    static void deserializeVec4(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, Vector4F &p_out);
    
    static void deserializeQuat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                const std::string &p_name, QuaternionF &p_out);
    
    static void deserializeColor(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                 const std::string &p_name, Color &p_out);
    
//    static void deserializeModel(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                 const std::string &p_name, OvRendering::Resources::Model *&p_out);
//
//    static void deserializeTexture(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                   const std::string &p_name, OvRendering::Resources::Texture *&p_out);
//
//    static void deserializeShader(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                  const std::string &p_name, OvRendering::Resources::Shader *&p_out);
//
//    static void deserializeMaterial(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                    const std::string &p_name, OvCore::Resources::Material *&p_out);
//
//    static void deserializeSound(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                 const std::string &p_name, OvAudio::Resources::Sound *&p_out);
    
    static bool deserializeBoolean(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                   const std::string &p_name);
    
    static std::string deserializeString(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                         const std::string &p_name);
    
    static float deserializeFloat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name);
    
    static double deserializeDouble(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name);
    
    static int deserializeInt(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                              const std::string &p_name);
    
    static unsigned deserializeUint(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name);
    
    static int64_t deserializeInt64(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name);
    
    static Vector2F deserializeVec2(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name);
    
    static Vector3F deserializeVec3(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name);
    
    static Vector4F deserializeVec4(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                    const std::string &p_name);
    
    static QuaternionF deserializeQuat(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                       const std::string &p_name);
    
    static Color deserializeColor(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
                                  const std::string &p_name);
    
//    static OvRendering::Resources::Model *deserializeModel(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                           const std::string &p_name);
//
//    static OvRendering::Resources::Texture *deserializeTexture(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                               const std::string &p_name);
//
//    static OvRendering::Resources::Shader *deserializeShader(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                             const std::string &p_name);
//
//    static OvCore::Resources::Material *deserializeMaterial(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                            const std::string &p_name);
//
//    static OvAudio::Resources::Sound *deserializeSound(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node,
//                                                       const std::string &p_name);
};


}

#endif /* serializer_hpp */

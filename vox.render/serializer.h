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
    static void serializeBoolean(nlohmann::json &data,
                                 const std::string &p_name, bool p_value);
    
    static void serializeString(nlohmann::json &data,
                                const std::string &p_name, const std::string &p_value);
    
    static void serializeFloat(nlohmann::json &data,
                               const std::string &p_name, float p_value);
    
    static void serializeDouble(nlohmann::json &data,
                                const std::string &p_name, double p_value);
    
    static void serializeInt(nlohmann::json &data,
                             const std::string &p_name, int p_value);
    
    static void serializeUint(nlohmann::json &data,
                              const std::string &p_name, unsigned p_value);
    
    static void serializeInt64(nlohmann::json &data,
                               const std::string &p_name, int64_t p_value);
    
    static void serializeVec2(nlohmann::json &data,
                              const std::string &p_name, const Vector2F &p_value);
    
    static void serializeVec3(nlohmann::json &data,
                              const std::string &p_name, const Vector3F &p_value);
    
    static void serializeVec4(nlohmann::json &data,
                              const std::string &p_name, const Vector4F &p_value);
    
    static void serializeQuat(nlohmann::json &data,
                              const std::string &p_name, const QuaternionF &p_value);
    
    static void serializeColor(nlohmann::json &data,
                               const std::string &p_name, const Color &p_value);
    
//    static void serializeModel(nlohmann::json &data,
//                               const std::string &p_name, OvRendering::Resources::Model *p_value);
//
//    static void serializeTexture(nlohmann::json &data,
//                                 const std::string &p_name, OvRendering::Resources::Texture *p_value);
//
//    static void serializeShader(nlohmann::json &data,
//                                const std::string &p_name, OvRendering::Resources::Shader *p_value);
//
//    static void serializeMaterial(nlohmann::json &data,
//                                  const std::string &p_name, OvCore::Resources::Material *p_value);
//
//    static void serializeSound(nlohmann::json &data,
//                               const std::string &p_name, OvAudio::Resources::Sound *p_value);
    
    // MARK: -  DESERIALIZATION_HELPERS
    static void deserializeBoolean(nlohmann::json &data,
                                   const std::string &p_name, bool &p_out);
    
    static void deserializeString(nlohmann::json &data,
                                  const std::string &p_name, std::string &p_out);
    
    static void deserializeFloat(nlohmann::json &data,
                                 const std::string &p_name, float &p_out);
    
    static void deserializeDouble(nlohmann::json &data,
                                  const std::string &p_name, double &p_out);
    
    static void deserializeInt(nlohmann::json &data,
                               const std::string &p_name, int &p_out);
    
    static void deserializeUint(nlohmann::json &data,
                                const std::string &p_name, unsigned &p_out);
    
    static void deserializeInt64(nlohmann::json &data,
                                 const std::string &p_name, int64_t &p_out);
    
    static void deserializeVec2(nlohmann::json &data,
                                const std::string &p_name, Vector2F &p_out);
    
    static void deserializeVec3(nlohmann::json &data,
                                const std::string &p_name, Vector3F &p_out);
    
    static void deserializeVec4(nlohmann::json &data,
                                const std::string &p_name, Vector4F &p_out);
    
    static void deserializeQuat(nlohmann::json &data,
                                const std::string &p_name, QuaternionF &p_out);
    
    static void deserializeColor(nlohmann::json &data,
                                 const std::string &p_name, Color &p_out);
    
//    static void deserializeModel(nlohmann::json &data,
//                                 const std::string &p_name, OvRendering::Resources::Model *&p_out);
//
//    static void deserializeTexture(nlohmann::json &data,
//                                   const std::string &p_name, OvRendering::Resources::Texture *&p_out);
//
//    static void deserializeShader(nlohmann::json &data,
//                                  const std::string &p_name, OvRendering::Resources::Shader *&p_out);
//
//    static void deserializeMaterial(nlohmann::json &data,
//                                    const std::string &p_name, OvCore::Resources::Material *&p_out);
//
//    static void deserializeSound(nlohmann::json &data,
//                                 const std::string &p_name, OvAudio::Resources::Sound *&p_out);
    
    static bool deserializeBoolean(nlohmann::json &data,
                                   const std::string &p_name);
    
    static std::string deserializeString(nlohmann::json &data,
                                         const std::string &p_name);
    
    static float deserializeFloat(nlohmann::json &data,
                                  const std::string &p_name);
    
    static double deserializeDouble(nlohmann::json &data,
                                    const std::string &p_name);
    
    static int deserializeInt(nlohmann::json &data,
                              const std::string &p_name);
    
    static unsigned deserializeUint(nlohmann::json &data,
                                    const std::string &p_name);
    
    static int64_t deserializeInt64(nlohmann::json &data,
                                    const std::string &p_name);
    
    static Vector2F deserializeVec2(nlohmann::json &data,
                                    const std::string &p_name);
    
    static Vector3F deserializeVec3(nlohmann::json &data,
                                    const std::string &p_name);
    
    static Vector4F deserializeVec4(nlohmann::json &data,
                                    const std::string &p_name);
    
    static QuaternionF deserializeQuat(nlohmann::json &data,
                                       const std::string &p_name);
    
    static Color deserializeColor(nlohmann::json &data,
                                  const std::string &p_name);
    
//    static OvRendering::Resources::Model *deserializeModel(nlohmann::json &data,
//                                                           const std::string &p_name);
//
//    static OvRendering::Resources::Texture *deserializeTexture(nlohmann::json &data,
//                                                               const std::string &p_name);
//
//    static OvRendering::Resources::Shader *deserializeShader(nlohmann::json &data,
//                                                             const std::string &p_name);
//
//    static OvCore::Resources::Material *deserializeMaterial(nlohmann::json &data,
//                                                            const std::string &p_name);
//
//    static OvAudio::Resources::Sound *deserializeSound(nlohmann::json &data,
//                                                       const std::string &p_name);
};


}

#endif /* serializer_hpp */

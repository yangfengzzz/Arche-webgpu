//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spherical_harmonics3.h"

namespace vox {

SphericalHarmonics3::SphericalHarmonics3() {
}

SphericalHarmonics3::SphericalHarmonics3(std::array<float, 27> coefficients) :
_coefficients(coefficients) {
}

const std::array<float, 27> &SphericalHarmonics3::coefficients() const {
    return _coefficients;
}

void SphericalHarmonics3::addLight(const Vector3F &direction, const Color &c, float deltaSolidAngle) {
    /**
     * Implements `EvalSHBasis` from [Projection from Cube maps] in http://www.ppsloan.org/publications/StupidSH36.pdf.
     *
     * Basis constants
     * 0: Math.sqrt(1/(4 * Math.PI))
     *
     * 1: -Math.sqrt(3 / (4 * Math.PI))
     * 2: Math.sqrt(3 / (4 * Math.PI))
     * 3: -Math.sqrt(3 / (4 * Math.PI))
     *
     * 4: Math.sqrt(15 / (4 * Math.PI))
     * 5: -Math.sqrt(15 / (4 * Math.PI))
     * 6: Math.sqrt(5 / (16 * Math.PI))
     * 7: -Math.sqrt(15 / (4 * Math.PI)）
     * 8: Math.sqrt(15 / (16 * Math.PI))
     */
    
    auto color = c * deltaSolidAngle;
    auto &coe = _coefficients;
    
    const auto bv0 = 0.282095; // basis0 = 0.886227
    const auto bv1 = -0.488603 * direction.y; // basis1 = -0.488603
    const auto bv2 = 0.488603 * direction.z; // basis2 = 0.488603
    const auto bv3 = -0.488603 * direction.x; // basis3 = -0.488603
    const auto bv4 = 1.092548 * (direction.x * direction.y); // basis4 = 1.092548
    const auto bv5 = -1.092548 * (direction.y * direction.z); // basis5 = -1.092548
    const auto bv6 = 0.315392 * (3 * direction.z * direction.z - 1); // basis6 = 0.315392
    const auto bv7 = -1.092548 * (direction.x * direction.z); // basis7 = -1.092548
    const auto bv8 = 0.546274 * (direction.x * direction.x - direction.y * direction.y); // basis8 = 0.546274
    
    (coe[0] += color.r * bv0);
    (coe[1] += color.g * bv0);
    (coe[2] += color.b * bv0);
    
    (coe[3] += color.r * bv1);
    (coe[4] += color.g * bv1);
    (coe[5] += color.b * bv1);
    (coe[6] += color.r * bv2);
    (coe[7] += color.g * bv2);
    (coe[8] += color.b * bv2);
    (coe[9] += color.r * bv3);
    (coe[10] += color.g * bv3);
    (coe[11] += color.b * bv3);
    
    (coe[12] += color.r * bv4);
    (coe[13] += color.g * bv4);
    (coe[14] += color.b * bv4);
    (coe[15] += color.r * bv5);
    (coe[16] += color.g * bv5);
    (coe[17] += color.b * bv5);
    (coe[18] += color.r * bv6);
    (coe[19] += color.g * bv6);
    (coe[20] += color.b * bv6);
    (coe[21] += color.r * bv7);
    (coe[22] += color.g * bv7);
    (coe[23] += color.b * bv7);
    (coe[24] += color.r * bv8);
    (coe[25] += color.g * bv8);
    (coe[26] += color.b * bv8);
}

Color SphericalHarmonics3::operator()(const Vector3F &direction) {
    /**
     * Equations based on data from: http://ppsloan.org/publications/StupidSH36.pdf
     *
     *
     * Basis constants
     * 0: Math.sqrt(1/(4 * Math.PI))
     *
     * 1: -Math.sqrt(3 / (4 * Math.PI))
     * 2: Math.sqrt(3 / (4 * Math.PI))
     * 3: -Math.sqrt(3 / (4 * Math.PI))
     *
     * 4: Math.sqrt(15 / (4 * Math.PI)）
     * 5: -Math.sqrt(15 / (4 * Math.PI))
     * 6: Math.sqrt(5 / (16 * Math.PI)）
     * 7: -Math.sqrt(15 / (4 * Math.PI)）
     * 8: Math.sqrt(15 / (16 * Math.PI)）
     *
     *
     * Convolution kernel
     * 0: Math.PI
     * 1: (2 * Math.PI) / 3
     * 2: Math.PI / 4
     */
    
    const auto &coe = _coefficients;
    
    const auto bv0 = 0.886227; // kernel0 * basis0 = 0.886227
    const auto bv1 = -1.023327 * direction.y; // kernel1 * basis1 = -1.023327
    const auto bv2 = 1.023327 * direction.z; // kernel1 * basis2 = 1.023327
    const auto bv3 = -1.023327 * direction.x; // kernel1 * basis3 = -1.023327
    const auto bv4 = 0.858086 * direction.y * direction.x; // kernel2 * basis4 = 0.858086
    const auto bv5 = -0.858086 * direction.y * direction.z; // kernel2 * basis5 = -0.858086
    const auto bv6 = 0.247708 * (3 * direction.z * direction.z - 1); // kernel2 * basis6 = 0.247708
    const auto bv7 = -0.858086 * direction.z * direction.x; // kernel2 * basis7 = -0.858086
    const auto bv8 = 0.429042 * (direction.x * direction.x - direction.y * direction.y); // kernel2 * basis8 = 0.429042
    
    // l0
    auto r = coe[0] * bv0;
    auto g = coe[1] * bv0;
    auto b = coe[2] * bv0;
    
    // l1
    r += coe[3] * bv1 + coe[6] * bv2 + coe[9] * bv3;
    g += coe[4] * bv1 + coe[7] * bv2 + coe[10] * bv3;
    b += coe[5] * bv1 + coe[8] * bv2 + coe[11] * bv3;
    
    // l2
    r += coe[12] * bv4 + coe[15] * bv5 + coe[18] * bv6 + coe[21] * bv7 + coe[24] * bv8;
    g += coe[13] * bv4 + coe[16] * bv5 + coe[19] * bv6 + coe[22] * bv7 + coe[25] * bv8;
    b += coe[14] * bv4 + coe[17] * bv5 + coe[20] * bv6 + coe[23] * bv7 + coe[26] * bv8;
    
    return Color(r, g, b, 1.0);
}

SphericalHarmonics3 SphericalHarmonics3::operator*(float s) {
    auto src = _coefficients;
    
    (src[0] *= s);
    (src[1] *= s);
    (src[2] *= s);
    (src[3] *= s);
    (src[4] *= s);
    (src[5] *= s);
    (src[6] *= s);
    (src[7] *= s);
    (src[8] *= s);
    (src[9] *= s);
    (src[10] *= s);
    (src[11] *= s);
    (src[12] *= s);
    (src[13] *= s);
    (src[14] *= s);
    (src[15] *= s);
    (src[16] *= s);
    (src[17] *= s);
    (src[18] *= s);
    (src[19] *= s);
    (src[20] *= s);
    (src[21] *= s);
    (src[22] *= s);
    (src[23] *= s);
    (src[24] *= s);
    (src[25] *= s);
    (src[26] *= s);
    return SphericalHarmonics3(src);
}

}

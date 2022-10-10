//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "apps/physics_samples/physics_debug_app.h"

// shapes
#include "apps/physics_samples/shapes/box_shape_app.h"
#include "apps/physics_samples/shapes/capsule_shape_app.h"
#include "apps/physics_samples/shapes/convex_hull_shape_app.h"
#include "apps/physics_samples/shapes/cylinder_shape_app.h"
// #include "apps/physics_samples/shapes/height_field_shape_app.h"
#include "apps/physics_samples/shapes/mesh_shape_app.h"
#include "apps/physics_samples/shapes/mutable_compound_shape_app.h"
#include "apps/physics_samples/shapes/offset_center_of_mass_shape_app.h"
#include "apps/physics_samples/shapes/rotated_translated_shape_app.h"
#include "apps/physics_samples/shapes/sphere_shape_app.h"
#include "apps/physics_samples/shapes/static_compound_shape_app.h"
#include "apps/physics_samples/shapes/tapered_capsule_shape_app.h"
#include "apps/physics_samples/shapes/triangle_shape_app.h"

// scaled shapes
#include "apps/physics_samples/scaled_shapes/scaled_box_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_capsule_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_convex_hull_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_cylinder_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_height_field_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_mesh_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_mutable_compound_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_offset_center_of_mass_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_sphere_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_static_compound_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_tapered_capsule_shape_app.h"
#include "apps/physics_samples/scaled_shapes/scaled_triangle_shape_app.h"

// water
#include "apps/physics_samples/water/water_shape_app.h"

// constraints
#include "apps/physics_samples/constraints/cone_constraint_app.h"
#include "apps/physics_samples/constraints/constraint_singularity_app.h"
#include "apps/physics_samples/constraints/distance_constraint_app.h"
#include "apps/physics_samples/constraints/fixed_constraint_app.h"
#include "apps/physics_samples/constraints/gear_constraint_app.h"
#include "apps/physics_samples/constraints/hinge_constraint_app.h"
// #include "apps/physics_samples/constraints/path_constraint_app.h"
#include "apps/physics_samples/constraints/point_constraint_app.h"
//#include "apps/physics_samples/constraints/powered_hinge_constraint_app.h"
//#include "apps/physics_samples/constraints/powered_slider_constraint_app.h"
//#include "apps/physics_samples/constraints/powered_swing_twist_constraint_app.h"
#include "apps/physics_samples/constraints/pulley_constraint_app.h"
#include "apps/physics_samples/constraints/rack_and_pinion_constraint_app.h"
//#include "apps/physics_samples/constraints/six_dof_constraint_app.h"
#include "apps/physics_samples/constraints/slider_constraint_app.h"
#include "apps/physics_samples/constraints/spring_app.h"
//#include "apps/physics_samples/constraints/swing_twist_constraint_app.h"
//#include "apps/physics_samples/constraints/swing_twist_constraint_friction_app.h"

// general
#include "apps/physics_samples/general/activate_during_update_app.h"
#include "apps/physics_samples/general/active_edges_app.h"
#include "apps/physics_samples/general/big_vs_small_app.h"
#include "apps/physics_samples/general/center_of_mass_app.h"
//#include "apps/physics_samples/general/change_motion_type_app.h"
//#include "apps/physics_samples/general/change_object_layer_app.h"
//#include "apps/physics_samples/general/change_shape_app.h"
//#include "apps/physics_samples/general/contact_listener_app.h"
#include "apps/physics_samples/general/contact_manifold_app.h"
#include "apps/physics_samples/general/damping_app.h"
#include "apps/physics_samples/general/dynamic_mesh_app.h"
//#include "apps/physics_samples/general/friction_per_triangle_app.h"
#include "apps/physics_samples/general/friction_app.h"
#include "apps/physics_samples/general/funnel_app.h"
#include "apps/physics_samples/general/gravity_factor_app.h"
#include "apps/physics_samples/general/heavy_on_light_app.h"
#include "apps/physics_samples/general/high_speed_app.h"
#include "apps/physics_samples/general/island_app.h"
#include "apps/physics_samples/general/kinematic_app.h"
#include "apps/physics_samples/general/manifold_reduction_app.h"
#include "apps/physics_samples/general/restitution_app.h"
//#include "apps/physics_samples/general/sensor_app.h"
//#include "apps/physics_samples/general/simple_app.h"
#include "apps/physics_samples/general/stack_app.h"
#include "apps/physics_samples/general/wall_app.h"
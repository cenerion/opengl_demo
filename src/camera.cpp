#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;


// mat4 Camera::calc_view(){
//     view = lookAt(m_position, normalize(m_target - m_position), up_dir);
//     return view;
// }

// mat4 Camera::calc_perpective(){
//     perspective = glm::perspective(
//                 radians(fov), 
//                 aspect, 
//                 near_plane, 
//                 far_plane
//             );

//     return perspective;
// }

//need to analyze and check
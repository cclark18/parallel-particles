#include "Camera.h"

Camera::Camera()
{
  eye = glm::vec3(-2.0f, 0.0f, 0.0f);
  lookat.x = eye.x + cos(phi * M_PI / 180) * cos(theta * M_PI / 180);
  lookat.y = eye.y + sin(phi * M_PI / 180);
  lookat.z = eye.z + cos(phi * M_PI / 180) * sin(-1.0 * theta * M_PI / 180);
  up = glm::vec3(0.0f, 1.0f, 0.0f);
  fov = DEFAULT_FOV;
  aspect = DEFAULT_ASPECT;
  _near = DEFAULT_NEAR;
  _far = DEFAULT_FAR;
}

Camera::Camera(glm::vec3 lookat, glm::vec3 eye, glm::vec3 up,
    float fov, float aspect, float _near, float _far)
{
  this->lookat = lookat;
  this->eye = eye;
  this->up = up;
  this->fov = fov;
  this->aspect = aspect;
  this->_near= _near;
  this->_far = _far;
}

glm::mat4 Camera::getView()
{
  return glm::lookAt(eye, lookat, up);
}

glm::mat4 Camera::getProjection()
{
  return glm::perspective(fov, aspect, _near, _far);
}

glm::vec3 Camera::getForward()
{
  return glm::normalize(lookat - eye);
}
glm::vec3 Camera::getStrafe()
{
  return glm::normalize(glm::cross(lookat - eye, up));
}
glm::vec3 Camera::getUp()
{
  return glm::normalize(up);
}

void Camera::moveHoriz(float step)
{
  theta += step * 80;

  lookat.x = eye.x + cos(phi * M_PI / 180) * cos(theta * M_PI / 180);
  lookat.y = eye.y + sin(phi * M_PI / 180);
  lookat.z = eye.z + cos(phi * M_PI / 180) * sin(-1.0 * theta * M_PI / 180);
}

void Camera::moveVert(float step)
{
  // note that ypos is measured from the top of the screen, so
  // an increase in ypos means moving the mouse down the y axis
  if ((phi < max_vert_angle && step < 0) || (phi > -1.0 * max_vert_angle && step > 0)) {
    phi -= step * 80;
  }

  lookat.x = eye.x + cos(phi * M_PI / 180) * cos(theta * M_PI / 180);
  lookat.y = eye.y + sin(phi * M_PI / 180);
  lookat.z = eye.z + cos(phi * M_PI / 180) * sin(-1.0 * theta * M_PI / 180);
}

#define MAX_LIGHTS 3

attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjMatrix; // perspective
uniform mat4 uViewMatrix; // camera
uniform mat4 uModelMatrix; // global (except camera)
uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;

varying vec3 vNormal;
varying vec3 vCol;
varying vec3 vPos;

void main() {
  vec3 pre_pos = vec3(uModelMatrix * vec4(aPosition, 1.0));
  gl_Position = uProjMatrix * uViewMatrix * vec4(pre_pos, 1.0); // vec4
  vec3 normal_fin = normalize(vec3(uModelMatrix * vec4(aNormal, 0.0)));

  vNormal = normal_fin; // normalized
  vPos = pre_pos;
}

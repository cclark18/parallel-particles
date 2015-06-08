attribute vec3 aPosition;
attribute vec3 aColor;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;

varying vec3 vColor;

void main() {
  gl_Position = uProjMatrix * uViewMatrix * vec4(aPosition, 1.0);
  vColor = aColor; // normalized
}

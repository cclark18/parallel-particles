uniform mat4 uModelMatrix;
uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform vec3 uCamPos;
uniform vec3 uLightPos;
uniform vec3 uLightCol;

varying vec3 vNormal;
varying vec3 vCol;
varying vec3 vPos;

void main() {
  vec3 color = vec3(0.0, 0.0, 0.0);
  vec3 light_color = vec3(0.5, 0.5, 0.243);
  vec3 ambient = vec3(0.05, 0.05, 0.05);
  vec3 surfacePos = vPos;
  vec3 surfaceToCamera = normalize(uCamPos - surfacePos);
  float alpha = 1.0;
  vec3 normal = normalize(vNormal);

  float visibility = 1.0;
  vec3 surfaceToLight = normalize(uLightPos - surfacePos);     
  //float dist = distance(vec2(uLightPos.x, uLightPos.z), vec2(surfacePos.x, surfacePos.z));     
  //float bias = 0.005 * tan(acos(dot(normal, surfaceToLight)));
  //bias = clamp(bias, 0.0, 0.01);

  vec3 diffuse = UdColor * dot(normal, surfaceToLight);
  diffuse.x = diffuse.x < 0.0 ? 0.0: diffuse.x;
  diffuse.y = diffuse.y < 0.0 ? 0.0: diffuse.y;
  diffuse.z = diffuse.z < 0.0 ? 0.0: diffuse.z;
  float temp = dot(surfaceToCamera, reflect(-surfaceToLight, normal));
  temp = temp < 0.0 ? 0.0: temp;
  vec3 specular = UsColor * pow(temp, Ushine); // n=1
  color += UaColor + diffuse + specular;
  alpha = 0.4;

  gl_FragColor = vec4(color.x * uLightCol.x, color.y * uLightCol.y, color.z * uLightCol.z, alpha);
}

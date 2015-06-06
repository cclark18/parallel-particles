#include "Handles.h"

bool Handles::installShaders(const std::string &vShaderName, const std::string &fShaderName) 
{
  GLint rc;

  // Create shader handles
  GLuint VS = glCreateShader(GL_VERTEX_SHADER);
  GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

  // Read shader sources
  const char *vshader = GLSL::textFileRead(vShaderName.c_str());
  const char *fshader = GLSL::textFileRead(fShaderName.c_str());
  glShaderSource(VS, 1, &vshader, NULL);
  glShaderSource(FS, 1, &fshader, NULL);

  // Compile vertex shader
  glCompileShader(VS);
  GLSL::printError();
  glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
  GLSL::printShaderInfoLog(VS);
  if(!rc) {
    printf("Error compiling vertex shader %s\n", vShaderName.c_str());
    return false;
  }

  // Compile fragment shader
  glCompileShader(FS);
  GLSL::printError();
  glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
  GLSL::printShaderInfoLog(FS);
  if(!rc) {
    printf("Error compiling fragment shader %s\n", fShaderName.c_str());
    return false;
  }

  // Create the program and link
  this->prog = glCreateProgram();
  glAttachShader(this->prog, VS);
  glAttachShader(this->prog, FS);
  glLinkProgram(this->prog);

  GLSL::printError();
  glGetProgramiv(this->prog, GL_LINK_STATUS, &rc);
  GLSL::printProgramInfoLog(this->prog);
  if(!rc) {
    printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
    return false;
  }

  /* get handles to attribute data */

  this->aPosition = GLSL::getAttribLocation(this->prog, "aPosition");
  this->aNormal = GLSL::getAttribLocation(this->prog, "aNormal");
  this->uProjMatrix = GLSL::getUniformLocation(this->prog, "uProjMatrix");
  this->uViewMatrix = GLSL::getUniformLocation(this->prog, "uViewMatrix");
  this->uModelMatrix = GLSL::getUniformLocation(this->prog, "uModelMatrix");
  this->uLightPos = GLSL::getUniformLocation(this->prog, "uLightPos");
  this->uLightCol = GLSL::getUniformLocation(this->prog, "uLightCol");
  this->uMatAmb = GLSL::getUniformLocation(this->prog, "UaColor");
  this->uMatDif = GLSL::getUniformLocation(this->prog, "UdColor");
  this->uMatSpec = GLSL::getUniformLocation(this->prog, "UsColor");
  this->uMatShine = GLSL::getUniformLocation(this->prog, "Ushine");
  this->uCamPos = GLSL::getUniformLocation(this->prog, "uCamPos");

  assert(glGetError() == GL_NO_ERROR);
  return true;
}

#include "PointHandles.h"
//#define DEBUG

bool PointHandles::installShaders(const std::string &vShaderName, const std::string &fShaderName) 
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
  this->uProjMatrix = GLSL::getUniformLocation(this->prog, "uProjMatrix");
  this->uViewMatrix = GLSL::getUniformLocation(this->prog, "uViewMatrix");
  this->aColor = GLSL::getAttribLocation(this->prog, "aColor");

  assert(glGetError() == GL_NO_ERROR);
  return true;
}

void PointHandles::draw(std::vector<float> points, std::vector<float> colors) {
  GLuint posbuf;
  GLuint colbuf;

#ifdef DEBUG
  if (points.size() > 0) {
    for (int i = 0; i < points.size(); i += 3) {
      std::cout << points[i*3+0] << ", " << points[i*3+0] << ", " << points[i*3+0] << std::endl;
    }
  }
#endif

  glGenBuffers(1, &posbuf);
  glGenBuffers(1, &colbuf);

  glBindBuffer(GL_ARRAY_BUFFER, posbuf);
  GLSL::enableVertexAttribArray(this->aPosition);
  glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(float), &points[0], GL_STATIC_DRAW);
  glVertexAttribPointer(this->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  glBindBuffer(GL_ARRAY_BUFFER, colbuf);
  GLSL::enableVertexAttribArray(this->aColor);
  glVertexAttribPointer(this->aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(float), &colors[0], GL_STATIC_DRAW);

  glDrawElements(GL_POINTS, points.size() / 3, GL_UNSIGNED_INT, 0);
}

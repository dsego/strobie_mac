/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


static inline void printError() {

  switch(glGetError()) {
    case GL_INVALID_ENUM:
      printf("GL_INVALID_ENUM\n");
      break;
    case GL_INVALID_VALUE:
      printf("GL_INVALID_VALUE\n");
      break;
    case GL_INVALID_OPERATION:
      printf("GL_INVALID_OPERATION\n");
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      printf("GL_INVALID_FRAMEBUFFER_OPERATION\n");
      break;
    case GL_OUT_OF_MEMORY:
      printf("GL_OUT_OF_MEMORY\n");
      break;
    case GL_NO_ERROR:
      printf("GL_NO_ERROR\n");
      break;
  }

}


static inline void printFramebufferStatus(GLenum target) {

  switch(glCheckFramebufferStatus(target)) {
    case GL_FRAMEBUFFER_UNDEFINED:
      printf("GL_FRAMEBUFFER_UNDEFINED\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      printf("GL_FRAMEBUFFER_UNSUPPORTED\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
      printf("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
      printf("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n");
      break;
  }

}


static inline void printShaderInfo(GLint shaderProgram) {

  GLint status;
  glValidateProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &status);
  if (status != GL_TRUE) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("%s\n", infoLog);
  }

}

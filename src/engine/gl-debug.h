// Copyright (c) 2025 Davorin Šego
// Licensed under the GNU General Public License v3.0 or later.
// See LICENSE file or <https://www.gnu.org/licenses/gpl-3.0.html>.


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

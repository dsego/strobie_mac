/*
  Copyright (C) 2013 Davorin Šego
*/

using GL;
using GLFW;
using Cairo;

public class GLCairoWindow {
  protected Cairo.ImageSurface surface;
  protected Cairo.Context context;
  protected unowned uchar[] surface_data;
  protected bool quit;

  protected int window_width;
  protected int window_height;

  protected GLuint[] textures     = new GLuint[1];
  // protected GLuint[] framebuffers = new GLuint[1];


  public GLCairoWindow(string title = "", int width = 400, int height = 300) {
    /* Initialize GLFW */
    if (! glfwInit()) {
      stderr.printf ("Could not create an OpenGL window.\n");
      Posix.exit(Posix.EXIT_FAILURE);
    }

    glfwOpenWindowHint(Target.WINDOW_NO_RESIZE, GL_TRUE);

    /* Open an OpenGL window */
    if (! glfwOpenWindow(width, height, 8, 8, 8, 8, 0, 0, Mode.WINDOW)) {
      glfwTerminate();
      stderr.printf("Could not create an OpenGL window.\n");
      Posix.exit(Posix.EXIT_FAILURE);
    }

    glfwSetWindowTitle(title);
    init_video(width, height);
  }

  ~GLCairoWindow() {
    glfwTerminate();
  }

  protected void init_video(int width, int height) {
    this.window_width  = width;
    this.window_height = height;
    this.surface       = new ImageSurface(Format.ARGB32, width, height);
    this.surface_data  = surface.get_data();
    this.context       = new Context(surface);

    // glGenFramebuffers(1, framebuffers);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    /* openGL texture */
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,  textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, null);
  }


  protected void flush() {
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[0]);
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // glDrawPixels(width, height, GL_BGRA, GL_UNSIGNED_BYTE, (GL.GLvoid[]) surface_data);
    // glClear(GL_COLOR_BUFFER_BIT);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window_width, window_height, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid[]) surface_data);

    // glTexSubImage2D (GL.GLenum target,
    //   GL.GLint level, GL.GLint xoffset, GL.GLint yoffset, GL.GLsizei width, GL.GLsizei height, GL.GLenum format, GL.GLenum type, [CCode (array_length = false, array_null_terminated = true)] GL.GLvoid[]? pixels);


    glBegin(GL_QUADS);
      glTexCoord2f(0f, 1f); glVertex2f(-1f, -1f);
      glTexCoord2f(1f, 1f); glVertex2f(1f, -1f);
      glTexCoord2f(1f, 0f); glVertex2f(1f, 1f);
      glTexCoord2f(0f, 0f); glVertex2f(-1f, 1f);
    glEnd();

    // glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[0] );
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glDrawBuffer(GL_BACK);
    glfwSwapBuffers();
  }

  protected void process_events() {
    flush();
    if (! (bool) glfwGetWindowParam(WindowParam.OPENED)) quit = true;
  }


  protected bool get_key(int key) {
    return glfwGetKey(key);
  }

}
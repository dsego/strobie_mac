/*
  Copyright (C) 2013 Davorin Šego
*/

using GL;
using GLFW;
using Cairo;

public class GLCairoWindow {
  protected Cairo.ImageSurface surface;
  protected Cairo.Context context;
  protected weak uchar[] surface_data;
  protected bool quit;

  protected int width;
  protected int height;

  protected unowned GLuint[] textures = new GLuint[1];


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
      stderr.printf ("Could not create an OpenGL window.\n");
      Posix.exit(Posix.EXIT_FAILURE);
    }

    glfwSetWindowTitle(title);
    glfwSwapInterval(1);
    init_video(width, height);
  }

  ~GLCairoWindow() {
    glfwTerminate();
  }

  protected void init_video(int width, int height) {
    this.width        = width;
    this.height       = height;
    this.surface      = new ImageSurface(Format.ARGB32, width, height);
    this.surface_data = surface.get_data();
    this.context      = new Context(surface);
  }

  protected void flush() {
    surface.flush();
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,  textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GL.GLvoid[]) surface_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /* The texture mapping coordinates are confusing as hell */
    glBegin(GL_QUADS);
      glTexCoord2f(0f, 1f); glVertex2f(-1f, -1f);
      glTexCoord2f(1f, 1f); glVertex2f(1f, -1f);
      glTexCoord2f(1f, 0f); glVertex2f(1f, 1f);
      glTexCoord2f(0f, 0f); glVertex2f(-1f, 1f);
    glEnd();
    glDeleteTextures(1, textures);
    glDrawBuffer(GL_BACK);
    glfwSwapBuffers();
  }

  protected void process_events() {
    flush();
    if (! (bool) glfwGetWindowParam(WindowParam.OPENED)) quit = true;
  }

}
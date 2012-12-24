/*
  Copyright (C) 2012 Davorin Šego
*/

using SDL;
using Cairo;

public class SDLCairoWindow {

  protected const int SCREEN_BPP     = 32;
  protected const int DELAY          = 10;
  protected const uint32 VIDEO_FLAGS = SurfaceFlag.DOUBLEBUF | SurfaceFlag.HWACCEL | SurfaceFlag.HWSURFACE | SurfaceFlag.RESIZABLE;
  protected unowned SDL.Screen screen;
  protected SDL.Surface surface;
  protected Cairo.Surface cairo_surface;
  protected Cairo.Context context;
  protected bool quit;

  public SDLCairoWindow(int width = 400, int height = 300) {
    SDL.init(InitFlag.VIDEO);
    init_video(width, height);
  }

  ~SDLCairoWindow() {
    SDL.quit();
  }

  void init_video(int width, int height) {
    this.screen = Screen.set_video_mode(width, height, SCREEN_BPP, VIDEO_FLAGS);
    if (this.screen == null) {
      stderr.printf ("Could not set video mode.\n");
    }
    this.surface       = new SDL.Surface.RGB(VIDEO_FLAGS, this.screen.w, this.screen.h, SCREEN_BPP, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
    this.cairo_surface = CairoSDL.surface_create(this.surface);
    this.context       = new Context(this.cairo_surface);
  }

  protected void process_events() {
    Event event;
    while (Event.poll(out event) == 1) {
      switch (event.type) {
        case EventType.QUIT:
          this.quit = true;
          break;
        case EventType.VIDEORESIZE:
          this.init_video(event.resize.w, event.resize.h);
          break;
      }
    }
  }

}
#include <time.h>
#include "engine.h"


int main(int argc, char *argv[])
{
  Engine* engine = Engine_create();
  Engine_init_audio(engine);
  Note note;
  note.frequency = 440;

  struct timespec t[] = (struct timespec[]){{0, 200000000}};

  while (true) {
    Engine_read_strobes(engine, note);
    Engine_estimate_pitch(engine);
    nanosleep(t, NULL);
  }
  return 0;
}



static int strobieVersion[2] = { 1, 0 };

static int newVersionAvailable(char* string) {
  int minor;
  int major;
  sscanf(string, "v%d.%d", &major, &minor);
  return (major > strobieVersion[0] || (major >= strobieVersion[0] && minor > strobieVersion[1]));
}

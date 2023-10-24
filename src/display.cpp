#include <zephyr/display/mb_display.h>

static mb_image smile[] = {
  MB_IMAGE(
    { 0, 1, 0, 1, 0 },
    { 0, 1, 0, 1, 0 },
    { 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 1 },
    { 0, 1, 1, 1, 0 }
  )
};

static mb_image laugh[] = {
  MB_IMAGE(
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1 },
    { 0, 1, 1, 1, 0 }
  ),
  MB_IMAGE(
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0 }
  )
};

static mb_image sad[] = {
  MB_IMAGE(
    { 1, 0, 0, 0, 0 },
    { 0, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
  ),
  MB_IMAGE(
    { 0, 0, 0, 0, 1 },
    { 1, 1, 0, 1, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
  ),
  MB_IMAGE(
    { 1, 0, 0, 0, 1 },
    { 0, 1, 0, 1, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
  ),
  MB_IMAGE(
    { 0, 0, 0, 0, 0 },
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
  )
};

static mb_image woot[] = {
  MB_IMAGE(
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 1, 0, 1, 0 },
    { 0, 0, 1, 0, 0 }
  )
};

void show_smile(int duration, int index) {
  mb_display *disp = mb_display_get();
  mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, duration * 1000, &smile[index], 1);
}

void show_laugh(int duration, int index) {
  mb_display *disp = mb_display_get();
  mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, duration * 1000, &laugh[index], 1);
}

void show_sad(int duration, int index) {
  mb_display *disp = mb_display_get();
  mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, duration * 1000, &sad[index], 1);
}

void show_woot(int duration, int index) {
  mb_display *disp = mb_display_get();
  mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, duration * 1000, &woot[index], 1);
}

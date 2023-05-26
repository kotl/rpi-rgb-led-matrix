#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <tuple>

#include "led-matrix.h"
#include "tetris.h"

using rgb_matrix::Canvas;
using rgb_matrix::FrameCanvas;
using rgb_matrix::RGBMatrix;
using tetris::interrupt_received;
using tetris::TetrisString;

static void InterruptHandler(int signo) { interrupt_received = true; }

int main(int argc, char *argv[]) {
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "regular";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.cols = 64;
  defaults.multiplexing = 1;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = false;

  RGBMatrix *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);

  if (argc == 1) {
    printf(
        "Usage: tetris_string x1_coord y1_coord string1 x2_coord y2_coord "
        "string2 delay_seconds delay_usec scale\n");
    return 1;
  }

  if (canvas == NULL || argc < 10) return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  tetris::set_delay(atoi(argv[8]));
  tetris::set_scale(atoi(argv[9]));
  TetrisString y = TetrisString(atoi(argv[4]), atoi(argv[5]), argv[6], canvas);
  y.animate(canvas);

  TetrisString x = TetrisString(atoi(argv[1]), atoi(argv[2]), argv[3], canvas);
  x.animate(canvas);

  int msec = atoi(argv[7]) * 1000;
  while (!interrupt_received && msec > 0) {
    usleep(100 * 1000);
    msec -= 100;
  }

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}

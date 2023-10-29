#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <ctime>
#include <tuple>

#include "led-matrix.h"
#include "tetris.h"

using rgb_matrix::Canvas;
using rgb_matrix::FrameCanvas;
using rgb_matrix::RGBMatrix;
using tetris::interrupt_received;
using tetris::TetrisString;

static void InterruptHandler(int signo) { interrupt_received = true; }

static void drawRect(Canvas *canvas) {
  int w = canvas->width();
  int h = canvas->height();
  for (int x = 0; x < w; x++) {
    if (interrupt_received) return;
    canvas->SetPixel(x, 0, 0, 0, 255);
    canvas->SetPixel(x, h - 1, 255, 255, 0);
  }

  for (int y = 0; y < h / 2; y++) {
    if (interrupt_received) return;
    canvas->SetPixel(0, y + h / 2, 255, 255, 0);
    canvas->SetPixel(w - 1, y + h / 2, 255, 255, 0);

    canvas->SetPixel(0, y, 0, 0, 255);
    canvas->SetPixel(w - 1, y, 0, 0, 255);
  }
}

static void removeRow(Canvas *canvas, int row, int dir) {
  int w = canvas->width();
  for (int x = 1; x < w-1; x++) {
    if (interrupt_received) return;
    if (dir == 0) {
      canvas->SetPixel(x, row*2+2, 10, 10, 100);
      canvas->SetPixel(x, row*2+1, 10, 10, 100);
    } else {
      canvas->SetPixel(w - x - 1, row*2+2, 100, 100, 10);
      canvas->SetPixel(w - x - 1, row*2+1, 100, 100, 10);
    }
  }
}


static void removeCol(Canvas *canvas, int col) {
  int w = canvas->width();
  int h = canvas->height();

  for (int y = 0; y < h / 2; y++) {
    if (interrupt_received) return;
    canvas->SetPixel(col+1, y + h / 2, 255, 255, 0);
    canvas->SetPixel(w - 2-col, y + h / 2, 255, 255, 0);

    canvas->SetPixel(col+1, y, 0, 0, 255);
    canvas->SetPixel(w - 2-col, y, 0, 0, 255);
  }
}

char buf[6];

std::string generate_time_str() {
  std::time_t t = std::time(0);  // get time now
  std::tm *now = std::localtime(&t);
  snprintf(buf, 6, "%02d %02d", now->tm_hour, now->tm_min);
  return std::string(buf);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Usage: tetris_clock speed_delay_usec freq_delay_seconds brightness\n");
    return 1;
  }

  RGBMatrix::Options defaults;

  int brightness = atoi(argv[3]);

  defaults.hardware_mapping = "regular";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.cols = 64;
  defaults.multiplexing = 1;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = false;
  defaults.brightness = brightness;

  RGBMatrix *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);

  if (canvas == NULL) return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  int delay = atoi(argv[1]);
  tetris::set_delay(delay);
  tetris::set_scale(2);
  int freq_sec = atoi(argv[2]);

  FrameCanvas *border_canvas = canvas->CreateFrameCanvas();
  drawRect(border_canvas);
  drawRect(canvas);

  TetrisString y = TetrisString(13, 0, ":", canvas);
  TetrisString x = TetrisString(1, 0, generate_time_str(), canvas);

  int w = canvas->width();
  int h = canvas->height();

  while (!interrupt_received) {
    y.animate(canvas);
    x.animate(canvas);

    int msec = freq_sec * 1000;
    while (!interrupt_received && msec > 0) {
      usleep(100 * 1000);
      msec -= 100;
    }

    for (int col = 0; col < w/2; col++) {
      removeCol(canvas, col);
      if (!interrupt_received) {
        usleep(delay/4);
      }
    }

    y.reset(":");
    x.reset(generate_time_str());
    border_canvas = canvas->SwapOnVSync(border_canvas);
    border_canvas->CopyFrom(*canvas->SwapOnVSync(NULL));
  }

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}

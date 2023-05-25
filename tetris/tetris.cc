#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <tuple>

#include "led-matrix.h"
#include "tetris_data.h"

using rgb_matrix::Canvas;
using rgb_matrix::FrameCanvas;
using rgb_matrix::RGBMatrix;

int scale = 1;
int bottom_shift = 2;
int CHAR_HEIGHT = 16;
int CHAR_WIDTH = 5;
int CHAR_KERNING = 2;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) { interrupt_received = true; }

std::vector<std::vector<int>> get_tetris_num(int n) {
  return tetris_data::mynums[n];
}

std::vector<std::vector<int>> get_tetris_char(char ascii) {
  int n = static_cast<int>(ascii);
  if (n < 33 || n > 90) {
    return std::vector<std::vector<int>>();
  }
  return tetris_data::mychars[n - 33];
}

bool set_pixel(Canvas *tcanvas, int x, int x1, int x2, int y, int y1, int y2,
               std::tuple<int, int, int> color, int y_pos) {
  bool ret = false;
  int height = tcanvas->height();
  int width = tcanvas->width();
  for (int i = x + x1 * scale; i < x + x2 * scale; i++) {
    for (int j = y + y1 * scale; j < y + y2 * scale; j++) {
      if (i < width && j < height) {
        tcanvas->SetPixel(i, j, std::get<0>(color), std::get<1>(color),
                          std::get<2>(color));
      }
      if (j == (y_pos - 1)) {
        ret = true;
      }
    }
  }
  return ret;
}

bool draw_shape(Canvas *tcanvas, int x, int y, std::tuple<int, int, int> color,
                int shape, int rotate, int y_pos) {
  int rot = rotate % 4;
  bool ret1 = false;
  bool ret2 = false;
  if (shape == 0) {  // rectangle
    if (rot == 0 || rot == 1 || rot == 2 || rot == 3) {
      ret1 = set_pixel(tcanvas, x, 0, 2, y, 0, 2, color, y_pos);
    }
  } else if (shape == 1) {
    if (rot == 3) {
      ret1 = set_pixel(tcanvas, x, 2, 3, y, 1, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 3, y, 2, 3, color, y_pos);
    } else if (rot == 0) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 0, 3, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 2, 3, color, y_pos);
    } else if (rot == 1) {
      ret1 = set_pixel(tcanvas, x, 0, 3, y, 1, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 1, y, 2, 3, color, y_pos);
    } else if (rot == 2) {
      ret1 = set_pixel(tcanvas, x, 0, 2, y, 1, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 2, 4, color, y_pos);
    }
  } else if (shape == 2) {
    if (rot == 1) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 1, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 3, y, 2, 3, color, y_pos);
    } else if (rot == 2) {
      ret1 = set_pixel(tcanvas, x, 0, 2, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 1, y, 0, 3, color, y_pos);
    } else if (rot == 3) {
      ret1 = set_pixel(tcanvas, x, 0, 3, y, 1, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 2, 3, y, 2, 3, color, y_pos);
    } else if (rot == 0) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 2, 3, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 0, 3, color, y_pos);
    }
  } else if (shape == 3) {
    if (rot == 0 || rot == 2) {
      ret1 = set_pixel(tcanvas, x, 0, 4, y, 2, 3, color, y_pos);
    } else if (rot == 1 || rot == 3) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 0, 4, color, y_pos);
    }
  } else if (shape == 4) {
    if (rot == 0) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 0, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 1, 3, color, y_pos);
    } else if (rot == 1) {
      ret1 = set_pixel(tcanvas, x, 1, 3, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 2, y, 1, 2, color, y_pos);
    } else if (rot == 2) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 0, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 1, 3, color, y_pos);
    } else if (rot == 3) {
      ret1 = set_pixel(tcanvas, x, 1, 3, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 2, y, 1, 2, color, y_pos);
    }
  } else if (shape == 5) {
    if (rot == 0) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 1, 3, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 0, 2, color, y_pos);
    } else if (rot == 1) {
      ret1 = set_pixel(tcanvas, x, 0, 2, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 3, y, 1, 2, color, y_pos);
    } else if (rot == 2) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 1, 3, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 0, 2, color, y_pos);
    } else if (rot == 3) {
      ret1 = set_pixel(tcanvas, x, 0, 2, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 3, y, 1, 2, color, y_pos);
    }
  } else if (shape == 6) {
    if (rot == 0) {
      ret1 = set_pixel(tcanvas, x, 1, 2, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 3, y, 1, 2, color, y_pos);
    } else if (rot == 1) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 0, 3, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 1, 2, color, y_pos);
    } else if (rot == 2) {
      ret1 = set_pixel(tcanvas, x, 0, 3, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 1, 2, color, y_pos);
    } else if (rot == 3) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 1, 2, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 0, 3, color, y_pos);
    }
  } else if (shape == 7) {
    if (rot == 0) {
      ret1 = set_pixel(tcanvas, x, 0, 1, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 2, y, 1, 2, color, y_pos);
    } else if (rot == 1) {
      ret1 = set_pixel(tcanvas, x, 0, 2, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 1, y, 1, 2, color, y_pos);
    } else if (rot == 2) {
      ret1 = set_pixel(tcanvas, x, 0, 2, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 1, 2, y, 1, 2, color, y_pos);
    } else if (rot == 3) {
      ret1 = set_pixel(tcanvas, x, 1, 2, y, 0, 1, color, y_pos);
      ret2 = set_pixel(tcanvas, x, 0, 2, y, 1, 2, color, y_pos);
    }
  }
  return ret1 || ret2;
}

class TetrisChar {
 private:
  int xpos;
  int scale;
  int movement_cnt;
  int shape_index;
  int shape_rot;
  int shape_ypos;
  std::string type;
  std::vector<std::vector<int>> val;
  int shape_num;
  std::vector<bool> shape_move_end;

  void reset_shape() {
    shape_rot = 0;
    shape_ypos = 0;
  }

  void init_val() {
    shape_num = this->val.size();
    shape_move_end = std::vector<bool>(shape_num, false);
  }

 public:
  bool move_end;
  int ypos;

  TetrisChar(int xpos, int ypos, int scale) {
    this->xpos = xpos;
    this->ypos = ypos;
    this->scale = scale;
    movement_cnt = 0;
    move_end = false;
    shape_index = 0;
    reset_shape();
  }

  void set_char(char val) {
    type = "ASCII";
    this->val = get_tetris_char(val);
    init_val();
  }

  void set_num(int val) {
    type = "NUMBER";
    this->val = get_tetris_num(val);
    init_val();
  }

  bool animate(Canvas *tcanv, Canvas *rcanv, int ypos) {
    std::vector<int> i = val[shape_index];
    int shape = i[0];
    int x_pos = i[2] + xpos;
    int y_pos = i[3] + ypos - bottom_shift;
    int rotation = i[4];
    bool ret = false;
    ret = draw_shape(tcanv, x_pos * scale, shape_ypos * scale,
                     tetris_data::myCOLORS[i[1]], shape, shape_rot,
                     y_pos * scale);

    if (ret == true) {
      draw_shape(rcanv, x_pos * scale, shape_ypos * scale,
                 tetris_data::myCOLORS[i[1]], shape, shape_rot, y_pos * scale);
      shape_move_end[shape_index] = true;
      if (shape_index == (shape_num - 1)) {
        move_end = true;
      } else {
        shape_index += 1;
      }
      reset_shape();
    } else {
      if (shape_rot < rotation) {
        shape_rot += 1;
      }
      shape_ypos += 1;
    }

    return ret;
  }
};

class TetrisString {
 private:
  int xpos;
  int ypos;
  std::string str;
  std::vector<TetrisChar> string;
  FrameCanvas *tcanv;
  FrameCanvas *rcanv;
  FrameCanvas *canvas;

 public:
  TetrisString(int xpos, int ypos, const std::string &str, RGBMatrix *matrix) {
    this->tcanv = matrix->CreateFrameCanvas();
    this->rcanv = matrix->CreateFrameCanvas();
    this->canvas = matrix->CreateFrameCanvas();

    this->xpos = xpos;
    this->ypos = ypos;
    this->str = str;
    float index = 0.0;
    for (char ascii : str) {
      if (ascii == ' ') {
        index += 0.5;
        continue;
      }
      TetrisChar A(xpos + int((CHAR_WIDTH + CHAR_KERNING) * index), ypos,
                   scale);
      A.set_char(ascii);
      string.push_back(A);
      index += 1.0;
    }
  }

  bool animate(RGBMatrix *matrix) {
    bool ret = false;
    bool not_finished = true;
    FrameCanvas *current = matrix->SwapOnVSync(NULL);
    tcanv->CopyFrom(*current);
    rcanv->CopyFrom(*current);
    canvas->CopyFrom(*current);
    while (not_finished) {
      not_finished = false;
      int index = 0;
      std::vector<bool> move_end(string.size(), false);
      for (auto &tetrisChar : string) {
        if (!tetrisChar.move_end) {
          ret = tetrisChar.animate(tcanv, rcanv, tetrisChar.ypos);
          if (ret == true) {
            canvas->CopyFrom(*rcanv);
          }
          // printf("  ret=%d\n", ret);
          if (!tetrisChar.move_end) {
            not_finished = true;
          }
        }
        index += 1;
      }

      tcanv = matrix->SwapOnVSync(tcanv);
      tcanv->CopyFrom(*canvas);
      usleep(50000);  // wait a little to slow down things.
    }

    return ret;
  }
};

static void DrawOnCanvas(RGBMatrix *canvas, char *text1, char *text2) {
  TetrisString y = TetrisString(1, 16, text2, canvas);
  y.animate(canvas);

  TetrisString x = TetrisString(1, 1, text1, canvas);
  x.animate(canvas);
}

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

  if (canvas == NULL || argc < 3) return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  DrawOnCanvas(canvas, argv[1], argv[2]);  // Using the canvas.
  while (!interrupt_received) {
    usleep(100000);
  }

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}

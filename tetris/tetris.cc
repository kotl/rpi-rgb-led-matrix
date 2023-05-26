#include "tetris.h"

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

namespace tetris {

volatile bool interrupt_received = false;

std::vector<std::vector<int>> get_tetris_num(int n) {
  return tetris_data::mynums[n];
}

int delay = 50000;
int scale = 1;
int bottom_shift = 1;
int CHAR_HEIGHT = 16;
int CHAR_WIDTH = 5;
int CHAR_KERNING = 2;

void set_scale(int new_scale) { scale = new_scale; }

void set_bottom_shift(int new_bottom_shift) { bottom_shift = new_bottom_shift; }

void set_delay(int new_delay) { delay = new_delay; }

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

void TetrisChar::reset_shape() {
  shape_rot = 0;
  shape_ypos = 0;
}

void TetrisChar::init_val() {
  shape_num = this->val.size();
  shape_move_end = std::vector<bool>(shape_num, false);
}

TetrisChar::TetrisChar(int xpos, int ypos, int scale) {
  this->xpos = xpos;
  this->ypos = ypos;
  this->scale = scale;
  movement_cnt = 0;
  move_end = false;
  shape_index = 0;
  reset_shape();
}

void TetrisChar::set_char(char val) {
  type = "ASCII";
  this->val = get_tetris_char(val);
  init_val();
}

void TetrisChar::set_num(int val) {
  type = "NUMBER";
  this->val = get_tetris_num(val);
  init_val();
}

bool TetrisChar::animate(Canvas *tcanv, Canvas *rcanv, int ypos) {
  std::vector<int> i = val[shape_index];
  int shape = i[0];
  int x_pos = i[2] + xpos;
  int y_pos = i[3] + ypos - bottom_shift;
  int rotation = i[4];
  bool ret = false;
  ret =
      draw_shape(tcanv, x_pos * scale, shape_ypos * scale,
                 tetris_data::myCOLORS[i[1]], shape, shape_rot, y_pos * scale);

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

void TetrisString::reset(const std::string &str) {
  this->str = str;
  float index = 0.0;
  this->string.clear();
  for (char ascii : str) {
    if (ascii == ' ') {
      index += 0.5;
      continue;
    }
    TetrisChar A(xpos + int((CHAR_WIDTH + CHAR_KERNING) * index), ypos, scale);
    A.set_char(ascii);
    string.push_back(A);
    index += 1.0;
  }
}

TetrisString::TetrisString(int xpos, int ypos, const std::string &str,
                           RGBMatrix *matrix) {
  this->tcanv = matrix->CreateFrameCanvas();
  this->rcanv = matrix->CreateFrameCanvas();
  this->canvas = matrix->CreateFrameCanvas();
  this->xpos = xpos;
  this->ypos = ypos;
  this->reset(str);
}

bool TetrisString::animate(RGBMatrix *matrix) {
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
    if (!interrupt_received) {
      usleep(delay);  // wait a little to slow down things.
    }
  }

  return ret;
}
}  // namespace tetris
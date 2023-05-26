#ifndef TETRIS_H
#define TETRIS_H

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

// define your own namespace to hold constants
namespace tetris
{

extern volatile bool interrupt_received;

 void set_scale(int);
 void set_delay(int);

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

  void reset_shape();

  void init_val();

 public:
  bool move_end;
  int ypos;

  TetrisChar(int xpos, int ypos, int scale);

  void set_char(char val);

  void set_num(int val);

  bool animate(Canvas *tcanv, Canvas *rcanv, int ypos);
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
  TetrisString(int xpos, int ypos, const std::string &str, RGBMatrix *matrix);
  void reset(const std::string &str);
  bool animate(RGBMatrix *matrix);
};


}
#endif

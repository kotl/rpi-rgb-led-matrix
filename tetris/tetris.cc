// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <tuple>

#include "tetris_data.h"


using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

int scale = 1;
// int x_shift = 2;
int bottom_shift = 1;
int CHAR_HEIGHT = 16;
int CHAR_WIDTH = 5;
int CHAR_KERNING = 2;


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

void set_scale(int s) {
    scale = s;
}

void set_bottom_shift(int s) {
    bottom_shift = s;
}


bool set_pixel(Canvas *tcanvas, int x, int x1, int x2, int y, int y1, int y2, std::tuple<int, int, int> color, int y_pos) {
    bool ret = false;
    int height = tcanvas->height();
    int width = tcanvas->width();
    for (int i = x + x1 * scale; i < x + x2 * scale; i++) {
        for (int j = y + y1 * scale; j < y + y2 * scale; j++) {
            if (i < width && j < height) {
                tcanvas->SetPixel(i, j, std::get<0>(color), std::get<1>(color), std::get<2>(color));
            }
            if (j == (y_pos - 1)) {
                ret = true;
            }
        }
    }
    return ret;
}

bool draw_shape(Canvas *tcanvas, int x, int y, std::tuple<int, int, int>  color, int shape, int rotate, int y_pos) {
    int rot = rotate % 4;
    bool ret1 = false;
    bool ret2 = false;
    if (shape == 0) { // rectangle
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

/*
class TetrisChar {
private:
    int xpos;
    int ypos;
    int scale;
    int movement_cnt;
    bool move_end;
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

    void init_val(const std::string& val) {
        shape_num = val.size();
        shape_move_end = std::vector<bool>(shape_num, false);
    }

public:
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
        init_val(std::string(1, val));
    }

    void set_num(int val) {
        type = "NUMBER";
        this->val = get_tetris_num(val);
        init_val(std::to_string(val));
    }

    std::tuple<std::vector<std::vector<std::tuple<int, int, int>>>, std::vector<std::vector<std::tuple<int, int, int>>>, bool> animate(std::vector<std::vector<std::tuple<int, int, int>>>& rcanv, Canvas  *tcanv, int ypos) {
        std::vector<int> i = val[shape_index];
        int shape = i[0];
        int x_pos = i[2] + xpos;
        int y_pos = i[3] + ypos - bottom_shift;
        int rotation = i[4];
        Canvas *mycanvas;
        bool ret = false;
        std::tie(mycanvas, ret) = draw_shape(tcanv, x_pos * scale, shape_ypos * scale, myCOLORS[i[1]], shape, shape_rot, y_pos * scale);

        if (ret == true) {
            std::tie(rcanv, std::ignore) = draw_shape(rcanv, x_pos * scale, shape_ypos * scale, myCOLORS[i[1]], shape, shape_rot, y_pos * scale);
            shape_move_end[shape_index] = true;
            if (shape_index == (shape_num - 1)) {
                move_end = true;
            }
            else {
                shape_index += 1;
            }
            reset_shape();
        }
        else {
            if (shape_rot < rotation) {
                shape_rot += 1;
            }
            shape_ypos += 1;
        }

        return std::make_tuple(mycanvas, rcanv, ret);
    }
};

 
class TetrisString {
private:
    int xpos;
    int ypos;
    std::string str;
    std::vector<TetrisChar> string;

public:
    TetrisString(int xpos, int ypos, const std::string& str) {
        this->xpos = xpos;
        this->ypos = ypos;
        this->str = str;
        float index = 0.0;
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

    std::vector<std::vector<std::tuple<int, int, int>>> animate(std::vector<std::vector<std::tuple<int, int, int>>>& matrix, std::vector<std::vector<std::tuple<int, int, int>>>& led_buffer) {
        std::vector<std::vector<std::tuple<int, int, int>>> tcanv = canvas;
        std::vector<std::vector<std::tuple<int, int, int>>> rcanv = canvas;

        int h = tcanv.size();
        int w = tcanv[0].size();
        while (true) {
            int index = 0;
            std::vector<bool> move_end(string.size(), false);
            for (auto& tetrisChar : string) {
                if (!tetrisChar.move_end) {
                    std::vector<std::vector<std::tuple<int, int, int>>> mycanvas;
                    bool ret;
                    std::tie(mycanvas, rcanv, ret) = tetrisChar.animate(rcanv, tcanv, tetrisChar.ypos);
                    if (ret == true) {
                        canvas = rcanv;
                    }
                    move_end[index] = tetrisChar.move_end;
                }
                index += 1;
            }

            Image im_pil = ImageFromArray(tcanv);
            led_buffer.SetImage(im_pil);
            led_buffer = matrix.SwapOnVSync(led_buffer);

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            if (std::all_of(move_end.begin(), move_end.end(), [](bool val) { return val; })) {
                break;
            }
            tcanv = canvas;
        }

        return tcanv;
    }
};


*/

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}


static void drawRect(Canvas *canvas, int r, int g, int b) {
  int w = canvas->width();
  int h = canvas->height();
  for (int x = 0; x < w; x++) {
    if (interrupt_received)
      return;
    canvas->SetPixel(x, 0,
                     r,g,b);
    canvas->SetPixel(x, h-1,
                     r,g,b);
    usleep(2 * 1000);  // wait a little to slow down things.
  }

  for (int y = 0; y < h; y++) {
    if (interrupt_received)
      return;
    canvas->SetPixel(0,y,
                     r,g,b);
    canvas->SetPixel(w-1, y,
                     r,g,b);
    usleep(2 * 1000);  // wait a little to slow down things.
  }

}

static void DrawOnCanvas(Canvas *canvas) {
  /*
   * Let's create a simple animation. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   */

  drawRect(canvas, 255, 0, 0);

  draw_shape(canvas, 10, 10, std::make_tuple(0,255,255), 0, 1, 5);

  usleep(20000 * 1000);  // wait a little to slow down things.

/* drawRect(canvas, 0, 255, 255);
  usleep(2000 * 1000);  // wait a little to slow down things.
  drawRect(canvas, 0, 255, 0);
  usleep(2000 * 1000);  // wait a little to slow down things.
*/
}


int main(int argc, char *argv[]) {
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "regular";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.cols = 64;
  defaults.multiplexing = 1;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = true;
  Canvas *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL)
    return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}

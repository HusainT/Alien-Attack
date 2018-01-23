#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();
void print_word_lives();
void print_word_level();
void player_movement();

// y limit
const int Y_MAX = 15;

// enemy array
const int MAX_DRONES = 40;
const int MAX_HIVES = 3;

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(7, 0, 0);
const Color ORANGE(7, 4, 0);
const Color YELLOW(7, 7, 0);
const Color GREEN(0, 7, 0);
const Color BLUE(0, 0, 7);
const Color PURPLE(7, 0, 7);
const Color WHITE(7, 7, 7);
const Color LIME(4, 7, 0);
const Color AQUA(0, 7, 7);

class Drone {
  public:
    // Constructors
    Drone() {
      x = 0;
      y = 0;
      strength = 0;
    }
    // sets values for private date members x and y
    Drone(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    // sets values for private data members
    Drone(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    // getters for moving bools
    bool get_moving_northeast() {
      return moving_northeast;
    }
    bool get_moving_northwest() {
      return moving_northwest;
    }
    bool get_moving_southeast() {
      return moving_southeast;
    }
    bool get_moving_southwest() {
      return moving_southwest;
    }
    void drone_hit() {
      strength = strength - 1;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }

    // getters
    int get_x() const {
      return x;
    }

    int get_y() const {
      return y;
    }

    int get_strength() const {
      return strength;
    }

    void set_origin(int origin_arg) {
      origin = origin_arg;
    }

    int get_origin() {
      return origin;
    }


    // Moves the Drone around the screen
    // Modifies: x, y
    void move_northeast() {
      //writing in game function
      x = x + 1;
      y = y - 1;
      moving_northeast = true;
      moving_northwest = false;
      moving_southwest = false;
      moving_southeast = false;
    }

    void move_southeast() {
      x = x + 1;
      y = y + 1;
      moving_northeast = false;
      moving_northwest = false;
      moving_southwest = false;
      moving_southeast = true;
    }

    void move_northwest() {
      x = x - 1;
      y = y - 1;
      moving_northeast = false;
      moving_northwest = true;
      moving_southwest = false;
      moving_southeast = false;
    }

    void move_southwest() {
      x = x - 1;
      y = y + 1;
      moving_northeast = false;
      moving_northwest = false;
      moving_southwest = true;
      moving_southeast = false;
    }

    void spawn (int index, int shape, int drone_strength) {
      if (index == 0) {
        origin = 0;
        switch (shape) {
          case 1:
            initialize(4, 3, drone_strength);
            break;
          case 2:
            initialize(4, 4, drone_strength);
            break;
          case 3:
            initialize(4, 5, drone_strength);
            break;
        }
      }
      if (index == 1) {
        origin = 1;
        switch (shape) {
          case 1:
            initialize(15, 3, drone_strength);
            break;
          case 2:
            initialize(15, 4, drone_strength);
            break;
          case 3:
            initialize(15, 5, drone_strength);
            break;
        }
      }
      if (index == 2) {
        origin = 2;
        switch (shape) {
          case 1:
            initialize(27, 3, drone_strength);
            break;
          case 2:
            initialize(27, 4, drone_strength);
            break;
          case 3:
            initialize(27, 5, drone_strength);
            break;
        }
      }
    }

    // draws the Drone if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      switch (strength) {
        case 1:
          draw_with_rgb(AQUA, RED);
          break;
        case 2:
          draw_with_rgb(RED, YELLOW);
          break;

      }
    }

    // draws black where the Drone used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }

    // Drone is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      strength = strength - 1;
    }

  private:
    int x;
    int y;
    int strength;
    int origin;
    bool moving_northeast = false;
    bool moving_northwest = false;
    bool moving_southwest = false;
    bool moving_southeast = false;
    // draws the Drone
    void draw_with_rgb(Color body_color, Color eye_color) {
      matrix.drawPixel(x, y, eye_color.to_333());
      matrix.drawPixel(x, y - 1, body_color.to_333());
      matrix.drawPixel(x, y + 1, body_color.to_333());
      matrix.drawPixel(x + 1, y, body_color.to_333());
      matrix.drawPixel(x - 1, y, body_color.to_333());
    }
};

class Cannonball {
  public:
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
    }

    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }

    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    bool has_been_fired() const {
      return fired;
    }

    // sets private data members
    void fire(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      fired = true;
    }

    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
      if (fired) {
        y = y - 1;
        const int Y_LIMIT = 0;
        if (y < Y_LIMIT) {
          fired = false;
        }
        if (y <= -1) {
          erase();
          fired = false;
        }
      }
    }

    // resets private data members to initial values
    void hit() {
      fired = false;
      erase();
      x = 0;
      y = 0;
    }

    // draws the Cannonball, if it is fired
    void draw() {
      if (fired) {
        matrix.drawPixel(x, y, PURPLE.to_333());
        matrix.drawPixel(x, y + 1, PURPLE.to_333());
      }
    }

    // draws black where the Cannonball used to be
    void erase() {
      for (int i = 15; i >= y; i--) {
        matrix.drawPixel(x, i, BLACK.to_333());

      }
    }

  private:
    int x;
    int y;
    bool fired;
};

class Player {
  public:
    Player() {
      x = 15;
      y = 15;
      lives = 3;
    }

    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }

    // setter
    void set_x(int x_arg) {
      initialize(x_arg, Y_MAX);
    }

    // Modifies: lives
    void die() {
      lives = lives - 1;

    }

    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(WHITE);
    }

    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }

    // draws the player
    void draw_with_rgb(Color color) {
      matrix.drawPixel(x, y, color.to_333());
      matrix.drawPixel(x, y - 1, color.to_333());
      matrix.drawPixel(x + 1, y, color.to_333());
      matrix.drawPixel(x + 1, y - 1, color.to_333());
      matrix.drawPixel(x - 1, y, color.to_333());
      matrix.drawPixel(x - 1, y - 1, color.to_333());
      matrix.drawPixel(x, y - 2, color.to_333());
    }
};



class Hive {
  private:
    int x;
    int y;
    int strength;
    int shape;

    void draw_with_rgb(Color body_color) {
      switch (shape) {
        case 1:
          matrix.drawPixel(x, y, body_color.to_333());
          matrix.drawPixel(x - 1, y, body_color.to_333());
          matrix.drawPixel(x + 1, y, body_color.to_333());
          matrix.drawPixel(x + 2, y, body_color.to_333());
          matrix.drawPixel(x, y + 1, body_color.to_333());
          matrix.drawPixel(x + 1, y + 1, body_color.to_333());
          break;

        case 2:
          matrix.drawPixel(x, y, body_color.to_333());
          matrix.drawPixel(x - 1, y, body_color.to_333());
          matrix.drawPixel(x - 2, y, body_color.to_333());
          matrix.drawPixel(x + 1, y, body_color.to_333());
          matrix.drawPixel(x + 2, y, body_color.to_333());
          matrix.drawPixel(x + 3, y, body_color.to_333());
          matrix.drawPixel(x, y + 1, body_color.to_333());
          matrix.drawPixel(x - 1, y + 1, body_color.to_333());
          matrix.drawPixel(x + 1, y + 1, body_color.to_333());
          matrix.drawPixel(x + 2, y + 1, body_color.to_333());
          matrix.drawPixel(x, y + 2, body_color.to_333());
          matrix.drawPixel(x + 1, y + 2, body_color.to_333());

          break;

        case 3:
          matrix.drawPixel(x, y, body_color.to_333());
          matrix.drawPixel(x - 1, y, body_color.to_333());
          matrix.drawPixel(x - 2, y, body_color.to_333());
          matrix.drawPixel(x - 3, y, body_color.to_333());
          matrix.drawPixel(x + 1, y, body_color.to_333());
          matrix.drawPixel(x + 2, y, body_color.to_333());
          matrix.drawPixel(x + 3, y, body_color.to_333());
          matrix.drawPixel(x + 4, y, body_color.to_333());
          matrix.drawPixel(x, y + 1, body_color.to_333());
          matrix.drawPixel(x - 1, y + 1, body_color.to_333());
          matrix.drawPixel(x - 2, y + 1, body_color.to_333());
          matrix.drawPixel(x + 1, y + 1, body_color.to_333());
          matrix.drawPixel(x + 2, y + 1, body_color.to_333());
          matrix.drawPixel(x + 3, y + 1, body_color.to_333());
          matrix.drawPixel(x - 1, y + 2, body_color.to_333());
          matrix.drawPixel(x, y + 2, body_color.to_333());
          matrix.drawPixel(x + 1, y + 2, body_color.to_333());
          matrix.drawPixel(x + 2, y + 2, body_color.to_333());
          matrix.drawPixel(x, y + 3, body_color.to_333());
          matrix.drawPixel(x + 1, y + 3, body_color.to_333());
          break;
      }

    }

  public:

    // default constructor
    Hive() {
      x = 15;
      y = 0;
      strength = 10;
    }

    // non-default constructor
    Hive(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }

    // setters
    void set_x(int x_arg) {
      x = x_arg;
    }

    void set_y(int y_arg) {
      y = y_arg;
    }

    void set_strength(int strength_arg) {
      strength = strength_arg;
    }

    void initialize(int x_arg, int strength_arg) {
      x = x_arg;
      strength = strength_arg;
    }

    // getters
    int get_x() {
      return x;
    }
    int get_y() {
      return y;
    }
    int get_strength() {
      return strength;
    }

    int get_shape() {
      return shape;
    }

    void decrement_hive_str() {
      strength = strength - 1;
      if (strength == 0) {
        erase();
      }
    }

    // draw function
    void draw() {
      if (strength >= 1 && strength <= 4) {
        shape = 1;
        draw_with_rgb(RED);
      }
      else if (strength > 4 && strength <= 8) {
        shape = 2;
        draw_with_rgb(BLUE);
      }
      else if (strength > 8) {
        shape = 3;
        draw_with_rgb(LIME);
      }
    }

    // erase function
    void erase() {
      switch (shape) {
        case 1:
          matrix.drawPixel(x, y, BLACK.to_333());
          matrix.drawPixel(x - 1, y, BLACK.to_333());
          matrix.drawPixel(x + 1, y, BLACK.to_333());
          matrix.drawPixel(x + 2, y, BLACK.to_333());
          matrix.drawPixel(x, y + 1, BLACK.to_333());
          matrix.drawPixel(x + 1, y + 1, BLACK.to_333());
          break;
        case 2:
          matrix.drawPixel(x, y, BLACK.to_333());
          matrix.drawPixel(x - 1, y, BLACK.to_333());
          matrix.drawPixel(x - 2, y, BLACK.to_333());
          matrix.drawPixel(x + 1, y, BLACK.to_333());
          matrix.drawPixel(x + 2, y, BLACK.to_333());
          matrix.drawPixel(x + 3, y, BLACK.to_333());
          matrix.drawPixel(x, y + 1, BLACK.to_333());
          matrix.drawPixel(x - 1, y + 1, BLACK.to_333());
          matrix.drawPixel(x + 1, y + 1, BLACK.to_333());
          matrix.drawPixel(x + 2, y + 1, BLACK.to_333());
          matrix.drawPixel(x, y + 2, BLACK.to_333());
          matrix.drawPixel(x + 1, y + 2, BLACK.to_333());

          break;
        case 3:
          matrix.drawPixel(x, y, BLACK.to_333());
          matrix.drawPixel(x - 1, y, BLACK.to_333());
          matrix.drawPixel(x - 2, y, BLACK.to_333());
          matrix.drawPixel(x - 3, y, BLACK.to_333());
          matrix.drawPixel(x + 1, y, BLACK.to_333());
          matrix.drawPixel(x + 2, y, BLACK.to_333());
          matrix.drawPixel(x + 3, y, BLACK.to_333());
          matrix.drawPixel(x + 4, y, BLACK.to_333());
          matrix.drawPixel(x, y + 1, BLACK.to_333());
          matrix.drawPixel(x - 1, y + 1, BLACK.to_333());
          matrix.drawPixel(x - 2, y + 1, BLACK.to_333());
          matrix.drawPixel(x + 1, y + 1, BLACK.to_333());
          matrix.drawPixel(x + 2, y + 1, BLACK.to_333());
          matrix.drawPixel(x + 3, y + 1, BLACK.to_333());
          matrix.drawPixel(x - 1, y + 2, BLACK.to_333());
          matrix.drawPixel(x, y + 2, BLACK.to_333());
          matrix.drawPixel(x + 1, y + 2, BLACK.to_333());
          matrix.drawPixel(x + 2, y + 2, BLACK.to_333());
          matrix.drawPixel(x, y + 3, BLACK.to_333());
          matrix.drawPixel(x + 1, y + 3, BLACK.to_333());
          break;
      }
    }

};

class Game {
  private:
    int level = 1;
    int index;
    Player player;
    Cannonball ball;
    int drones_in_lvl;
    Hive hives[MAX_HIVES];
    Drone drones[MAX_DRONES];
    bool player_is_dead = false;
    int count = 0;
    bool moved_this_count;


    // Player movement
    void player_movement(int count, int potentiometer_value) {
      if (count % 100 == 0) {
        //erases current player
        player.erase();
        //checks if potentiometer_value is a lot smaller than corresponding current x and sets new x for player
        if ((player.get_x() * 32 - 32) >= potentiometer_value) {
          player.set_x(player.get_x() - 1);
        }
        //checks if potentiometer_value is a lot bigger than corresponding current x and sets new x for player
        if (((player.get_x() * 32) + 48) < potentiometer_value) {
          player.set_x(player.get_x() + 1);
        }
        //draws new moved player
        player.draw();

      }
    }

    // Ball movement
    void ball_movement(int count) {
      if (count % 300 == 0 && (ball.has_been_fired() || ball.get_y() == 0)) {
        ball.move();
        ball.draw();
      }
    }

    // Fire the Ball
    void fire_ball(bool button_pressed) {
      if (!ball.has_been_fired() && button_pressed) {
        ball.fire(player.get_x(), 15);

      }
    }

    void drone_movement() {
      if (count % 1200 == 0) {
        for (int i = 0; i < drones_in_lvl; ++i) {
          if (drones[i].get_strength() > 0) {
            // other bottom right pseudo-corner
            if (drones[i].get_x() + 1 == 31 && drones[i].get_y() + 1 == 14) {
              drones[i].erase();
              drones[i].move_northwest();
              drones[i].draw();
            }
            // other bottom left pseudo-corner
            if (drones[i].get_x() - 1 == 1 && drones[i].get_y() + 1 == 15) {
              drones[i].erase();
              drones[i].move_northeast();
              drones[i].draw();
            }
            // other top left pseudo-corner
            if (drones[i].get_x() - 1 == 1 && drones[i].get_y() - 1 == 5) {
              drones[i].erase();
              drones[i].move_southeast();
              drones[i].draw();
            }
            // other top right pseudo-corner
            if (drones[i].get_x() + 1 == 30 && drones[i].get_y() - 1 == 5) {
              drones[i].erase();
              drones[i].move_southwest();
              drones[i].draw();
            }
            // bottom right pseudo-corner
            if (drones[i].get_x() + 1 == 30 && drones[i].get_y() + 1 == 15) {
              drones[i].erase();
              drones[i].move_northwest();
              drones[i].draw();
            }
            // bottom left psuedo-corner
            if (drones[i].get_x() - 1 == 0 && drones[i].get_y() + 1 == 14) {
              drones[i].erase();
              drones[i].move_northeast();
              drones[i].draw();
            }
            // top left pseudo-corner
            if (drones[i].get_x() - 1 == 0 && drones[i].get_y() - 1 == 6) {
              drones[i].erase();
              drones[i].move_southeast();
              drones[i].draw();
            }
            // top right pseudo-corner
            if (drones[i].get_x() + 1 == 31 && drones[i].get_y() - 1 == 6) {
              drones[i].erase();
              drones[i].move_southwest();
              drones[i].draw();
            }
            // bottom right real-corner
            if ((drones[i].get_y() + 1 == 15) && (drones[i].get_x() + 1 == 31)) {
              drones[i].erase();
              drones[i].move_northwest();
              drones[i].draw();
            }
            // bottom left real-corner
            if ((drones[i].get_y() + 1 == 15) && (drones[i].get_x() - 1 == 0)) {
              drones[i].erase();
              drones[i].move_northeast();
              drones[i].draw();
            }
            // top left real-corner
            if ((drones[i].get_y() - 1 == 5) && (drones[i].get_x() - 1 == 0)) {
              drones[i].erase();
              drones[i].move_southeast();
              drones[i].draw();
            }
            // top right real-corner
            if ((drones[i].get_y() - 1 == 5) && (drones[i].get_x() + 1 == 31)) {
              drones[i].erase();
              drones[i].move_southwest();
              drones[i].draw();
            }
            // if it hits left wall bounces off
            if (drones[i].get_x() - 1 == 0) {
              if (drones[i].get_moving_northwest()) {
                drones[i].erase();
                drones[i].move_northeast();
                drones[i].draw();
              }
              if (drones[i].get_moving_southwest()) {
                drones[i].erase();
                drones[i].move_southeast();
                drones[i].draw();
              }
            }

            // if it hits right wall, bounces off
            if (drones[i].get_x() + 1 == 31) {
              if (drones[i].get_moving_northeast()) {
                drones[i].erase();
                drones[i].move_northwest();
                drones[i].draw();
              }
              if (drones[i].get_moving_southeast()) {
                drones[i].erase();
                drones[i].move_southwest();
                drones[i].draw();
              }
            }
            // if it hits the floor, bounces off
            if (drones[i].get_y() + 1 == 15) {
              if (drones[i].get_moving_southwest()) {
                drones[i].erase();
                drones[i].move_northwest();
                drones[i].draw();
              }
              if (drones[i].get_moving_southeast()) {
                drones[i].erase();
                drones[i].move_northeast();
                drones[i].draw();
              }
            }

            // If at top bound, bounce off
            if (drones[i].get_y() - 1 == 5) {
              if (drones[i].get_moving_northeast()) {
                drones[i].erase();
                drones[i].move_southeast();
                drones[i].draw();
              }
              if (drones[i].get_moving_northwest()) {
                drones[i].erase();
                drones[i].move_southwest();
                drones[i].draw();
              }
            }
            if (drones[i].get_moving_northeast()) {
              drones[i].erase();
              drones[i].move_northeast();
              drones[i].draw();
            }
            if (drones[i].get_moving_northwest()) {
              drones[i].erase();
              drones[i].move_northwest();
              drones[i].draw();
            }
            if (drones[i].get_moving_southeast()) {
              drones[i].erase();
              drones[i].move_southeast();
              drones[i].draw();
            }
            if (drones[i].get_moving_southwest()) {
              drones[i].erase();
              drones[i].move_southwest();
              drones[i].draw();
            }
            if (drones[i].get_y() - 1 < 5) {
              int initial_direction = rand() % 2;
              if (initial_direction == 0) {
                drones[i].erase();
                drones[i].move_southwest();
                drones[i].draw();
              }
              if (initial_direction == 1) {
                drones[i].erase();
                drones[i].move_southeast();
                drones[i].draw();
              }
            }
          }
        }
      }
    }


    void drone_hit_reg() {
      if (ball.has_been_fired()) {

        for (int i = 0; i < drones_in_lvl; ++i) {
          if (drones[i].get_strength() > 0) {
 
		  for (int k = 15; k >= ball.get_y(); --k) {
              if (drones[i].get_y() == k) {
                if (drones[i].get_x() - 1 == ball.get_x()
                    || drones[i].get_x() == ball.get_x()
                    || drones[i].get_x() + 1 == ball.get_x()) {
                  drones[i].drone_hit();
                  ball.hit();
                  ball.erase();
                  drones[i].erase();
                  drones[i].draw();
                  if (drones[i].get_strength() == 0) {
                    drones[i].erase();
                  }
                  hives[0].draw();
                  hives[1].draw();
                  hives[2].draw();
                }
              }
              if (drones[i].get_y() + 1 == k) {
                if (drones[i].get_x() == ball.get_x()) {
                  drones[i].drone_hit();
                  ball.hit();
                  ball.erase();
                  drones[i].erase();
                  drones[i].draw();
                  if (drones[i].get_strength() == 0) {
                    drones[i].erase();
                  }
                  hives[0].draw();
                  hives[1].draw();
                  hives[2].draw();
                }
              }
            }
          }
        }
      }
    }

    void player_hit() {
      for (int i = 0; i < drones_in_lvl; i++) {
        if (drones[i].get_strength() > 0) {
          if (player.get_x() == drones[i].get_x()) {
            if (player.get_y() == drones[i].get_y() ||
                player.get_y() - 1 == drones[i].get_y() + 1 ||
                player.get_y() - 2 == drones[i].get_y() + 1) {
              player_is_dead = true;
            }
          }
          if (player.get_x() - 1 == drones[i].get_x()) {
            if (player.get_y() - 1 == drones[i].get_y() + 1 ||
                player.get_y() == drones[i].get_y() + 1) {
              player_is_dead = true;
            }
          }
          if (player.get_x() + 1 == drones[i].get_x()) {
            if (player.get_y() - 1 == drones[i].get_y() + 1 ||
                player.get_y() == drones[i].get_y() + 1) {
              player_is_dead = true;
            }
          }
          if (player.get_x() + 1 == drones[i].get_x() - 1) {
            if (player.get_y() - 1 == drones[i].get_y()) {
              player_is_dead = true;
            }
          }
          if (player.get_x() - 1 == drones[i].get_x() + 1) {
            if (player.get_y() - 1 == drones[i].get_y()) {
              player_is_dead = true;
            }
          }
        }
      }
    }

    bool level_cleared() {
      if (hives[0].get_strength() == 0 &&
          hives[1].get_strength() == 0 &&
          hives[2].get_strength() == 0) {
        return true;
      }
      return false;
    }


    // checks if the hive was hit, decrements str
    void check_shape_hit(int shape, int index) {
      if (shape == 1 && hives[index].get_strength() != 0) {
        if (ball.get_y() == hives[index].get_y()) {
          if (ball.get_x() == (hives[index].get_x() - 1) || ball.get_x() == (hives[index].get_x() + 2)) {
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
        if (ball.get_y() == hives[index].get_y() + 1) {
          if (ball.get_x() == (hives[index].get_x()) || ball.get_x() == (hives[index].get_x() + 1)) {
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            Serial.println("It worked");
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }


      }

      if (shape == 2 && hives[index].get_strength() != 0) {
        if (ball.get_y() == hives[index].get_y()) {
          if (ball.get_x() == (hives[index].get_x() - 2) || ball.get_x() == (hives[index].get_x() + 3)) {
            hives[index].erase();
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
        if (ball.get_y() == (hives[index].get_y() + 1)) {
          if (ball.get_x() == (hives[index].get_x() - 1) || ball.get_x() == (hives[index].get_x() + 2)) {
            hives[index].erase();
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
        if (ball.get_y() == (hives[index].get_y() + 2)) {
          if (ball.get_x() == (hives[index].get_x()) || ball.get_x() == (hives[index].get_x() + 1)) {
            hives[index].erase();
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
      }

      if (shape == 3 && hives[index].get_strength() != 0) {
        if (ball.get_y() == hives[index].get_y()) {
          if (ball.get_x() == (hives[index].get_x() - 3) || ball.get_x() == (hives[index].get_x() + 4)) {
            hives[index].erase();
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
        if (ball.get_y() == (hives[index].get_y() + 1)) {
          if (ball.get_x() == (hives[index].get_x() - 2) || ball.get_x() == (hives[index].get_x() + 3)) {
            hives[index].erase();
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
        if (ball.get_y() == (hives[index].get_y() + 2)) {
          if (ball.get_x() == (hives[index].get_x() - 1) || ball.get_x() == (hives[index].get_x() + 2)) {
            hives[index].erase();
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
        if (ball.get_y() == (hives[index].get_y() + 3)) {
          if (ball.get_x() == (hives[index].get_x()) || ball.get_x() == (hives[index].get_x() + 1)) {
            hives[index].erase();
            hives[index].decrement_hive_str();
            ball.erase();
            ball.hit();
            if (hives[index].get_strength() != 0) {
              hives[index].draw();
            }
          }
        }
      }

    }

    // Hive hit reg fnc
    void hive_hit_reg() {
      if (ball.get_y() <= 3) {
        if (ball.get_x() >= 0 && ball.get_x() <= 10) {
          index = 0;
          check_shape_hit(hives[index].get_shape(), index);
        }


        else if (ball.get_x() >= 11 && ball.get_x() <= 21) {
          index = 1;
          check_shape_hit(hives[index].get_shape(), index);
        }

        else if (ball.get_x() >= 22 && ball.get_x() <= 31) {
          index = 2;
          check_shape_hit(hives[index].get_shape(), index);
        }
      }
    }

    // Spawns drones on the board
    void drone_spawn () {
      if ((count + 1) % 10000 == 0) {
        for (int i = 0; i < drones_in_lvl; i++) {
          if (drones[i].get_strength() == 0) {
            if (hives[0].get_strength() != 0 && hives[1].get_strength() != 0 && hives[2].get_strength() != 0) {
              int hive_current = rand() % 3;
              drones[i].spawn(hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
              drones[i].draw();
            }
            else if (hives[0].get_strength() != 0 && hives[1].get_strength() != 0) {
              int hive_current = rand() % 2;
              drones[i].spawn(hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
              drones[i].draw();
            }
            else if (hives[1].get_strength() != 0 && hives[2].get_strength() != 0) {
              int hive_current = (rand() % 2) + 1;
              drones[i].spawn(hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
              drones[i].draw();
            }
            else if (hives[0].get_strength() != 0 && hives[2].get_strength() != 0) {
              int hive_current = rand() % 2;
              if (hive_current == 1) {
                hive_current++;
              }
              drones[i].spawn (hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
              drones[i].draw();
            }
            else if (hives[0].get_strength() != 0) {
              drones[i].spawn (0, hives[0].get_shape(), (rand() % 2) + 1);
              drones[i].draw();
            }
            else if (hives[1].get_strength() != 0) {
              drones[i].spawn (1, hives[1].get_shape(), (rand() % 2) + 1);
              drones[i].draw();
            }
            else if (hives[2].get_strength() != 0) {
              drones[i].spawn(2, hives[2].get_shape(), (rand() % 2) + 1);
              drones[i].draw();
            }
          }
        }
        if (drones_in_lvl < 3) {
          if (hives[0].get_strength() != 0 && hives[1].get_strength() != 0 && hives[2].get_strength() != 0) {
            int hive_current = rand() % 3;
            drones[drones_in_lvl].spawn(hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
            drones[drones_in_lvl].draw();
            drones_in_lvl++;
          }
          else if (hives[0].get_strength() != 0 && hives[1].get_strength() != 0) {
            int hive_current = rand() % 2;
            drones[drones_in_lvl].spawn(hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
            drones[drones_in_lvl].draw();
            drones_in_lvl++;
          }
          else if (hives[1].get_strength() != 0 && hives[2].get_strength() != 0) {
            int hive_current = (rand() % 2) + 1;
            drones[drones_in_lvl].spawn(hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
            drones[drones_in_lvl].draw();
            drones_in_lvl++;
          }
          else if (hives[0].get_strength() != 0 && hives[2].get_strength() != 0) {
            int hive_current = rand() % 2;
            if (hive_current == 1) {
              hive_current++;
            }
            drones[drones_in_lvl].spawn (hive_current, hives[hive_current].get_shape(), (rand() % 2) + 1);
            drones[drones_in_lvl].draw();
            drones_in_lvl++;
          }
          else if (hives[0].get_strength() != 0) {
            drones[drones_in_lvl].spawn (0, hives[0].get_shape(), (rand() % 2) + 1);
            drones[drones_in_lvl].draw();
            drones_in_lvl++;
          }
          else if (hives[1].get_strength() != 0) {
            drones[drones_in_lvl].spawn (1, hives[1].get_shape(), (rand() % 2) + 1);
            drones[drones_in_lvl].draw();
            drones_in_lvl++;
          }
          else if (hives[2].get_strength() != 0) {
            drones[drones_in_lvl].spawn(2, hives[2].get_shape(), (rand() % 2) + 1);
            drones[drones_in_lvl].draw();
            drones_in_lvl++;
          }
        }
      }
    }

    void set_up_lvl(int level) {
      matrix.fillScreen(BLACK.to_333());
      drones_in_lvl = 0;
      if (level == 1) {
        // prints beginning
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        print_level(level);
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        // initialzes and prints hives
        hives[0].initialize(4, 4);
        hives[0].draw();

        hives[1].initialize(15, 4);
        hives[1].draw();

        hives[2].initialize(26, 4);
        hives[2].draw();


        player.draw();

      }
      else if (level == 2) {

        // prints beginning
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        print_level(level);
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        // initialzes and prints hives
        hives[0].initialize(4, 4);
        hives[0].draw();

        hives[1].initialize(15, 8);
        hives[1].draw();

        hives[2].initialize(26, 4);
        hives[2].draw();
        player.draw();

      }
      else if (level == 3) {
        // prints beginning
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        print_level(level);
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        // initialzes and prints hives
        hives[0].initialize(4, 8);
        hives[0].draw();

        hives[1].initialize(15, 8);
        hives[1].draw();

        hives[2].initialize(26, 8);
        hives[2].draw();
        player.draw();

      }
      else if (level == 4) {
        // prints beginning
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        print_level(level);
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        // initialzes and prints hives
        hives[0].initialize(4, 8);
        hives[0].draw();

        hives[1].initialize(15, 12);
        hives[1].draw();

        hives[2].initialize(26, 8);
        hives[2].draw();

        player.draw();

      }
      else if (level == 5) {
        // prints beginning
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        print_level(level);
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        // initialzes and prints hives
        hives[0].initialize(4, 12);
        hives[0].draw();

        hives[1].initialize(15, 12);
        hives[1].draw();

        hives[2].initialize(26, 12);
        hives[2].draw();

        player.draw();

      }
      else {
        // prints beginning
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        print_level(level);
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        // initialzes and prints hives
        hives[0].initialize(4, (rand() % 10) + 9);
        hives[0].draw();

        hives[1].initialize(15, (rand() % 10) + 9);
        hives[1].draw();

        hives[2].initialize(26, (rand() % 10) + 9);
        hives[2].draw();
        player.draw();

      }
    }

  public:
    Game() {
      level = 1;
      drones_in_lvl = 0;
      int count = 0;
    }

    int get_count() {
      return count;
    }
    void setup() {
      set_up_lvl(level);
      Player player;
      player.draw();

    }

    void set_drones_in_lvl(int arg_drones) {
      drones_in_lvl = arg_drones;
    }

    int get_drones_in_lvl() {
      return drones_in_lvl;
    }

    void loop(int potentiometer_value, bool button_pressed) {

      player_movement(count, potentiometer_value);
      fire_ball(button_pressed);
      ball_movement(count);
      hive_hit_reg();
      drone_spawn();
      drone_movement();
      drone_hit_reg();
      player_hit();
      if (level_cleared()) {
        level++;
        player.erase();
        set_up_lvl(level);
      }
      if (player_is_dead) {
        ball.reset();
        ball.erase();
        matrix.fillScreen(BLACK.to_333());
        player.die();
        if (player.get_lives() == 0) {
          game_over();
          delay(7000000000);
        }
        player_is_dead = false;
        set_up_lvl(level);
      }
      count++;
    }

};

Game game;
// see http://arduino.cc/en/Reference/Setup
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.setup();

}

// see http://arduino.cc/en/Reference/Loop
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.loop(potentiometer_value, button_pressed);
}

// displays Level
void print_level(int level) {
  print_word_level();

  matrix.setCursor(13, 9);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.25);
  matrix.print((level));



}

// displays number of lives
void print_lives(int lives) {
  if (lives == 3) {
    // print 3
    print_word_lives();

    matrix.setCursor(10, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(1);
    matrix.print('x');

    matrix.setCursor(16, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(.25);
    matrix.print('3');

  }
  else if (lives == 2) {

    // print 2
    print_word_lives();

    matrix.setCursor(10, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(1);
    matrix.print('x');

    matrix.setCursor(16, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(.25);
    matrix.print('2');

  }
  else if (lives == 1) {

    // print 1
    print_word_lives();

    matrix.setCursor(10, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(1);
    matrix.print('x');

    matrix.setCursor(16, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(.25);
    matrix.print('1');

  }
  else {

    print_word_lives();

    matrix.setCursor(10, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(1);
    matrix.print('x');

    matrix.setCursor(16, 9);
    matrix.setTextColor(BLUE.to_333());
    matrix.setTextSize(.25);
    matrix.print('0');
  }
}


// displays "game over"
void game_over() {
  matrix.setCursor(4, 1);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('G');

  matrix.setCursor(10, 1);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('A');

  matrix.setCursor(16, 1);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('M');

  matrix.setCursor(22, 1);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('E');

  matrix.setCursor(4, 9);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('O');

  matrix.setCursor(10, 9);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('V');

  matrix.setCursor(16, 9);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('E');

  matrix.setCursor(22, 9);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('R');

  delay(3000);

}


// displays black screen
void black_screen() {
  matrix.fillScreen(BLACK.to_333());
}

void print_word_lives() {
  matrix.setCursor(2, 1);
  matrix.setTextColor(BLUE.to_333());
  matrix.setTextSize(.6);
  matrix.print('L');

  matrix.setCursor(7, 1);
  matrix.setTextColor(BLUE.to_333());
  matrix.setTextSize(.6);
  matrix.print('I');

  matrix.setCursor(12, 1);
  matrix.setTextColor(BLUE.to_333());
  matrix.setTextSize(.6);
  matrix.print('V');

  matrix.setCursor(18, 1);
  matrix.setTextColor(BLUE.to_333());
  matrix.setTextSize(.6);
  matrix.print('E');

  matrix.setCursor(24, 1);
  matrix.setTextColor(BLUE.to_333());
  matrix.setTextSize(.6);
  matrix.print('S');

  matrix.setCursor(28, 1);
  matrix.setTextColor(BLUE.to_333());
  matrix.setTextSize(1);
  matrix.print(':');

}

void print_word_level() {
  matrix.setCursor(3, 1);
  matrix.setTextColor(RED.to_333());
  matrix.setTextSize(.6);
  matrix.print('L');

  matrix.setCursor(8, 1);
  matrix.setTextColor(BLUE.to_333());
  matrix.setTextSize(.6);
  matrix.print('E');

  matrix.setCursor(13, 1);
  matrix.setTextColor(GREEN.to_333());
  matrix.setTextSize(.6);
  matrix.print('V');

  matrix.setCursor(19, 1);
  matrix.setTextColor(YELLOW.to_333());
  matrix.setTextSize(.6);
  matrix.print('E');

  matrix.setCursor(25, 1);
  matrix.setTextColor(AQUA.to_333());
  matrix.setTextSize(.6);
  matrix.print('L');

}







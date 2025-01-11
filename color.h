#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

using color = vec3;

void write_color(std::ostream& out, color pixel_color) {
  // Write the translated [0,255] value of each color component.
  out << static_cast<int>(255.999 * pixel_color.x()) << ' '
    << static_cast<int>(255.999 * pixel_color.y()) << ' '
    << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

color red = color(1.0f, 0.0f, 0.0f);
color green = color(0.0f, 1.0f, 0.0f);
color lightgreen = color(0.23f, 0.95f, 0.14f);
color blu = color(0.0f, 0.0f, 1.0f);
color gray = color(0.5f, 0.5f, 0.5f);
color white = color(1.0f, 1.0f, 1.0f);
color black = color(0.0f, 0.0f, 0.0f);
color darkgray = color(0.25f, 0.25f, 0.25f);
color lightgray = color(0.75f, 0.75f, 0.75f);
color slategray = color(0.4f, 0.5f, 0.56f);
color magenta = color(1.0f, 0.0f, 1.0f);
color cyan = color(0.0f, 1.0f, 1.0f);
color brown = color(0.60f, 0.40f, 0.12f);
color orange = color(0.95f, 0.55f, 0.14f);
color yellow = color(1.0f, 1.0f, 0.0f);

#endif
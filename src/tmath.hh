#pragma once

struct TVec2 {
   double x, y;
};

struct TVec3 {
   double x, y, z;
};

struct TVec4 {
   union {
      struct {
         double x, y, z, t;
      };
      struct {
         double r, g, b, a;
      };
   };
};

struct TRectangle {
   double x, y, w, h;
};

struct TCircle {
   TVec2  centre;
   double radius;
};

template <typename T>
inline static T min(T a, T b) { return a < b ? a : b; }

template <typename T>
inline static T max(T a, T b) { return a > b ? a : b; }

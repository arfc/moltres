L = 1;
// Bottom left
Point(0) = {0, 0, 0, .05};
// Top left
Point(1) = {0, L, 0, .05};
// Bottom right
Point(2) = {.5, 0, 0, .05};
// Top right
Point(3) = {.5, L, 0, .05};
Point(4) = {0, .49, 0, .001};
Point(5) = {0, .5, 0, .001};
Point(6) = {0, .51, 0, .001};

// Bottom (left to right)
Line(1) = {0, 2};
// Right (bottom to top)
Line(2) = {2, 3};
// Top (right to left)
Line(3) = {3, 1};
// Sphere (bottom to top)
Circle(6) = {4, 5, 6};
// Left (bottom segment, bottom to top)
Line(4) = {0, 4};
// Left (top segment, bottom to top)
Line(5) = {6, 1};

Line Loop(0) = {1, 2, 3, -5, -6, -4};
Plane Surface(0) = {0};
Physical Surface(0) = {0};
Physical Line("left") = {4, 5};
Physical Line("bottom") = {1};
Physical Line("right") = {2};
Physical Line("top") = {3};
Physical Line("sphere") = {6};
Physical Point("bottom_left") = {0};
Physical Point("top_right") = {3};

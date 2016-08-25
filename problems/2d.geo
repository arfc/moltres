Point(0) = {0, 0, 0, .001};
Point(1) = {0, 1, 0, .01};
Point(2) = {.5, 0, 0, .1};
Point(3) = {.5, 1, 0, .1};

Line(4) = {0, 1};
Line(1) = {0, 2};
Line(2) = {2, 3};
Line(3) = {3, 1};

Line Loop(0) = {1, 2, 3, -4};
Plane Surface(0) = {0};
Physical Surface(0) = {0};
Physical Line("left") = {4};
Physical Line("bottom") = {1};
Physical Line("right") = {2};
Physical Line("top") = {3};

// Bottom left
Point(0) = {0, 0, 0, .05};
// Top left
Point(1) = {0, 1, 0, .05};
// Bottom right
Point(2) = {.5, 0, 0, .05};
// Top right
Point(3) = {1, 1, 0, .05};

Line(4) = {0, 1};
Line(1) = {0, 2};
Line(2) = {2, 3};
Line(3) = {3, 1};

Line Loop(0) = {1, 2, 3, -4};
Plane Surface(0) = {0};
Point(4) = {0, 4, 0, 1.0};
Point(5) = {1, 4, 0, 1.0};
Line(8) = {1, 4};
Line(9) = {4, 5};
Line(10) = {5, 3};
Line Loop(11) = {9, 10, 3, 8};
Plane Surface(12) = {11};
Physical Surface(0) = {0, 12};
Physical Point("outlet_center") = {4};
Physical Line("axis") = {8, 4};
Physical Line("outlet") = {9};
Physical Line("walls") = {10, 2};
Physical Line("inlet") = {1};

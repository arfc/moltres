h = 1;

Point(0) = {0, 0, 0, h};
Point(1) = {1, 0, 0, h};
Point(2) = {-0, 0.7, 0, 1.0};
Point(3) = {1, 0.7, 0, 1.0};
Line(1) = {2, 3};
Line(2) = {2, 0};
Line(3) = {0, 1};
Line(4) = {1, 3};
Line Loop(5) = {1, -4, -3, -2};
Plane Surface(6) = {5};

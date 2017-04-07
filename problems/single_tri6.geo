//+
Point(1) = {0, 0, 0, 10};
//+
Point(2) = {1, 0, 0, 10};
//+
Point(3) = {0, 1, 0, 10};
//+
Line(1) = {3, 1};
//+
Line(2) = {1, 2};
//+
Line(3) = {2, 3};
//+
Physical Line("boundary") = {1, 3, 2};
//+
Line Loop(4) = {3, 1, 2};
//+
Plane Surface(5) = {4};
Physical Surface("bobby") = {5};

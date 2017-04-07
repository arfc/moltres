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
Line Loop(4) = {3, 1, 2};
//+
Plane Surface(5) = {4};
//+
Point(4) = {0, 0, 1, 10};
//+
Line(6) = {4, 1};
//+
Line(7) = {4, 2};
//+
Line(8) = {4, 3};
//+
Line Loop(9) = {7, 3, -8};
//+
Plane Surface(10) = {9};
//+
Line Loop(11) = {2, -7, 6};
//+
Plane Surface(12) = {11};
//+
Line Loop(13) = {1, -6, 8};
//+
Plane Surface(14) = {13};
//+
Surface Loop(15) = {12, 5, 10, 14};
//+
Volume(16) = {15};
//+
Physical Surface("boundary") = {12, 5, 14, 10};
//+
Physical Volume("bobby") = {16};

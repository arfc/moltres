//+
Point(1) = {0, 0, 0, 5};
//+
Point(2) = {1, 0, 0, 5};
//+
Point(3) = {1.5, 0, 0, 5};
//+
Point(4) = {2, 0, 0, 5};
//+
Point(5) = {3, 0, 0, 5};
//+
Point(6) = {3, 1, 0, 5};
//+
Point(7) = {2, 1, 0, 5};
//+
Point(8) = {1.5, 1, 0, 5};
//+
Point(9) = {1, 1, 0, 5};
//+
Point(10) = {0, 1, 0, 5};
//+
Line(1) = {10, 1};
//+
Line(2) = {2, 1};
//+
Line(3) = {9, 2};
//+
Line(4) = {2, 3};
//+
Line(5) = {8, 3};
//+
Line(6) = {3, 4};
//+
Line(7) = {4, 7};
//+
Line(8) = {7, 6};
//+
Line(9) = {6, 5};
//+
Line(10) = {5, 4};
//+
Line(11) = {7, 8};
//+
Line(12) = {8, 9};
//+
Line(13) = {9, 10};
//+
Line Loop(14) = {13, 1, -2, -3};
//+
Plane Surface(15) = {14};
//+
Line Loop(16) = {12, 3, 4, -5};
//+
Plane Surface(17) = {16};
//+
Line Loop(18) = {11, 5, 6, 7};
//+
Plane Surface(19) = {18};
//+
Line Loop(20) = {8, 9, 10, 7};
//+
Plane Surface(21) = {20};
//+
Physical Surface("fuel") = {15, 21};
//+
Physical Surface("mod") = {17, 19};
//+
Physical Line("top") = {13, 12, 11, 8};
//+
Physical Line("bottom") = {2, 4, 6, 10};
//+
Physical Line("left") = {1};
//+
Physical Line("right") = {9};

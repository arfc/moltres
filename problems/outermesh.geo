//+
Point(1) = {5, -6, 0, 1.0};
//+
Point(2) = {5, 6, 0, 1.0};
//+
Point(3) = {-5, 6, 0, 1.0};
//+
Point(4) = {-5, -6, 0, 1.0};
//+
Point(5) = {4, -2, 0, 1.0};
//+
Point(6) = {4, 2, 0, 1.0};
//+
Point(7) = {-4, 2, 0, 1.0};
//+
Point(8) = {-4, -2, 0, 1.0};//+
Point(9) = {0, 2, 0, 1.0};
//+
Point(10) = {0, -2, 0, 1.0};
//+
Line(1) = {7, 8};
//+
Line(2) = {8, 10};
//+
Line(3) = {10, 5};
//+
Line(4) = {5, 6};
//+
Line(5) = {6, 9};
//+
Line(6) = {9, 7};
//+
Line(7) = {3, 4};
//+
Line(8) = {4, 1};
//+
Line(9) = {1, 2};
//+
Line(10) = {2, 3};
//+
Line Loop(11) = {7, 8, 9, 10};
//+
Line Loop(12) = {6, 1, 2, 3, 4, 5};
//+
Plane Surface(13) = {11, 12};
//+
Transfinite Line {6, -2, 3, -5} = 50 Using Progression 1.1;
//+
Transfinite Line {1, 4} = 10 Using Progression 1;
//+
Physical Line("left") = {7};
//+
Physical Line("bottom") = {8};
//+
Physical Line("right") = {9};
//+
Physical Line("top") = {10};
//+
Physical Line("bottom2") = {3, 2};
//+
Physical Line("right2") = {4};
//+
Physical Line("top2") = {5, 6};
//+
Physical Line("left2") = {1};
//+
Physical Surface("all") = {13};
//+
Physical Line("left_right") = {1, 4};
//+
Physical Line("top_bottom", 14) = {5, 6, 2, 3};
//+
Physical Line("inner_BC", 15) = {1, 2, 3, 4, 5, 6};

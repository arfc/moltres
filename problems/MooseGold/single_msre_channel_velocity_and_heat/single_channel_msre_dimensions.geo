Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
fuel_width = 3.048;
fuel_height = 1.016;
fuel_length = 162.56;
// fuel_length = 10;
lc = .1 * fuel_length;
extension = .762;

Point(1) = {0, 0, 0, lc};
Point(2) = {0, 0, fuel_length, lc};

// z-normal
Line(1) = {1, 2};

//+
Point(3) = {fuel_width, 0, 0, 1.0};
//+
Point(4) = {fuel_width, fuel_height, 0, 1.0};
//+
Point(5) = {0, fuel_height, 0, 1.0};
//+
Point(6) = {fuel_width, fuel_height, fuel_length, 1.0};
//+
Point(7) = {fuel_width, 0, fuel_length, 1.0};
//+
Point(8) = {0, fuel_height, fuel_length, 1.0};
//+ y-normal
Line(2) = {1, 5};
//+ x-normal
Line(3) = {5, 4};
//+ y-normal
Line(4) = {4, 3};
//+ z-normal
Line(5) = {3, 7};
//+ x-normal
Line(6) = {3, 1};
//+ z-normal
Line(7) = {4, 6};
//+ y-normal
Line(8) = {6, 7};
//+ x-normal
Line(9) = {7, 2};
//+ y-normal
Line(10) = {2, 8};
//+ x-normal
Line(11) = {8, 6};
//+ z-normal
Line(12) = {5, 8};

// Point(3) = {fuel_width / 2., fuel_height / 2., 0, lc};
//+
Line Loop(13) = {5, 9, -1, -6};
//+
Plane Surface(14) = {13};
//+
Line Loop(15) = {2, 12, -10, -1};
//+
Plane Surface(16) = {15};
//+
Line Loop(17) = {11, -7, -3, 12};
//+
Plane Surface(18) = {17};
//+
Line Loop(19) = {7, 8, -5, -4};
//+
Plane Surface(20) = {19};
//+
Line Loop(21) = {8, 9, 10, 11};
//+
Plane Surface(22) = {21};
//+
Line Loop(23) = {3, 4, 6, 2};
//+
Plane Surface(24) = {23};
//+
Surface Loop(25) = {18, 22, 20, 14, 16, 24};
//+
Volume(26) = {25};

//+
Physical Volume("fuel") = {26};
//+
Point(9) = {0, fuel_height+extension, 0, 1.0};
//+
Point(10) = {fuel_width, fuel_height+extension, 0, 1.0};
//+
Point(11) = {fuel_width, fuel_height+extension, fuel_length, 1.0};
//+
Point(12) = {0, fuel_height+extension, fuel_length, 1.0};
//+
Point(13) = {0, -extension, fuel_length, 1.0};
//+
Point(14) = {fuel_width, -extension, fuel_length, 1.0};
//+
Point(15) = {fuel_width, -extension, 0, 1.0};
//+
Point(16) = {0, -extension, 0, 1.0};
//+ y-normal
Line(27) = {1, 16};
//+ z-normal
Line(28) = {16, 13};
//+ y-normal
Line(29) = {13, 2};
//+ x-normal
Line(30) = {13, 14};
//+ y-normal
Line(31) = {14, 7};
//+ z-normal
Line(32) = {14, 15};
//+ y-normal
Line(33) = {3, 15};
//+ x-normal
Line(34) = {15, 16};
//+ y-normal
Line(35) = {4, 10};
//+ x-normal
Line(36) = {10, 9};
//+ z-normal
Line(37) = {9, 12};
//+ y-normal
Line(38) = {12, 8};
//+ x-normal
Line(39) = {12, 11};
//+ z-normal
Line(40) = {11, 10};
//+ y-normal
Line(41) = {6, 11};
//+ y-normal
Line(42) = {5, 9};

Transfinite Line{1, 5, 7, 12, 28, 32, 37, 40} = 31 Using Progression 1.3;
Transfinite Line{2, 3, 4, 6, 8, 9, 10, 11, 30, 34, 36, 39, 27, 29, 31, 33, 35, 38, 41, 42} = 13 Using Bump 0.05;

//+
Line Loop(43) = {30, 32, 34, 28};
//+
Plane Surface(44) = {43};
//+
Line Loop(45) = {31, -5, 33, -32};
//+
Plane Surface(46) = {45};
//+
Line Loop(47) = {28, 29, -1, 27};
//+
Plane Surface(48) = {47};
//+
Line Loop(49) = {34, -27, -6, 33};
//+
Plane Surface(50) = {49};
//+
Line Loop(51) = {29, -9, -31, -30};
//+
Plane Surface(52) = {51};
//+
Line Loop(53) = {40, 36, 37, 39};
//+
Plane Surface(54) = {53};
//+
Line Loop(55) = {35, -40, -41, -7};
//+
Plane Surface(56) = {55};
//+
Line Loop(57) = {38, -12, 42, 37};
//+
Plane Surface(58) = {57};
//+
Line Loop(59) = {39, -41, -11, -38};
//+
Plane Surface(60) = {59};
//+
Line Loop(61) = {3, 35, 36, -42};
//+
Plane Surface(62) = {61};
//+
Surface Loop(63) = {44, 52, 48, 50, 46, 14};
//+
Volume(64) = {63};
//+
Surface Loop(65) = {58, 60, 54, 56, 62, 18};
//+
Volume(66) = {65};

Transfinite Surface{14, 16, 18, 20, 22, 24, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62};
Recombine Surface{14, 16, 18, 20, 22, 24, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62};
Transfinite Volume{26, 64, 66};
Recombine Volume{26, 64, 66};
//+
Physical Volume("moderator") = {66, 64};
//+
Physical Surface("fuel_top") = {22};
//+
Physical Surface("moderator_boundaries") = {46, 48, 52, 44, 50, 60, 62, 56, 54, 58};
//+
Physical Surface("fuel_mod_interface") = {18, 14};
//+
Physical Surface("fuel_side_walls") = {20, 16};
//+
Physical Surface("fuel_bottom") = {24};

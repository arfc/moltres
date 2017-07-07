Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
fuel_width = 3.048;
fuel_height = 1.016;
fuel_length = 162.56;
lc = .1 * fuel_length;

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
//+
Line(2) = {1, 5};
//+
Line(3) = {5, 4};
//+
Line(4) = {4, 3};
//+ z-normal
Line(5) = {3, 7};
//+
Line(6) = {3, 1};
//+ z-normal
Line(7) = {4, 6};
//+
Line(8) = {6, 7};
//+
Line(9) = {7, 2};
//+
Line(10) = {2, 8};
//+
Line(11) = {8, 6};
//+ z-normal
Line(12) = {5, 8};

Transfinite Line{1, 5, 7, 12} = 31 Using Progression 1.3;
Transfinite Line{2, 3, 4, 6, 8, 9, 10, 11} = 13 Using Bump 0.05;


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

Transfinite Surface{14, 16, 18, 20, 22, 24};
Recombine Surface{14, 16, 18, 20, 22, 24};
Transfinite Volume{26};
Recombine Volume{26};
//+
Physical Surface("left") = {16};
//+
Physical Surface("right") = {20};
//+
Physical Surface("top") = {18};
//+
Physical Surface("bottom") = {14};
//+
Physical Surface("back") = {24};
//+
Physical Surface("front") = {22};
//+
Physical Volume(0) = {26};
//+
Rotate {{0, 0, 1}, {0, 0, 0}, 3 * Pi / 2} {
  Volume{26};
}

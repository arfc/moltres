// Cammi geometry
r1 = 2.08; // cm
r2 = 5.73;
r1pt5 = 3.905; // For stupid temperature condition
H = 200; // cammi 396
lx = .5;
ly = 10;
lc = lx;

Point(1) = {0,0,0,lc};
Point(2) = {r1,0,0,lc};
Point(3) = {r2,0,0,lc};
Point(4) = {0,H,0,lc};
Point(5) = {r1,H,0,lc};
Point(6) = {r2,H,0,lc};
Point(7) = {r1pt5, 0, 0, lc};
Point(8) = {r1pt5, H, 0, lc};

// axis of symmetry
Line(1) = {1, 4};
// fuel top
Line(2) = {4, 5};
// graphite left top
Line(3) = {5, 8};
// graphite right top
Line(4) = {8, 6};
// outer wall
Line(5) = {6, 3};
// graphite right bottom
Line(6) = {3, 7};
// graphite left bottom
Line(7) = {7, 2};
// fuel bottom
Line(8) = {2, 1};
// fuel graphite interface
Line(9) = {2, 5};
// graphite center
Line(10) = {8, 7};

// fuel
Line Loop(11) = {1, 2, -9, 8};
Plane Surface(12) = {11};
// all of graphite
Line Loop(13) = {3, 4, 5, 6, 7, 9};
Plane Surface(14) = {13};
// left-half of graphite
Line Loop(15) = {3, 10, 7, 9};
Plane Surface(16) = {15};
// right half of graphite
Line Loop(17) = {4, 5, 6, -10};
Plane Surface(18) = {17};

Transfinite Line{2, 8} = r1/lx;
Transfinite Line{3, 4, 6, 7} = (r1pt5 - r1)/lx;
Transfinite Line{1, 5, 9, 10} = H/ly;
Transfinite Surface{12, 16, 18};
Recombine Surface{12, 16, 18};

Physical Surface("fuel") = {12};
Physical Surface("moder") = {14};

Physical Line("outer_wall") = {5};
Physical Line("all_top") = {2, 3, 4};
Physical Line("all_bottom") = {8, 7, 6};
Physical Line("fuel_bottom") = {8};
Physical Line("fuel_top") = {2};
Physical Line("graphite_bottom") = {7, 6};
Physical Line("graphite_top") = {3, 4};
Physical Line("graphite_middle") = {10};

Physical Surface("left_graphite") = {16};
Physical Surface("right_graphite") = {18};

// Cammi geometry
r1 = 2.08; // cm
r2 = 5.73;
H = 200; // cammi 396
lx = .05;
ly = 10;
lc = lx;

Point(1) = {0,0,0,lc};
Point(2) = {r1,0,0,lc};
Point(3) = {r2,0,0,lc};
Point(4) = {0,H,0,lc};
Point(5) = {r1,H,0,lc};
Point(6) = {r2,H,0,lc};

// x-lines
Line(1) = {1,2}; // bottom-fuel
Line(2) = {2,3}; // bottom-mod
Line(3) = {4,5}; // top-fuel
Line(4) = {5,6}; // top-mod

// y-lines
Line(5) = {1,4}; // axis-of-symmetry
Line(6) = {2,5}; // fuel-mod interface
Line(7) = {3,6}; // reactor wall

// Fuel
Line Loop(8) = {1, 6, -3, -5};
Plane Surface(9) = {8};

// Mod
Line Loop(10) = {2, 7, -4, -6};
Plane Surface(11) = {10};

Transfinite Line{1, 3} = r1/lx;
Transfinite Line{2, 4} = (r2 - r1)/lx;
Transfinite Line{5, 6, 7} = H/ly;
Transfinite Surface{9};
Transfinite Surface{11};
Recombine Surface{9};
Recombine Surface{11};

Physical Surface("fuel") = {9};
Physical Surface("moder") = {11};

Physical Line("outer_wall") = {7};
Physical Line("all_top") = {3, 4};
Physical Line("all_bottom") = {1, 2};
Physical Line("fuel_bottom") = {1};
Physical Line("fuel_top") = {3};
Physical Line("graphite_bottom") = {2};
Physical Line("graphite_top") = {4};

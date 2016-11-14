// Cammi geometry
r1 = 2.08; // cm
r2 = 5.73;
H = 396;
lc = 1;

Point(1) = {0,0,0,lc};

Point(2) = {r1,0,0,lc};
Point(3) = {0,r1,0,lc};
Point(4) = {-r1,0,0,lc};
Point(5) = {0,-r1,0,lc};

Point(6) = {r2,0,0,lc};
Point(7) = {0,r2,0,lc};
Point(8) = {-r2,0,0,lc};
Point(9) = {0,-r2,0,lc};

Circle(1) = {2,1,3};
Circle(2) = {3,1,4};
Circle(3) = {4,1,5};
Circle(4) = {5,1,2};

Circle(5) = {6,1,7};
Circle(6) = {7,1,8};
Circle(7) = {8,1,9};
Circle(8) = {9,1,6};

Line Loop(5) = {1,2,3,4};
Plane Surface(6) = {5};
Line Loop(6) = {5,6,7,8};
Plane Surface(7) = {6, 5};

Physical Volume ("fuel") = Extrude {0,0,H} {
  Surface{6};
};

Physical Volume ("moder") = Extrude {0,0,H} {
  Surface{7};
};
Physical Surface("outer_cyl_walls") = {47, 43, 55, 51};
Physical Surface("all_top") = {72, 30};
Physical Surface("all_bottom") = {7, 6};

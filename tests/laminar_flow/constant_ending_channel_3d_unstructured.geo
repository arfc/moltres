lc = 0.25;
Point(1) = {0,0,0,lc};
Point(2) = {0.5,0,0,lc};
Point(4) = {1, 0, 1, lc};
Point(5) = {1, 0, 4, lc};
Point(6) = {0, 0, 4, lc};
Line(2) = {6, 5};
Line(3) = {5, 4};
Line(4) = {4, 2};
Line(5) = {2, 1};
Point(7) = {0, 0, 1, lc};
Line(7) = {4, 7};
Line(8) = {1, 7};
Line(9) = {7, 6};
Line Loop(10) = {4, 5, 8, -7};
Plane Surface(11) = {10};
Line Loop(12) = {3, 7, 9, 2};
Plane Surface(13) = {12};

Extrude {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Surface{11,13};
}
Extrude {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Surface{47, 30};
}
Extrude {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Surface{64, 81};
}
Extrude {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Surface{98, 115};
}
Physical Surface("Walls") = {39, 22, 56, 73, 90, 107, 124, 140};
Physical Surface("Inlet") = {143, 110, 76, 25};
Physical Surface("Outlet") = {131, 97, 46, 63};
Physical Point("PressurePoint") = {6};
Physical Volume("Channel") = {3, 2, 7, 5, 6, 8, 4, 1};

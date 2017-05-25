lc = 1;

Point(1) = {-2.54, -2.54, 0, lc};
Point(2) = {-2.54, 2.54, 0, lc};
Point(3) = {2.54, 2.54, 0, lc};
Point(4) = {2.54, -2.54, 0, lc};
Point(5) = {-3.048, -3.048, 0, lc};
Point(6) = {-3.048, 3.048, 0, lc};
Point(7) = {3.048, 3.048, 0, lc};
Point(8) = {3.048, -3.048, 0, lc};
Line(1) = {6, 7};
Line(2) = {7, 8};
Line(3) = {8, 5};
Line(4) = {5, 6};
Line(5) = {2, 3};
Line(6) = {3, 4};
Line(7) = {4, 1};
Line(8) = {1, 2};
Line Loop(9) = {1, 2, 3, 4};
Line Loop(10) = {5, 6, 7, 8};
Plane Surface(11) = {9, 10};
Plane Surface(12) = {10};
Delete {
  Surface{12};
}
Delete {
  Surface{11};
}
Delete {
  Line{5, 8, 7, 6};
}
Delete {
  Point{2, 3, 4, 1};
}
Plane Surface(11) = {9};
Physical Surface(0) = {11};
Physical Line("boundary") = {1, 4, 2, 3};

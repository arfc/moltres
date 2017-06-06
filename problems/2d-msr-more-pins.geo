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
Translate {6.096, 0, 1} {
  Duplicata { Surface{11, 12}; }
}
Delete {
  Surface{13, 22};
}
Delete {
  Line{14, 17, 21, 18, 19, 16, 20, 15};
}
Delete {
  Point{9, 25, 18, 26, 30, 14, 34, 10};
}
Translate {0, 6.096, 0} {
  Duplicata { Surface{11, 12}; }
}
Translate {6.096, 0, 0} {
  Duplicata { Surface{22, 31, 11, 12}; }
}
Physical Surface("moder") = {31, 41, 55, 12};
Physical Surface("fuel") = {22, 32, 46, 11};
Physical Line("boundary") = {23, 33, 34, 48, 49, 3, 4, 26};

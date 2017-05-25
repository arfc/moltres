// Mesh.ElementOrder = 2;
// Mesh.SecondOrderLinear = 0;
radius = 1;
element_dim = 1;
Point(1) = {radius, 0, 0, element_dim} ;
Point(2) = {0, radius, 0, element_dim} ;
Point(3) = {0, 0, 0, element_dim};
Circle(1) = {1,3,2};
Line(2)={2,3};
Line(3)={3,1};
Line Loop(1) = {1,2,3};
Plane Surface(1) = {1};
Extrude {{0, 1, 0}, {0, 0, 0}, Pi/2} {
  Surface{1};
}

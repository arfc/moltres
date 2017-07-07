Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
fuel_width = 3.048;
fuel_height = 1.016;
fuel_length = 6.5;
lc = .1 * fuel_length;

Point(1) = {0, 0, 0, lc};
Point(2) = {0, 0, fuel_length, lc};
Point(3) = {fuel_width / 2., fuel_height / 2., 0, lc};
Line(1) = {1, 2};

Transfinite Line {1} = 11 Using Progression 1.2;
surface[] = Extrude {fuel_width, 0, 0} { Line{1}; Layers{5}; Recombine; };
volume[] = Extrude {0, fuel_height, 0} { Surface{surface[1]}; Layers{5}; Recombine; };
Physical Volume (0) = volume[1];
Physical Point ("entrance") = {3};
Physical Surface ("top") = volume[0];
Physical Surface ("bottom") = surface[1];
Physical Surface ("left") = volume[2];
Physical Surface ("front") = volume[3];
Physical Surface ("right") = volume[4];
Physical Surface ("back") = volume[5];

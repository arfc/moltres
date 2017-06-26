Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
fuel_width = 1;
fuel_height = 1;
fuel_length = 1.25;
lc = .1 * fuel_length;

Point(1) = {0, 0, 0, lc};
Point(2) = {0, 0, fuel_length, lc};
Line(1) = {1, 2};

Transfinite Line {1} = 31 Using Progression 1.2;
surface[] = Extrude {fuel_width, 0, 0} { Line{1}; Layers{5}; Recombine; };
volume[] = Extrude {0, fuel_height, 0} { Surface{surface[1]}; Layers{5}; Recombine; };
Physical Volume (0) = volume[1];
Physical Surface ("top") = volume[0];
Physical Surface ("bottom") = surface[1];
Physical Surface ("left") = volume[2];
Physical Surface ("front") = volume[3];
Physical Surface ("right") = volume[4];
Physical Surface ("back") = volume[5];

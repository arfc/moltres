Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
graph_sqc_rad = 39.94;
height = 162.56; // p. 101 robertson design report part 1: 162.56
lc = 2*graph_sqc_rad;

Point(1) = {-graph_sqc_rad, -graph_sqc_rad, 0, lc};
Point(2) = {graph_sqc_rad, -graph_sqc_rad, 0, lc};
Point(3) = {graph_sqc_rad, graph_sqc_rad, 0, lc};
Point(4) = {-graph_sqc_rad, graph_sqc_rad, 0, lc};
Line(1) = {4, 1};
Line(2) = {1, 2};
Line(3) = {3, 2};
Line(4) = {3, 4};

Line Loop(10) = {4, 1, 2, -3};
Plane Surface(12) = {10};

Transfinite Line {2, 1, 4, 3} = 8;
Transfinite Surface {12};
Recombine Surface {12};

moder_surfaces[] = {12};

fuel_volumes[] = {};
moder_volumes[] = {};
fuel_tops[] = {};
fuel_sides[] = {};
moder_sides[] = {};
moder_tops[] = {};
moder_out[] = Extrude {0, 0, height} { Surface{moder_surfaces[0]}; Layers{11}; Recombine; };
moder_volumes += moder_out[1];
moder_tops += moder_out[0];
moder_sides += moder_out[2];
moder_sides += moder_out[3];
moder_sides += moder_out[4];
moder_sides += moder_out[5];

Physical Volume ("fuel") = { moder_volumes[] };

Physical Surface ("fuel_bottoms") = { moder_surfaces[] };
Physical Surface ("fuel_tops") = { moder_tops[] };
Physical Surface ("fuel_sides") = { moder_sides[] };

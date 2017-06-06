Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
graph_sqc_rad = 39.94;
fuel_sqc_rad = 84.201;
pitch = 2 * fuel_sqc_rad;
num_cells = 1;
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
Point(5) = {-fuel_sqc_rad, -fuel_sqc_rad, 0, lc};
Point(6) = {-fuel_sqc_rad, fuel_sqc_rad, 0, lc};
Point(7) = {fuel_sqc_rad, fuel_sqc_rad, 0, lc};
Point(8) = {fuel_sqc_rad, -fuel_sqc_rad, 0, lc};
Line(5) = {6, 7};
Line(6) = {7, 8};
Line(7) = {8, 5};
Line(8) = {5, 6};
Line Loop(9) = {5, 6, 7, 8};
Line Loop(10) = {4, 1, 2, -3};
Plane Surface(11) = {9, 10};
Plane Surface(12) = {10};

Transfinite Line {8, 5, 6, 7} = 14;
Transfinite Line {2, 1, 4, 3} = 7;
Transfinite Surface {12};
Recombine Surface {11, 12};

fuel_surfaces[] = {11};
moder_surfaces[] = {12};
For xindex In {1:num_cells-1}
new_f_surface = Translate {xindex*pitch, 0, 0} {
  Duplicata { Surface{11}; }
};
fuel_surfaces += new_f_surface;
new_m_surface = Translate {xindex*pitch, 0, 0} {
  Duplicata { Surface{12}; }
};
moder_surfaces += new_m_surface;
EndFor // xindex

For yindex In {1:num_cells-1}
new_f_surface = Translate {0, yindex*pitch, 0} {
  Duplicata { Surface{11}; }
};
fuel_surfaces += new_f_surface;
new_m_surface = Translate {0, yindex*pitch, 0} {
  Duplicata { Surface{12}; }
};
moder_surfaces += new_m_surface;
EndFor // yindex

For xindex In {1:num_cells-1}
For yindex In {1:num_cells-1}
new_f_surface = Translate {xindex*pitch, yindex*pitch, 0} {
  Duplicata { Surface{11}; }
};
fuel_surfaces += new_f_surface;
new_m_surface = Translate {xindex*pitch, yindex*pitch, 0} {
  Duplicata { Surface{12}; }
};
moder_surfaces += new_m_surface;
EndFor // xindex
EndFor // yindex

fuel_volumes[] = {};
moder_volumes[] = {};
fuel_tops[] = {};
fuel_sides[] = {};
moder_tops[] = {};
For index In {0:num_cells*num_cells-1}
// For index In {3:5}
fuel_out[] = Extrude {0, 0, height} { Surface{fuel_surfaces[index]}; Layers{7}; Recombine; };
fuel_volumes += fuel_out[1];
fuel_tops += fuel_out[0];
fuel_sides += fuel_out[4];
fuel_sides += fuel_out[5];
fuel_sides += fuel_out[2];
fuel_sides += fuel_out[3];
moder_out[] = Extrude {0, 0, height} { Surface{moder_surfaces[index]}; Layers{7}; Recombine; };
moder_volumes += moder_out[1];
moder_tops += moder_out[0];
EndFor // index

Physical Volume ("moder") = { fuel_volumes[] };
Physical Volume ("fuel") = { moder_volumes[] };
tot_volumes = {};
tot_volumes[] = fuel_volumes[];
tot_volumes += moder_volumes[];
bound_surfs = {};
bound_surfs[] = fuel_surfaces[];
bound_surfs += moder_surfaces[];
bound_surfs += fuel_tops[];
bound_surfs += moder_tops[];
bound_surfs += fuel_sides[];

temp_diri_surfs = {};
temp_diri_surfs[] = fuel_surfaces[];
temp_diri_surfs += moder_surfaces[];
temp_diri_surfs += fuel_sides[];

temp_outflow_surfs = {};
temp_outflow_surfs[] = fuel_tops[];

temp_inflow_surfs = {};
temp_inflow_surfs[] = fuel_surfaces[];

Physical Surface ("moder_bottoms") = { fuel_surfaces[] };
Physical Surface ("moder_tops") = { fuel_tops[] };
Physical Surface ("fuel_bottoms") = { moder_surfaces[] };
Physical Surface ("fuel_tops") = { moder_tops[] };
Physical Surface ("moder_sides") = { fuel_sides[] };

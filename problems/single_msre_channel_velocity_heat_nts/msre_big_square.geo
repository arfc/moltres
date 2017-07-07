Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
cell_width = 5.08;
fuel_width = 3.05;
fuel_height = 1.02;
num_cells = 29;
height = 135; // p. 101 robertson design report part 1: 162.56
lc = fuel_height/2;


// Transfinite Line {8, 5, 6, 7} = 6;
// Transfinite Line {2, 1, 4, 3} = 3;
// Transfinite Surface {12};
// Recombine Surface {11, 12};

// fuel_surfaces[] = {11};
// moder_surfaces[] = {12};
// For xindex In {1:num_cells-1}
// new_f_surface = Translate {xindex*pitch, 0, 0} {
//   Duplicata { Surface{11}; }
// };
// fuel_surfaces += new_f_surface;
// new_m_surface = Translate {xindex*pitch, 0, 0} {
//   Duplicata { Surface{12}; }
// };
// moder_surfaces += new_m_surface;
// EndFor // xindex

// For yindex In {1:num_cells-1}
// new_f_surface = Translate {0, yindex*pitch, 0} {
//   Duplicata { Surface{11}; }
// };
// fuel_surfaces += new_f_surface;
// new_m_surface = Translate {0, yindex*pitch, 0} {
//   Duplicata { Surface{12}; }
// };
// moder_surfaces += new_m_surface;
// EndFor // yindex

// For xindex In {1:num_cells-1}
// For yindex In {1:num_cells-1}
// new_f_surface = Translate {xindex*pitch, yindex*pitch, 0} {
//   Duplicata { Surface{11}; }
// };
// fuel_surfaces += new_f_surface;
// new_m_surface = Translate {xindex*pitch, yindex*pitch, 0} {
//   Duplicata { Surface{12}; }
// };
// moder_surfaces += new_m_surface;
// EndFor // xindex
// EndFor // yindex

// fuel_volumes[] = {};
// moder_volumes[] = {};
// fuel_tops[] = {};
// fuel_sides[] = {};
// moder_tops[] = {};
// For index In {0:num_cells*num_cells-1}
// // For index In {3:5}
// fuel_out[] = Extrude {0, 0, height} { Surface{fuel_surfaces[index]}; Layers{10}; Recombine; };
// fuel_volumes += fuel_out[1];
// fuel_tops += fuel_out[0];
// If (index == 0)
// fuel_sides += fuel_out[4];
// fuel_sides += fuel_out[5];
// ElseIf (index == num_cells * num_cells - 1)
// fuel_sides += fuel_out[2];
// fuel_sides += fuel_out[3];
// ElseIf (index == num_cells -1)
// fuel_sides += fuel_out[3];
// fuel_sides += fuel_out[4];
// ElseIf (index == 2 * num_cells - 2)
// fuel_sides += fuel_out[5];
// fuel_sides += fuel_out[2];
// ElseIf (index < num_cells) // -y side
// fuel_sides += fuel_out[4];
// ElseIf (index < 2 * num_cells - 2) // -x side
// fuel_sides += fuel_out[5];
// ElseIf ((index - 2 * (num_cells - 1)) % (num_cells - 1) == 0)
// fuel_sides += fuel_out[2];
// ElseIf (index >= num_cells * num_cells - num_cells + 1 && index < num_cells * num_cells - 1)
// fuel_sides += fuel_out[3];
// EndIf
// moder_out[] = Extrude {0, 0, height} { Surface{moder_surfaces[index]}; Layers{10}; Recombine; };
// moder_volumes += moder_out[1];
// moder_tops += moder_out[0];
// EndFor // index

// Physical Volume ("moder") = { fuel_volumes[] };
// Physical Volume ("fuel") = { moder_volumes[] };
// tot_volumes = {};
// tot_volumes[] = fuel_volumes[];
// tot_volumes += moder_volumes[];
// bound_surfs = {};
// bound_surfs[] = fuel_surfaces[];
// bound_surfs += moder_surfaces[];
// bound_surfs += fuel_tops[];
// bound_surfs += moder_tops[];
// bound_surfs += fuel_sides[];

// temp_diri_surfs = {};
// temp_diri_surfs[] = fuel_surfaces[];
// temp_diri_surfs += moder_surfaces[];
// temp_diri_surfs += fuel_sides[];

// temp_outflow_surfs = {};
// temp_outflow_surfs[] = fuel_tops[];

// temp_inflow_surfs = {};
// temp_inflow_surfs[] = fuel_surfaces[];

// Physical Surface ("moder_bottoms") = { fuel_surfaces[] };
// Physical Surface ("moder_tops") = { fuel_tops[] };
// Physical Surface ("fuel_bottoms") = { moder_surfaces[] };
// Physical Surface ("fuel_tops") = { moder_tops[] };
// Physical Surface ("moder_sides") = { fuel_sides[] };
//+
Point(1) = {-cell_width/2, cell_width/2, 0, lc};
//+
Point(2) = {cell_width/2, cell_width/2, 0, lc};
//+
Point(3) = {cell_width/2, -cell_width/2, 0, lc};
//+
Point(4) = {-cell_width/2, -cell_width/2, 0, lc};
//+
Point(5) = {-fuel_width/2, -cell_width/2, 0, lc};
//+
Point(6) = {fuel_width/2, -cell_width/2, 0, lc};
//+
Point(8) = {fuel_width/2, -cell_width/2+fuel_height/2, 0, lc};
//+
Point(9) = {-fuel_width/2, -cell_width/2+fuel_height/2, 0, lc};
//+
Point(10) = {-fuel_width/2, cell_width/2-fuel_height/2, 0, lc};
//+
Point(11) = {fuel_width/2, cell_width/2-fuel_height/2, 0, lc};
//+
Point(12) = {fuel_width/2, cell_width/2, 0, lc};
//+
Point(13) = {fuel_width/2, -cell_width/2, 0, lc};
//+
Point(14) = {-fuel_width/2, cell_width/2, 0, lc};
//+
Point(15) = {cell_width/2, fuel_width/2, 0, lc};
//+
Point(16) = {cell_width/2, -fuel_width/2, 0, lc};
//+
Point(17) = {-cell_width/2, -fuel_width/2, 0, lc};
//+
Point(18) = {-cell_width/2, fuel_width/2, 0, lc};
//+
Point(19) = {-cell_width/2+fuel_height/2, fuel_width/2, 0, lc};
//+
Point(20) = {-cell_width/2+fuel_height/2, -fuel_width/2, 0, lc};
//+
Point(21) = {cell_width/2-fuel_height/2, -fuel_width/2, 0, lc};
//+
Point(22) = {cell_width/2-fuel_height/2, fuel_width/2, 0, lc};
//+
Line(1) = {1, 14};
//+
Line(2) = {14, 12};
//+
Line(3) = {12, 2};
//+
Line(4) = {2, 15};
//+
Line(5) = {15, 16};
//+
Line(6) = {16, 3};
//+
Line(7) = {3, 6};
//+
Line(8) = {6, 5};
//+
Line(9) = {5, 4};
//+
Line(10) = {4, 17};
//+
Line(11) = {17, 18};
//+
Line(12) = {18, 1};
//+
Line(13) = {14, 10};
//+
Line(14) = {10, 11};
//+
Line(15) = {11, 12};
//+
Line(16) = {15, 22};
//+
Line(17) = {22, 21};
//+
Line(18) = {21, 16};
//+
Line(19) = {8, 6};
//+
Line(20) = {8, 9};
//+
Line(21) = {9, 5};
//+
Line(22) = {17, 20};
//+
Line(23) = {20, 19};
//+
Line(24) = {19, 18};
//+
Line Loop(25) = {24, -11, 22, 23};
//+ left fuel
Plane Surface(26) = {25};
//+
Line Loop(27) = {17, 18, -5, 16};
//+ right fuel
Plane Surface(28) = {27};
//+
Line Loop(29) = {20, 21, -8, -19};
//+ bottom fuel
Plane Surface(30) = {29};
//+
Line Loop(31) = {14, 15, -2, 13};
//+ top fuel
Plane Surface(32) = {31};
//+
Line Loop(33) = {1, 13, 14, 15, 3, 4, 16, 17, 18, 6, 7, -19, 20, 21, 9, 10, 22, 23, 24, 12};
//+ moderator
Plane Surface(34) = {33};
//+
Line Loop(35) = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
//+
Plane Surface(36) = {35};
//+
Line(37) = {1, 2};
//+
Line(38) = {2, 3};
//+
Line(39) = {3, 4};
//+
Line(40) = {4, 1};

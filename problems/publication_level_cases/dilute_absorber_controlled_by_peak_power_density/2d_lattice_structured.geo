Geometry.CopyMeshingMethod = 1;
R = 72.5;
H = 153;
num_segments = 14;
pitch = R / num_segments;
x = .237952211;
fuel_rad = x * pitch;
graph_rad = pitch;
lc = 1;
lx = .4;
ly = 10;

Point(1) = {0, 0, 0, lc};
Point(2) = {fuel_rad, 0, 0, lc};
Point(3) = {graph_rad, 0, 0, lc};
Point(4) = {0, H, 0, lc};
Point(5) = {fuel_rad, H, 0, lc};
Point(6) = {graph_rad, H, 0, lc};
// fuel top
Line(1) = {4, 5};
// graph top
Line(2) = {5, 6};
// graph right edge
Line(3) = {6, 3};
// graph bottom
Line(4) = {3, 2};
// fuel bottom
Line(5) = {1, 2};
// fuel-graph interface
Line(6) = {2, 5};
// fuel left edge
Line(7) = {4, 1};

// Fuel
Line Loop(8) = {1, -6, -5, -7};
Plane Surface(9) = {8};

// Moderator
Line Loop(10) = {2, 3, 4, 6};
Plane Surface(11) = {10};

// Structured
Transfinite Line{1, 5} = fuel_rad/lx;
Transfinite Line{2, 4} = (graph_rad - fuel_rad)/lx;
Transfinite Line{3, 6, 7} = H/ly;
Transfinite Surface{9};
Transfinite Surface{11};
Recombine Surface{9};
Recombine Surface{11};

fuel_surfaces[] = {9};
moder_surfaces[] = {11};
fuel_tops[] = {1};
fuel_bottoms[] = {5};
moder_bottoms[] = {4};
moder_tops[] = {2};
For xindex In {1:num_segments-1}
new_f_surface = Translate {xindex*pitch, 0, 0} {
  Duplicata { Surface{9}; }
};
fuel_surfaces += new_f_surface;
new_m_surface = Translate {xindex*pitch, 0, 0} {
  Duplicata { Surface{11}; }
};
moder_surfaces += new_m_surface;
fuel_tops += {13 + (xindex - 1) * 8};
moder_tops += {17 + (xindex - 1) * 8};
fuel_bottoms += {15 + (xindex - 1) * 8};
moder_bottoms += {19 + (xindex - 1) * 8};
EndFor // xindex

Physical Surface("fuel") = { fuel_surfaces[] };
Physical Surface("moder") = { moder_surfaces[] };
Physical Line("fuel_tops") = { fuel_tops[] };
Physical Line("moder_tops") = { moder_tops[] };
Physical Line("fuel_bottoms") = { fuel_bottoms[] };
Physical Line("moder_bottoms") = { moder_bottoms[] };
Physical Line("outer_wall") = { 18 + 8 * (num_segments - 2) };

Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
cell_width = 5.08;
pitch = cell_width;
fuel_width = 3.05;
fuel_height = 1.02;
num_cells = 1;
height = 162.56; // p. 101 robertson design report part 1: 162.56
lc = fuel_height/2;

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
Line(37) = {1, 2};
//+
Line(38) = {2, 3};
//+
Line(39) = {3, 4};
//+
Line(40) = {4, 1};

Line Loop(41) = {37, 38, 39, 40};
Plane Surface(42) = {41};

//+
Physical Surface("fuel") = {32, 28, 30, 26};
//+
Physical Surface("moderator") = {34};

fuel_bottoms[] = {32, 28, 30, 26};
moderator_bottoms[] = {34};

For xindex In {1:num_cells-1}
  new_f_bottom = Translate {xindex*pitch, 0, 0} {
    Duplicata { Surface{32}; }
  };
  fuel_bottoms += new_f_bottom;
  new_f_bottom = Translate {xindex*pitch, 0, 0} {
    Duplicata { Surface{28}; }
  };
  fuel_bottoms += new_f_bottom;
  new_f_bottom = Translate {xindex*pitch, 0, 0} {
    Duplicata { Surface{30}; }
  };
  fuel_bottoms += new_f_bottom;
  new_f_bottom = Translate {xindex*pitch, 0, 0} {
    Duplicata { Surface{26}; }
  };
  fuel_bottoms += new_f_bottom;
  new_m_bottom = Translate {xindex*pitch, 0, 0} {
    Duplicata { Surface{34}; }
  };
  moderator_bottoms += new_m_bottom;
EndFor // xindex

For yindex In {1:num_cells-1}
  new_f_bottom = Translate {0, yindex*pitch, 0} {
    Duplicata { Surface{32}; }
  };
  fuel_bottoms += new_f_bottom;
  new_f_bottom = Translate {0, yindex*pitch, 0} {
    Duplicata { Surface{28}; }
  };
  fuel_bottoms += new_f_bottom;
  new_f_bottom = Translate {0, yindex*pitch, 0} {
    Duplicata { Surface{30}; }
  };
  fuel_bottoms += new_f_bottom;
  new_f_bottom = Translate {0, yindex*pitch, 0} {
    Duplicata { Surface{26}; }
  };
  fuel_bottoms += new_f_bottom;
  new_m_bottom = Translate {0, yindex*pitch, 0} {
    Duplicata { Surface{34}; }
  };
  moderator_bottoms += new_m_bottom;
EndFor // yindex

For xindex In {1:num_cells-1}
  For yindex In {1:num_cells-1}
    new_f_bottom = Translate {xindex*pitch, yindex*pitch, 0} {
      Duplicata { Surface{32}; }
    };
    fuel_bottoms += new_f_bottom;
    new_f_bottom = Translate {xindex*pitch, yindex*pitch, 0} {
      Duplicata { Surface{28}; }
    };
    fuel_bottoms += new_f_bottom;
    new_f_bottom = Translate {xindex*pitch, yindex*pitch, 0} {
      Duplicata { Surface{30}; }
    };
    fuel_bottoms += new_f_bottom;
    new_f_bottom = Translate {xindex*pitch, yindex*pitch, 0} {
      Duplicata { Surface{26}; }
    };
    fuel_bottoms += new_f_bottom;
    new_m_bottom = Translate {xindex*pitch, yindex*pitch, 0} {
      Duplicata { Surface{34}; }
    };
    moderator_bottoms += new_m_bottom;
  EndFor // yindex
EndFor // xindex

fuel_volumes[] = {};
moderator_volumes[] = {};
fuel_tops[] = {};
moderator_tops[] = {};

For i In {0 : num_cells*num_cells - 1}
  For j In {0 : 3}
    fuel_out[] = Extrude {0, 0, height} { Surface{fuel_bottoms[i * 4 + j]}; Layers{10}; Recombine; };
    fuel_tops += fuel_out[0];
    fuel_volumes += fuel_out[1];
  EndFor // j
  moder_out[] = Extrude {0, 0, height} { Surface{moderator_bottoms[i]}; Layers{10}; Recombine; };
  moderator_volumes += moder_out[1];
  moderator_tops += moder_out[0];
EndFor // i

Physical Volume ("moderator") = { moderator_volumes[] };
Physical Volume ("fuel") = { fuel_volumes[] };

Physical Surface ("moderator_bottoms") = { moderator_bottoms[] };
Physical Surface ("moderator_tops") = { moderator_tops[] };
Physical Surface ("fuel_bottoms") = { fuel_bottoms[] };
Physical Surface ("fuel_tops") = { fuel_tops[] };

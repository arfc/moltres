Geometry.CopyMeshingMethod = 1;
// Mesh.RandomFactor = 1e-6;
cell_width = 5.08;
pitch = cell_width;
fuel_width = 3.048;
fuel_height = 1.016;
num_cells = 1;
height = 162.56; // p. 101 robertson design report part 1: 162.56
lc = fuel_height/2;

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
Point(23) = {fuel_width/2, fuel_width/2, 0, lc};
//+
Point(24) = {fuel_width/2, -fuel_width/2, 0, lc};
//+
Point(25) = {-fuel_width/2, -fuel_width/2, 0, lc};
//+
Point(26) = {-fuel_width/2, fuel_width/2, 0, lc};
//+
Line(35) = {19, 26};
//+
Line(36) = {26, 10};
//+
Line(37) = {23, 11};
//+
Line(38) = {22, 23};
//+
Line(39) = {21, 24};
//+
Line(40) = {24, 8};
//+
Line(41) = {9, 25};
//+
Line(42) = {25, 20};
//+
Line(43) = {26, 23};
//+
Line(44) = {23, 24};
//+
Line(45) = {24, 25};
//+
Line(46) = {25, 26};
//+
Line Loop(47) = {1, 13, -36, -35, 24, 12};
//+
Plane Surface(48) = {47};
//+
Line Loop(49) = {3, 4, 16, 38, 37, 15};
//+
Plane Surface(50) = {49};
//+
Line Loop(51) = {39, 40, 19, -7, -6, -18};
//+
Plane Surface(52) = {51};
//+
Line Loop(53) = {22, -42, -41, 21, 9, 10};
//+
Plane Surface(54) = {53};
//+
Line Loop(55) = {14, -37, -43, 36};
//+
Plane Surface(56) = {55};
//+
Line Loop(57) = {44, -39, -17, 38};
//+
Plane Surface(58) = {57};
//+
Line Loop(59) = {45, -41, -20, -40};
//+
Plane Surface(60) = {59};
//+
Line Loop(61) = {42, 23, 35, -46};
//+
Plane Surface(62) = {61};
//+
Line Loop(63) = {43, 44, 45, 46};
//+
Plane Surface(64) = {63};
//+
Point(27) = {-cell_width/2+fuel_height/2, cell_width/2-fuel_height/2, 0, lc};
//+
Point(28) = {cell_width/2-fuel_height/2, cell_width/2-fuel_height/2, 0, lc};
//+
Point(29) = {cell_width/2-fuel_height/2, -cell_width/2+fuel_height/2, 0, lc};
//+
Point(30) = {-cell_width/2+fuel_height/2, -cell_width/2+fuel_height/2, 0, lc};
//+
Point(31) = {-cell_width/2, -cell_width/2+fuel_height/2, 0, lc};
//+
Point(32) = {cell_width/2, -cell_width/2+fuel_height/2, 0, lc};
//+
Point(33) = {cell_width/2, cell_width/2-fuel_height/2, 0, lc};
//+
Point(34) = {-cell_width/2, cell_width/2-fuel_height/2, 0, lc};
//+
Point(35) = {-cell_width/2+fuel_height/2, cell_width/2, 0, lc};
//+
Point(36) = {-cell_width/2+fuel_height/2, -cell_width/2, 0, lc};
//+
Point(37) = {cell_width/2-fuel_height/2, -cell_width/2, 0, lc};
//+
Point(38) = {cell_width/2-fuel_height/2, cell_width/2, 0, lc};
//+
Line(65) = {27, 35};
//+
Line(66) = {27, 34};
//+
Line(67) = {27, 19};
//+
Line(68) = {27, 10};
//+
Line(69) = {28, 38};
//+
Line(70) = {28, 33};
//+
Line(71) = {28, 22};
//+
Line(72) = {28, 11};
//+
Line(73) = {21, 29};
//+
Line(74) = {29, 32};
//+
Line(75) = {29, 37};
//+
Line(76) = {29, 8};
//+
Line(77) = {20, 30};
//+
Line(78) = {30, 9};
//+
Line(79) = {30, 36};
//+
Line(80) = {30, 31};
//+
Delete {
  Line{1, 12, 3, 4, 6, 7, 9, 10};
}
//+
Delete {
  Line{1};
}
//+
Delete {
  Surface{48, 54, 52, 50};
}
//+
Delete {
  Line{1, 12};
}
//+
Delete {
  Line{3, 4, 6, 7, 9, 10};
}
//+
Line(81) = {1, 35};
//+
Line(82) = {35, 14};
//+
Line(83) = {1, 34};
//+
Line(84) = {34, 18};
//+
Line(85) = {17, 31};
//+
Line(86) = {31, 4};
//+
Line(87) = {4, 36};
//+
Line(88) = {36, 5};
//+
Line(89) = {6, 37};
//+
Line(90) = {37, 3};
//+
Line(91) = {3, 32};
//+
Line(92) = {32, 16};
//+
Line(93) = {15, 33};
//+
Line(94) = {33, 2};
//+
Line(95) = {2, 38};
//+
Line(96) = {38, 12};
//+
Line Loop(97) = {81, -65, 66, -83};
//+
Plane Surface(98) = {97};
//+
Line Loop(99) = {82, 13, -68, 65};
//+
Plane Surface(100) = {99};
//+
Line Loop(101) = {67, 35, 36, -68};
//+
Plane Surface(102) = {101};
//+
Line Loop(103) = {66, 84, -24, -67};
//+
Plane Surface(104) = {103};
//+
Line Loop(105) = {96, -15, -72, 69};
//+
Plane Surface(106) = {105};
//+
Line Loop(107) = {95, -69, 70, 94};
//+
Plane Surface(108) = {107};
//+
Line Loop(109) = {70, -93, 16, -71};
//+
Plane Surface(110) = {109};
//+
Line Loop(111) = {72, -37, -38, -71};
//+
Plane Surface(112) = {111};
//+
Line Loop(113) = {18, -92, -74, -73};
//+
Plane Surface(114) = {113};
//+
Line Loop(115) = {75, 90, 91, -74};
//+
Plane Surface(116) = {115};
//+
Line Loop(117) = {76, 19, 89, -75};
//+
Plane Surface(118) = {117};
//+
Line Loop(119) = {40, -76, -73, 39};
//+
Plane Surface(120) = {119};
//+
Line Loop(121) = {42, 77, 78, 41};
//+
Plane Surface(122) = {121};
//+
Line Loop(123) = {79, 88, -21, -78};
//+
Plane Surface(124) = {123};
//+
Line Loop(125) = {80, 86, 87, -79};
//+
Plane Surface(126) = {125};
//+
Line Loop(127) = {80, -85, 22, 77};
//+
Plane Surface(128) = {127};
//+
Transfinite Line {43, 14, 2, 44, 17, 5, 45, 20, 8, 46, 23, 11} = 10 Using Progression 1;
//+
Transfinite Line {81, 82, 13, 68, 65, 83, 66, 84, 24, 35, 67, 36} = 5 Using Progression 1;
//+
Transfinite Line {96, 95, 94, 93, 16, 71, 69, 70, 72, 15, 37, 38, 39, 18, 92, 74, 73, 40, 76, 19, 89, 75, 91, 90, 22, 42, 41, 21, 88, 79, 77, 78, 85, 80, 86, 87} = 5 Using Progression 1;

Transfinite Surface {26, 28, 30, 32, 56, 58, 60, 62, 64, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128};

Recombine Surface {26, 28, 30, 32, 56, 58, 60, 62, 64, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128};
unit_fuel[] = {28, 32, 26, 30};
unit_moderator[] = {98, 100, 102, 104, 128, 122, 124, 126, 120, 114, 116, 118, 106, 108, 110, 112, 56, 58, 60, 62, 64};
fuel_bottoms[] = unit_fuel[];
moderator_bottoms[] = unit_moderator[];

For xindex In {1:num_cells-1}
  For i In {0 : 3}
    new_f_bottom = Translate {xindex*pitch, 0, 0} {
      Duplicata { Surface{unit_fuel[i]}; }
    };
    fuel_bottoms += new_f_bottom;
  EndFor // i
  For i In {0 : 20}
    new_m_bottom = Translate {xindex*pitch, 0, 0} {
      Duplicata { Surface{unit_moderator[i]}; }
    };
    moderator_bottoms += new_m_bottom;
  EndFor // i
EndFor // xindex

For yindex In {1:num_cells-1}
  For i In {0 : 3}
    new_f_bottom = Translate {0, yindex*pitch, 0} {
      Duplicata { Surface{unit_fuel[i]}; }
    };
    fuel_bottoms += new_f_bottom;
  EndFor // i
  For i In {0 : 20}
    new_m_bottom = Translate {0, yindex*pitch, 0} {
      Duplicata { Surface{unit_moderator[i]}; }
    };
    moderator_bottoms += new_m_bottom;
  EndFor // i
EndFor // yindex

For xindex In {1:num_cells-1}
  For yindex In {1:num_cells-1}
    For i In {0 : 3}
      new_f_bottom = Translate {xindex*pitch, yindex*pitch, 0} {
        Duplicata { Surface{unit_fuel[i]}; }
      };
      fuel_bottoms += new_f_bottom;
    EndFor // i
    For i In {0 : 20}
      new_m_bottom = Translate {xindex*pitch, yindex*pitch, 0} {
        Duplicata { Surface{unit_moderator[i]}; }
      };
      moderator_bottoms += new_m_bottom;
    EndFor // i
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
  For j In {0 : 20}
    moder_out[] = Extrude {0, 0, height} { Surface{moderator_bottoms[i * 21 + j]}; Layers{10}; Recombine; };
    moderator_volumes += moder_out[1];
    moderator_tops += moder_out[0];
  EndFor // j
EndFor // i

Physical Volume ("moderator") = { moderator_volumes[] };
Physical Volume ("fuel") = { fuel_volumes[] };

Physical Surface ("moderator_bottoms") = { moderator_bottoms[] };
Physical Surface ("moderator_tops") = { moderator_tops[] };
Physical Surface ("fuel_bottoms") = { fuel_bottoms[] };
Physical Surface ("fuel_tops") = { fuel_tops[] };

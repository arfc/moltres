//Inputs
boxdim = 1;
nx = 10;
ny = 5;
gridsize = boxdim/nx;

Point(1) = {0,0,0,gridsize};
Point(2) = {boxdim,0,0,gridsize};
Point(3) = {boxdim,boxdim,0,gridsize};
Point(4) = {0,boxdim,0,gridsize};


Line(7) = {1,2};
Line(8) = {2,3};
Line(9) = {3,4};
Line(10) = {4,1};

Line Loop(14) = {7,8,9,10};

Plane Surface(16) = 14;

//Make one square structured.
Transfinite Line{7,9} = nx + 1;
Transfinite Line{8,10} = ny + 1;
Transfinite Surface{16};
Recombine Surface{16};

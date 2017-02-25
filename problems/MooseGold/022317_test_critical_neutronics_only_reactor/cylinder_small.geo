r1 = 10; // cm
H = 10;
lc = 3;

Point(1) = {0,0,0,lc};

Point(2) = {r1,0,0,lc};
Point(3) = {0,r1,0,lc};
Point(4) = {-r1,0,0,lc};
Point(5) = {0,-r1,0,lc};

Circle(1) = {2,1,3};
Circle(2) = {3,1,4};
Circle(3) = {4,1,5};
Circle(4) = {5,1,2};

Line Loop(5) = {1,2,3,4};
Plane Surface(6) = {5};

reactor_out[] = Extrude {0,0,H} {
  Surface{6};
};

volume = {};
boundary_surfs = {};
volume[] = reactor_out[1];
boundary_surfs[] = reactor_out[0];
boundary_surfs += reactor_out[2];
boundary_surfs += reactor_out[3];
boundary_surfs += reactor_out[4];
boundary_surfs += reactor_out[5];
boundary_surfs += {6};

Physical Volume("reactor") = { volume[] };
Physical Surface("boundaries") = { boundary_surfs[] };

Point(1) = {0,0,0,1};
Point(2) = {10,0,0,1};

n = 10;
r = 2;

Line(1) = {1,2};
Transfinite Line {1} = n + 1 Using Progression r;

Physical Point("left") = {1};
Physical Point("right") = {2};
Physical Line("domain") = {1};
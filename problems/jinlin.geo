//point
//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {10, 0, 0, 1.0};
//+
Point(3) = {10, 10, 0, 1.0};
//+
Point(4) = {0, 10, 0, 1.0};

//line
//+
Line(1) = {4, 3};
//+
Line(2) = {3, 2};
//+
Line(3) = {1, 2};
//+
Line(4) = {1, 4};
//+
Transfinite Line {1} = 10 Using Progression 1;
//+
Transfinite Line {2} = 10 Using Progression 1;
//+
Transfinite Line {3} = 10 Using Progression 1;
//+
Transfinite Line {4} = 10 Using Progression 1;

//physical line
//+
Physical Line("top") = {1};
//+
Physical Line("right") = {2};
//+
Physical Line("bottom") = {3};
//+
Physical Line("left") = {4};

//surface
//+
Line Loop(5) = {1, 2, -3, 4};
//+
Plane Surface(6) = {5};
//+
Transfinite Surface {6};


//+
Recombine Surface {6};
Physical Surface("bobby") = {6};

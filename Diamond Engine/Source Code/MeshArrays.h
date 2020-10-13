#pragma once

#pragma once

static const float cubeSlowMesh[] = {
0.5f, -0.5f, 0.5f,
0.5f, -0.5f, -0.5f,
0.5f, 0.5f, -0.5f,
0.5f, -0.5f, 0.5f,
0.5f, 0.5f, -0.5f,
0.5f, 0.5f, 0.5f,

-0.5f, -0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
-0.5f, -0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
-0.5f, 0.5f, 0.5f,

-0.5f, -0.5f, -0.5f,
-0.5f, -0.5f, 0.5f,
-0.5f, 0.5f, 0.5f,
-0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, 0.5f,
-0.5f, 0.5f, -0.5f,

0.5f, -0.5f, -0.5f,
-0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, -0.5f,
0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, -0.5f,
0.5f, 0.5f, -0.5f,

-0.5f, 0.5f, -0.5f,
-0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
-0.5f, 0.5f, -0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, -0.5f,

-0.5f, -0.5f, -0.5f,
0.5f, -0.5f, -0.5f,
0.5f, -0.5f, 0.5f,
-0.5f, -0.5f, -0.5f,
0.5f, -0.5f, 0.5f,
-0.5f, -0.5f, 0.5f,
};

static const float MA_Cube_Vertices[] = {
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
};

static const int MA_Cube_Indices[] = {
5, 4, 6, 5, 6, 7,
1, 5, 7, 1, 7, 3,
0, 1, 3, 0, 3, 2,
4, 0, 2, 4, 2, 6,
2, 3, 7, 2, 7, 6,
0, 4, 5, 0, 5, 1,
};

static const float MA_Pyramid_Vertices[] = {

	-0.5, 0, -0.5,
	0.5f, 0, -0.5,
	-0.5, 0, 0.5f,
	0.5f, 0, 0.5f,
	0, 1, 0,
};
static const int MA_Pyramid_Indices[] = {
	////0, 1, 2,
	////0, 2, 3,
	//0, 4, 1,
	//1, 4, 2, 
	//2, 4, 3, 
	//3, 4, 0,
	0, 2, 4,
	2, 3, 4,
	3, 1, 4, 
	1, 0, 4,
	0, 1, 3,
	0, 3, 2,

};




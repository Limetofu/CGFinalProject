#include "stdafx.h"

float Pos_rectangle[3000] =
{
	// �Ʒ� 0
	 -0.5, -0.5, -0.5, 0.0, -1.0, 0.0, 0.0, 1.0,	  -0.5, -0.5, 0.5, 0.0, -1.0, 0.0, 0.0, 0.0,	  0.5, -0.5, 0.5, 0.0, -1.0, 0.0, 1.0, 0.0,
	 0.5, -0.5, 0.5,   0.0, -1.0, 0.0, 1.0, 0.0, 	  0.5, -0.5, -0.5, 0.0, -1.0, 0.0, 1.0, 1.0,     -0.5, -0.5, -0.5, 0.0, -1.0, 0.0, 0.0, 1.0,

	 // ���� 12
	-0.5, 0.5, -0.5, -1.0, 0.0, 0.0, 0.0, 1.0, 	 -0.5, 0.5, 0.5, -1.0, 0.0, 0.0, 1.0, 1.0,    	-0.5, -0.5, 0.5, -1.0, 0.0, 0.0, 1.0, 0.0,
	-0.5, 0.5, -0.5, -1.0, 0.0, 0.0, 0.0, 1.0,	     -0.5, -0.5, 0.5, -1.0, 0.0, 0.0, 1.0, 0.0, 	-0.5, -0.5, -0.5, -1.0, 0.0, 0.0, 0.0, 0.0,

	// right 18
	0.5, -0.5, -0.5, 1.0, 0.0, 0.0, 0.0, 0.0,	 0.5, -0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 0.0,		0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 1.0,
	0.5, -0.5, -0.5, 1.0, 0.0, 0.0, 0.0, 0.0,	 0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0, 1.0,		0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 0.0, 1.0,

	// behind 24
	-0.5, 0.5, -0.5, 0.0, 0.0, -1.0, 0.0, 1.0,	 -0.5, -0.5, -0.5, 0.0, 0.0, -1.0, 0.0, 0.0,	0.5, -0.5, -0.5, 0.0, 0.0, -1.0, 1.0, 0.0,
	-0.5, 0.5, -0.5, 0.0, 0.0, -1.0, 0.0, 1.0,	 0.5, -0.5, -0.5, 0.0, 0.0, -1.0, 1.0, 0.0,	   0.5, 0.5, -0.5, 0.0, 0.0, -1.0, 1.0, 1.0,

	// front 30
	-0.5, 0.5, 0.5, 0.0, 0.0, 1.0, 0.0, 1.0, 		 -0.5, -0.5, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0,	  0.5, -0.5, 0.5, 0.0, 0.0, 1.0, 1.0, 0.0,
	-0.5, 0.5, 0.5, 0.0, 0.0, 1.0, 0.0, 1.0,		0.5, -0.5, 0.5, 0.0, 0.0, 1.0, 1.0, 0.0,		0.5, 0.5, 0.5, 0.0, 0.0, 1.0, 1.0, 1.0,

	// top 36
	-0.5, 0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0, 	  -0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 		0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0, 0.0, 		0.5, 0.5, -0.5, 0.0, 1.0, 0.0, 1.0, 1.0,	-0.5, 0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0,
};
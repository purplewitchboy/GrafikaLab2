#include "MyOGL.h"
#include <stdio.h>
#include <Math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <mutex>
#include <thread>
#include <deque>
#include <chrono>

#include "Render.h"

void Figure(int count) {

	double A[] = { 3, -6, 0 };
	double B[] = { 9, -6, 0 };
	double C[] = { 10, -1, 0 };
	double D[] = { 16, -2, 0 };
	double E[] = { 13, 5, 0 };
	double F[] = { 9, 2, 0 };
	double G[] = { 4, 6, 0 };
	double H[] = { 0, 0, 0 };

	if (count != NULL) {
		glBegin(GL_POLYGON);
		glVertex3dv(A);	// A
		glVertex3dv(B);	// B
		glVertex3dv(C);	// C
		glVertex3dv(F);	// F
		glVertex3dv(G);	// G
		glVertex3dv(H);	// H
		glEnd();

		glBegin(GL_QUADS);
		glVertex3dv(C);	// C
		glVertex3dv(D);	// D
		glVertex3dv(E);	// E
		glVertex3dv(F);	// F
		glEnd();
	}
}
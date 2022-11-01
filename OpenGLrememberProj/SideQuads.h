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

void SideQuads(int height) {
	double A[] = { 3, -6, 0 };
	double B[] = { 9, -6, 0 };
	double C[] = { 10, -1, 0 };
	double D[] = { 16, -2, 0 };
	double E[] = { 13, 5, 0 };
	double F[] = { 9, 2, 0 };
	double G[] = { 4, 6, 0 };
	double H[] = { 0, 0, 0 };
	double AA[] = { 3, -6, 0 + height };
	double BB[] = { 9, -6, 0 + height };
	double CC[] = { 10, -1, 0 + height };
	double DD[] = { 16, -2, 0 + height };
	double EE[] = { 13, 5, 0 + height };
	double FF[] = { 9, 2, 0 + height };
	double GG[] = { 4, 6, 0 + height };
	double HH[] = { 0, 0, 0 + height };

	glBegin(GL_QUADS);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(BB);
	glVertex3dv(AA);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(CC);
	glVertex3dv(BB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(DD);
	glVertex3dv(CC);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(EE);
	glVertex3dv(DD);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(FF);
	glVertex3dv(EE);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(GG);
	glVertex3dv(FF);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(HH);
	glVertex3dv(GG);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(H);
	glVertex3dv(A);
	glVertex3dv(AA);
	glVertex3dv(HH);
	glEnd();
}
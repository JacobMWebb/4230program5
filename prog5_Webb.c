/*
   Jacob Webb
   Program 5
   CSCE 4230 Computer Graphics
   Dr. Renka

This can be compiled on the CSE machines with the makefile that is on Dr. Renka's page.
	
	TO COMPILE:***
	This can be compiled on a Apple computer with the following pasted into terminal:

	gcc prog5_Webb.c -L/System/Library/Frameworks -framework GLUT -framework OpenGL -w 
*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>//for using cos, pow, sqrt

//for axis rotations
static GLint x_rotation = 0, y_rotation = 0;

//for projection zoom in/out
static GLfloat x_center = 0.0, y_center = 0.0, scale = 0.5, radius = 1.0;
//variables for bivariate function
static int k = 60; //huge number of triangles. I want a smooth curve
static int indv, indt, i, j, i1, i2, i3;
static GLfloat h, x, y;
//had to do math by hand since we are using arrays
//NV = (k+1)^2, NT = 2k^2
enum
{
	NV = 3721,
	NT = 7442
};


//arrays we need for bivariate function
static GLfloat v[NV][3];
static unsigned int ltri[NT][3];
static GLfloat vn[NV][3];
static GLfloat tn[3];

//playing with lighting stuff. 
GLfloat mat_ambient_color[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat mat_emission[] = { 1.0, 0.0, 0.0, 1.0 };


void normalize(float *v) //basic normalize function
{
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

float f(float x, float y) //function defined in assignment 
{
	return .5 * exp(-.04 * sqrt(pow(80.0 * x - 40.0, 2.0) + pow(90.0 * y - 45.0, 2.0))) * cos(0.15 * sqrt(pow(80.0 * x - 40.0, 2.0) + pow(90.0 * y - 45.0, 2.0)));
}


void init(void) 
{
	//just playing with lighting stuff. giving our mesh a reflecting color
	
	//Get some red ambient light. Ends up roughly pink
	GLfloat ambient[] = { 1.0, 0.0, 0.0, 1.0 };
	//using some specular light so we can see lighting when viewing back side
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position[] = { 0.0, 3.0, 2.0, 0.0 };
	GLfloat model_ambient[] = { 0.1, 0.0, 1.0, 1.0 };
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	//ambient light magic
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	//specular light magic
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	//tell opengl what kind of lighting we're using
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, model_ambient);
	
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	//store our vertices
	h = 1.0 / k;
	indv = 0;
	for(j = 0; j <= k; j++) 
	{
		y = j * h;
		for(i = 0; i <= k; i++) 
		{
			x = i * h;
			v[indv][0] = x;
			v[indv][1] = y;
			v[indv][2] = f(x, y);
			indv++;
		}
	}
	//store our triangles. in triples
	indt = 0;
	for(j = 1; j <= k; j++) 
	{
		for(i = 1; i <= k; i++) 
		{
			indv = j * (k + 1) + i;
			ltri[indt][0] = indv - k - 2;
			ltri[indt][1] = indv - k - 1;
			ltri[indt][2] = indv;
			ltri[indt + 1][0] = indv - k - 2;
			ltri[indt + 1][1] = indv;
			ltri[indt + 1][2] = indv - 1;
			indt+=2;
		}
	}
	//initialize normals
	for(indv = 0; indv < NV; indv++) 
	{
		vn[indv][0] = 0;
		vn[indv][1] = 0;
		vn[indv][2] = 0;
	}
	//add our triangle normals to our vertex normals
	for(indt = 0; indt < NT ; indt++) 
	{
		i1 = ltri[indt][0];
		i2 = ltri[indt][1];
		i3 = ltri[indt][2];
		tn[0] = (v[i2][1] - v[i1][1]) * (v[i3][2] - v[i1][2]) - (v[i2][2] - v[i1][2]) * (v[i3][1] - v[i1][1]);
		tn[1] = (v[i2][2] - v[i1][2]) * (v[i3][0] - v[i1][0]) - (v[i2][0] - v[i1][0]) * (v[i3][2] - v[i1][2]);
		tn[2] = (v[i2][0] - v[i1][0]) * (v[i3][1] - v[i1][1]) - (v[i2][1] - v[i1][1]) * (v[i3][0] - v[i1][0]);
		normalize(tn);
		vn[i1][0] += tn[0];
		vn[i1][1] += tn[1];
		vn[i1][2] += tn[2];
		vn[i2][0] += tn[0];
		vn[i2][1] += tn[1];
		vn[i2][2] += tn[2];
		vn[i3][0] += tn[0];
		vn[i3][1] += tn[1];
		vn[i3][2] += tn[2];
	}
	//normalize all of our vertex normals
	//just an fyi, if you don't do this the lighting is much too bright.
	for(indv = 0; indv < NV; indv++) 
	{
		normalize(vn[indv]);
	} 
	//tell openGL we're using vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY); //for lighting
	//tell openlGL the name of our vertex arrays
	glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), v);
	glNormalPointer(GL_FLOAT, 3 * sizeof(GLfloat), vn); //for lighting
}
   
void display(void)
{
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//push model view
	glPushMatrix();
	//apply rotations to axes
	glRotatef((GLfloat) x_rotation, 1.0, 0.0, 0.0);
	glRotatef((GLfloat) y_rotation, 0.0, 1.0, 0.0);
	//push rotations
	glPushMatrix();
	//draw our vertex arrays
	//NT*3 because 3 vertices per triangle
	glDrawElements(GL_TRIANGLES, NT*3, GL_UNSIGNED_INT, ltri);
	glPopMatrix();
	glPopMatrix();
	//since we're right multiplying we'll draw and then rotate
	glutSwapBuffers();
}

void reshape (int w, int h)
{
	//preserve ration with window resizing
	if(w < h)
	{
		glViewport(0, (GLint)(h-w)/2, (GLint)w, (GLint)w);
	}
	else if(h < w)
	{
		glViewport((GLint)(w-h)/2, 0, (GLint)h, (GLint)h);
	}
	else 
	{
		glViewport (0, 0, (GLsizei)w, (GLsizei)h); 
	}
	//projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	//so we can zoom in/out
	//left, right, bottom, top, near, far
	//arbitrary near/far
	glFrustum(x_center-scale*radius, x_center+scale*radius, y_center-scale*radius, y_center+scale*radius, 1, 7);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//translate a bit to center it. Arbitrary values
	glTranslatef(-0.5, -0.5, -4.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key) {
		case 'x':
			x_rotation = (x_rotation + 5) % 360;
			break;
		case 'X':
			x_rotation = (x_rotation - 5) % 360;
			break;
		case 'y':
			y_rotation = (y_rotation + 5) % 360;
			break;
		case 'Y':
			y_rotation = (y_rotation - 5) % 360;
			break;
		case '=':
			scale = scale * .9; //preserve our ration when we zoom in
			reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			break;
		case'+':
			scale = scale * 1.1; //preserve our ration when we zoom out
			reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
	}
	glutPostRedisplay();	
}

void menu(int item)
{
	switch(item) {
		case 'x':
			x_rotation = (x_rotation + 5) % 360;
			break;
		case 'X':
			x_rotation = (x_rotation - 5) % 360;
			break;
		case 'y':
			y_rotation = (y_rotation + 5) % 360;
			break;
		case 'Y':
			y_rotation = (y_rotation - 5) % 360;
			break;
		case '=':
			scale = scale * .9; //zoom in
			break;
		case '+':
			scale = scale * 1.1; //zoom out
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
	}
	glutPostRedisplay(); //display changes after moving/zooming
	return;
}

void CreateMenu(void) {
	/* create menu */
	glutCreateMenu(menu);
	/* add menu entries */
	glutAddMenuEntry("Press x to rotate view angle about x axis ccw", 'x');
	glutAddMenuEntry("Press shift+x rotate view angle about x axis cw", 'X');
	glutAddMenuEntry("Press y to rotate view angle about y axis ccw", 'y');
	glutAddMenuEntry("Press shift+y to rotate view angle about y axis cw", 'Y');
	glutAddMenuEntry("Press = to zoom in", '=');
	glutAddMenuEntry("Press shift+= to zoom out", '+');
	glutAddMenuEntry("ESC: terminate execution", 27);
	/* attach menu to right mouse button */
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	return;
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(500, 500); 
   glutInitWindowPosition(100, 100);
   glutCreateWindow("Program 5 Webb, Jacob");
   init();
   CreateMenu();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;   
}
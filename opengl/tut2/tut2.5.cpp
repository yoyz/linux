// g++ tut2.5.cpp -o tut2.5 -lm -lGLU -lglut  -lGL

#include <stdio.h>
#include <stdlib.h>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <math.h>
#include <sys/time.h>   // gettimeofday



using namespace std;

// The coordinates for the vertices of the cube
double x = 0.6;
double y = 0.6;
double z = 0.6;

float angle = 0.0;

int timer=1;
float timediff_in_usec=0.0;
float timediff_in_sec=0.0;
struct timeval tv0;
struct timeval tv1;
struct timezone tz; 

void drawCube()
{
    gettimeofday(&tv0,&tz);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0.0, 0.0, -5.0);

        // Add an ambient light
        GLfloat ambientColor[] = {0.2, 0.2, 0.2, 1.0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

        // Add a positioned light
        GLfloat lightColor0[] = {0.5, 0.5, 0.5, 1.0};
        GLfloat lightPos0[] = {4.0, 0.0, 8.0, 1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

        glTranslatef(0.5, 1.0, 0.0);
        glRotatef(angle, 1.0, 1.0, 1.0);

    glRotatef( angle, 1.0, 0.0, 1.0 );
    glRotatef( angle, 0.0, 1.0, 1.0 );
        glTranslatef(-0.5, -1.0, 0.0);

        // Create the 3D cube

    // BACK
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.3, 0.2);
    glVertex3f(x, -y, z);
    glVertex3f(x, y, z);
    glVertex3f(-x, y, z);
    glVertex3f(-x, -y, z);
    glEnd();

        // FRONT
        glBegin(GL_POLYGON);
        glColor3f(0.0, 0.5, 0.0);
        glVertex3f(-x, y, -z);
        glVertex3f(-x, -y, -z);
        glVertex3f(x, -y, -z);
        glVertex3f(x, y, -z);
        glEnd();

        // LEFT
        glBegin(GL_POLYGON);
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-x, -y, -z);
        glVertex3f(-x, -y, z);
        glVertex3f(-x, y, z);
        glVertex3f(-x, y, -z);
        glEnd();


        // RIGHT
        glBegin(GL_POLYGON);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(x, -y, -z);
        glVertex3f(x, -y, z);
        glVertex3f(x, y, z);
        glVertex3f(x, y, -z);
        glEnd();

        // TOP
        glBegin(GL_POLYGON);
        glColor3f(0.6, 0.0, 0.0);
        glVertex3f(x, y, z);
        glVertex3f(-x, y, z);
        glVertex3f(-x, y, -z);
        glVertex3f(x, y, -z);
        glEnd();


        // BOTTOM
        glBegin(GL_POLYGON);
        glColor3f(0.3, 0.0, 0.3);
        glVertex3f(-x, -y, -z);
        glVertex3f(-x, -y, z);
        glVertex3f(x, -y, z);
        glVertex3f(x, -y, -z);
        glEnd();

        glFlush();
    glutSwapBuffers();
    gettimeofday(&tv1,&tz);    
    timediff_in_usec=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
    timediff_in_sec=timediff_in_usec/1000000;

    printf("fps: %f timetorender : %f\n",1/timediff_in_sec,timediff_in_sec);

}

// Function for increasing the angle variable smoothly, 
// keeps it <=360
// It can also be implemented using the modulo operator.
void update(int value)
{
        angle += 1.0f;
        if (angle > 360)
                {
                        angle -= 360;
        }

        glutPostRedisplay();
        glutTimerFunc(timer, update, 0);
}

// Initializes 3D rendering
void initRendering()
{
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);

        // Set the color of the background
        glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
}


// Called when the window is resized
void handleResize(int w, int h)
{
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL - Rotating a Cube");
        initRendering();
    glutDisplayFunc(drawCube);
        glutReshapeFunc(handleResize);

        // Add a timer for the update(...) function
    glutTimerFunc(timer, update, 0);
    //    wglXSwapIntervalEXT(0);
    glutMainLoop();
    return 0;
}

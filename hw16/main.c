#include <stdio.h>
#include <GL/glut.h>

int alpha;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();

    glBegin(GL_LINES);
    for (GLfloat i = -2.5; i <= 2.5; i += 0.25)
    {
        glVertex3f(i, 0, 2.5);
        glVertex3f(i, 0, -2.5);
        glVertex3f(2.5, 0, i);
        glVertex3f(-2.5, 0, i);
    }
    glEnd();

    glRotatef(alpha, 1, 0, 0);
    glRotatef(alpha, 0, 0, 1);

    alpha += 4;
    if (alpha > 359) alpha = 0;

    glBegin(GL_QUAD_STRIP);
        glColor3f(0.5, 1, 1); glVertex3f(0, .7, 1);
        glColor3f(0.5, 0, 1); glVertex3f(0, -.7, 1);

        glColor3f(1, 1, 0.5); glVertex3f(1, .7, 0);
        glColor3f(1, 0, 0.5); glVertex3f(1, -.7, 0);

        glColor3f(0.5, 1, 0); glVertex3f(0, .7, -1);
        glColor3f(0.5, 0, 0); glVertex3f(0, -.7, -1);

        glColor3f(0, 1, 0.5); glVertex3f(-1, .7, 0);
        glColor3f(0, 0, 0.5); glVertex3f(-1, -.7, 0);

        glColor3f(0.5, 1, 1); glVertex3f(0, .7, 1);
        glColor3f(0.5, 0, 1); glVertex3f(0, -.7, 1);
    glEnd();
    glBegin(GL_QUADS);
        glColor3f(0.5, 1, 1); glVertex3f(0, .7, 1);
        glColor3f(1, 1, 0.5); glVertex3f(1, .7, 0);
        glColor3f(0.5, 1, 0); glVertex3f(0, .7, -1);
        glColor3f(0, 1, 0.5); glVertex3f(-1, .7, 0);

        glColor3f(0.5, 0, 1); glVertex3f(0, -1, 1);
        glColor3f(0, 0, 0.5); glVertex3f(-1, -1, 0);
        glColor3f(0.5, 0, 0); glVertex3f(0, -1, -1);
        glColor3f(1, 0, 0.5); glVertex3f(1, -1, 0);
    glEnd();

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glFlush();
    glutSwapBuffers();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glColor3f(1.0, 1.0, 1.0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2, 2, -1.5, 1.5, 1, 40);
    // glOrtho(-1, 1, -1, 1, 0, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -3);
    glRotatef(50, 1, 0, 0);
}

void Visibility(int state) {
    if (state == GLUT_NOT_VISIBLE) printf ("Window not visible!\n");
    if (state == GLUT_VISIBLE) printf ("Window visible!\n");
}

void timf (__attribute__((unused)) int value) {
    glutPostRedisplay();
    glutTimerFunc(40, timf, 0);
}

int main(int argc, char **argv) {
    alpha = 4 ;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(800, 600);
    glutCreateWindow("A Simple Cube");
    glutDisplayFunc(display);
    glutTimerFunc(40, timf, 0);
    glutVisibilityFunc(Visibility);

    init();
    glutMainLoop();
}

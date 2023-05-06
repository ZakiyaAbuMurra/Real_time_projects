#include <GL/gl.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <GL/glut.h>
#include "iostream"
#include "shared_data.h"
#include "shared_memory.h"
using namespace std;
float x; // x coordinate of the point
float y; // y coordinate of the point
int employee1;
int employee2;
int storage_area;
int carton_typeA;
int carton_typeB;
int carton_typeC;
int typeA_pices;
int typeB_pices;
int typeC_pices;
int truckA;
int truckB;
int truckC;
int stop_production_lines;
int stop_simulation;
int shipped_truck[3] = {0, 0, 0};
int printing_expired_date = 0;
float lineA[8][3], lineA1 = -0.94 * 150, lineA2 = -0.79 * 150, lineA3 = -0.64 * 150;
float lineB[6][2], lineB1 = -0.49 * 150, lineB2 = -0.34 * 150;
float lineC[5][2], lineC1 = -0.19 * 150, lineC2 = -0.04 * 150;
float line8_x = -0.94;
int flowA = 0, flowB = 0, flowC = 0;

void init_flowA()
{
    for (int i = 0; i < 8; i++)
    {
        lineA[i][0] = 0.7;
        lineA[i][1] = 0.7;
        lineA[i][2] = 0.7;
    }
}
void init_flowB()
{
    for (int i = 0; i < 6; i++)
    {
        lineB[i][0] = 0.7;
        lineB[i][1] = 0.7;
    }
}
void init_flowC()
{
    for (int i = 0; i < 5; i++)
    {
        lineC[i][0] = 0.7;
        lineC[i][1] = 0.7;
    }
}

sigset_t signal_set; // set of signals
void wait_for_starting_signal()
{
    // initialize the signal set
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGUSR1);

    // block the signals in the set
    sigprocmask(SIG_BLOCK, &signal_set, nullptr);

    // code that does some work

    // wait for a signal
    int signal_number;
    sigwait(&signal_set, &signal_number);
}
void init_flow(int req)
{
    if (req == 0)
    {
        init_flowA();
        init_flowB();
        init_flowC();
    }
    else if (req == 1)
    {
        init_flowA();
    }
    else if (req == 2)
    {
        init_flowB();
    }
    else if (req == 3)
    {
        init_flowC();
    }
}

void change_flow()
{
    for (int i = 0; i <= flowA; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            lineA[i][j] -= 0.1;
        }
    }
    flowA++;
    for (int i = 0; i <= flowB; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            lineB[i][j] -= 0.1;
        }
    }
    flowB++;
    for (int i = 0; i <= flowC; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            lineC[i][j] -= 0.2;
        }
    }
    flowC++;
}
void renderText(float x, float y, void *font, const char *s)
{
    glRasterPos2f(x, y);
    int len, i;
    len = strlen(s);
    for (i = 0; i < len && s[i] != '\0'; i++)
    {
        glutBitmapCharacter(font, s[i]);
    }
}

void drawText()
{
    glColor3f(1.0, 0.8, 0.5);
    renderText(-.80 * 150, -.55 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("employee 1 : " + to_string(employee1)).data());
    renderText(-.30 * 150, -.55 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("employee 2 : " + to_string(employee2)).data());
    renderText(.2 * 150, -.15 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("STORAGE AREA : " + to_string(storage_area)).data());

    renderText(.25 * 150, -0.3 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type A pices : " + to_string(typeA_pices)).data());
    renderText(.25 * 150, -0.4 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type B pices : " + to_string(typeB_pices)).data());
    renderText(.25 * 150, -0.5 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type C pices : " + to_string(typeC_pices)).data());

    renderText(.22 * 150, -0.6 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type A cartons : " + to_string(carton_typeA)).data());
    renderText(.22 * 150, -0.7 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type B cartons : " + to_string(carton_typeB)).data());
    renderText(.22 * 150, -0.8 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type C cartons : " + to_string(carton_typeC)).data());
    renderText(.63 * 150, .55 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "SHIPPING AREA");

    renderText(.63 * 150, .40 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type A loaded : " + to_string(truckA)).data());
    renderText(.63 * 150, 0.30 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type B loaded  : " + to_string(truckB)).data());
    renderText(.63 * 150, 0.20 * 150, GLUT_BITMAP_TIMES_ROMAN_24, ("type C loaded  : " + to_string(truckC)).data());
}

void startChocolate()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glBegin(GL_POINTS);
    glPointSize(50.0);

    glColor3f(255, 255, 255);
    glVertex2i(x, y);
    glEnd();
}

void display()
{

    int i;

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    glBegin(GL_LINES);

    float j = 0;
    for (int i = 1; i <= 7; i++)
    {
        glBegin(GL_LINES);
        if (i <= 3)
            glColor3f(255, 0, 0);
        else if (i <= 5)
            glColor3f(0, 255, 0);
        else
            glColor3f(0, 0, 255);

        // Main lines
        glVertex2f((-.97 + j) * 150, .85 * 150);
        glVertex2f((-.97 + j) * 150, -.3 * 150);
        glVertex2f((-.91 + j) * 150, .85 * 150);
        glVertex2f((-.91 + j) * 150, -.3 * 150);
        // Square below the main line
        glVertex2f((-.97 + j) * 150, -.35 * 150);
        glVertex2f((-.97 + j) * 150, -.43 * 150);
        glVertex2f((-.91 + j) * 150, -.35 * 150);
        glVertex2f((-.91 + j) * 150, -.43 * 150);

        glVertex2f((-.97 + j) * 150, -.35 * 150);
        glVertex2f((-.91 + j) * 150, -.35 * 150);
        glVertex2f((-.97 + j) * 150, -.43 * 150);
        glVertex2f((-.91 + j) * 150, -.43 * 150);
        glEnd();

        // Employees for each line
        // Employees for each line
        glColor3f(1.0, 0.8, 0.5);
        if (i <= 3)
        {
            renderText((-.99 + j) * 150, .7 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .6 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .5 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .4 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .3 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .2 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .1 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, 0, GLUT_BITMAP_TIMES_ROMAN_24, "X");
        }
        else if (i <= 5)
        {
            renderText((-.99 + j) * 150, .7 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .6 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .5 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .4 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .3 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .2 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
        }
        else
        {
            renderText((-.99 + j) * 150, .7 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .5 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .3 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, .1 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
            renderText((-.99 + j) * 150, -0.1 * 150, GLUT_BITMAP_TIMES_ROMAN_24, "X");
        }
        j += .15;
    }
    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            for (int k = 0; k < 8; k++)
            {
                glPointSize(10.0);
                glBegin(GL_POINTS);
                glVertex2i(lineA1, lineA[k][0] * 150);
                glEnd();
            }
        }
        else if (i == 1)
        {
            for (int k = 0; k < 6; k++)
            {
                glPointSize(10.0);
                glBegin(GL_POINTS);
                glVertex2i(lineA2, lineA[k][1] * 150);
                glEnd();
            }
        }
        else if (i == 2)
        {
            for (int k = 0; k < 8; k++)
            {
                glPointSize(10.0);
                glBegin(GL_POINTS);
                glVertex2i(lineA3, lineA[k][2] * 150);
                glEnd();
            }
        }
    }

    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            for (int k = 0; k < 6; k++)
            {
                glPointSize(10.0);
                glBegin(GL_POINTS);
                glVertex2i(lineB1, lineB[k][0] * 150);
                glEnd();
            }
        }
        else if (i == 1)
        {
            for (int k = 0; k < 6; k++)
            {
                glPointSize(10.0);
                glBegin(GL_POINTS);
                glVertex2i(lineB2, lineB[k][1] * 150);
                glEnd();
            }
        }
    }
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            for (int k = 0; k < 5; k++)
            {
                glPointSize(10.0);
                glBegin(GL_POINTS);
                glVertex2i(lineC1, lineC[k][0] * 150);
                glEnd();
            }
        }
        else if (i == 1)
        {
            for (int k = 0; k < 5; k++)
            {
                glPointSize(10.0);
                glBegin(GL_POINTS);
                glVertex2i(lineC2, lineC[k][1] * 150);
                glEnd();
            }
        }
    }
    // point at line 8
    glPointSize(40.0);
    glBegin(GL_POINTS);
    glVertex2i(line8_x * 150, -.7 * 150);
    glEnd();
    // Line 8
    glBegin(GL_LINES);
    glColor3f(255, 255, 255);
    glVertex2f(-.97 * 150, -.6 * 150);
    glVertex2f(.1 * 150, -.6 * 150);
    glVertex2f(-.97 * 150, -.8 * 150);
    glVertex2f(.1 * 150, -.8 * 150);

    // Expiration date printer
    glVertex2f(.04 * 150, -.6 * 150);
    glVertex2f(.04 * 150, -.8 * 150);
    glVertex2f(.08 * 150, -.6 * 150);
    glVertex2f(.08 * 150, -.8 * 150);

    // Storage area
    glVertex2f((.2) * 150, -.9 * 150);
    glVertex2f((.2) * 150, -.2 * 150);
    glVertex2f((.45) * 150, -.9 * 150);
    glVertex2f((.45) * 150, -.2 * 150);

    glVertex2f((.2) * 150, -.9 * 150);
    glVertex2f((.45) * 150, -.9 * 150);
    glVertex2f((.2) * 150, -.2 * 150);
    glVertex2f((.45) * 150, -.2 * 150);

    // Truck area
    glVertex2f((.6) * 150, -.9 * 150);
    glVertex2f((.6) * 150, .5 * 150);
    glVertex2f((.85) * 150, -.9 * 150);
    glVertex2f((.85) * 150, .5 * 150);

    glVertex2f((.6) * 150, -.9 * 150);
    glVertex2f((.85) * 150, -.9 * 150);
    glVertex2f((.6) * 150, .5 * 150);
    glVertex2f((.85) * 150, .5 * 150);
    glEnd();
    j = 0;

    for (int i = 1; i <= 3; i++)
    {
        glColor3f(255, 255, 0);
        if (shipped_truck[i - 1])
        {
            glBegin(GL_POLYGON);
            glVertex2f(.65 * 150, (-.1 - j) * 150);
            glVertex2f(0.8 * 150, (-.1 - j) * 150);
            glVertex2f(0.8 * 150, (.06 - j) * 150);
            glVertex2f(.65 * 150, (.06 - j) * 150);
            glEnd();
        }
        else
        {
            glBegin(GL_LINES);
            glVertex2f((.65) * 150, (.06 - j) * 150);
            glVertex2f((.65) * 150, (-.1 - j) * 150);
            glVertex2f((.8) * 150, (.06 - j) * 150);
            glVertex2f((.8) * 150, (-.1 - j) * 150);

            glVertex2f((.65) * 150, (.06 - j) * 150);
            glVertex2f((.8) * 150, (.06 - j) * 150);
            glVertex2f((.65) * 150, (-.1 - j) * 150);
            glVertex2f((.8) * 150, (-.1 - j) * 150);
            glEnd();
        }

        j += .36;
    }

    drawText();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-150, 150, -150, 150);
    glMatrixMode(GL_MODELVIEW);
}
void read_data()
{
    employee1 = read_shared_memory(SHARED_MEMORY_OPERATION(EMPLOYEE1));
    employee2 = read_shared_memory(SHARED_MEMORY_OPERATION(EMPLOYEE2));
    storage_area = read_shared_memory(SHARED_MEMORY_OPERATION(STORAGE_AREA));
    carton_typeA = read_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEA));
    carton_typeB = read_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEB));
    carton_typeC = read_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEC));
    typeA_pices = read_shared_memory(SHARED_MEMORY_OPERATION(TYPEA_PICES));
    typeB_pices = read_shared_memory(SHARED_MEMORY_OPERATION(TYPEB_PICES));
    typeC_pices = read_shared_memory(SHARED_MEMORY_OPERATION(TYPEC_PICES));
    truckA = read_shared_memory(SHARED_MEMORY_OPERATION(TRUCKA));
    truckB = read_shared_memory(SHARED_MEMORY_OPERATION(TRUCKB));
    truckC = read_shared_memory(SHARED_MEMORY_OPERATION(TRUCKC));
    shipped_truck[0] = read_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK1_SHIPPED));
    shipped_truck[1] = read_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK2_SHIPPED));
    shipped_truck[2] = read_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK3_SHIPPED));
    printing_expired_date = read_shared_memory(SHARED_MEMORY_OPERATION(IS_PRINTING_EXPIRED_DATE));
}
void timer(int)
{
    glutPostRedisplay();
    glutTimerFunc(1000 / 30, timer, 0);
    change_flow();
    if (printing_expired_date || (line8_x < 0.0 && line8_x > -0.94))
    {
        line8_x += 0.04;
    }

    if (line8_x >= 0.0)
    {
        line8_x = -.94;
    }
    if (flowA == 8)
    {
        flowA = 0;
        init_flow(1);
    }
    if (flowB == 6)
    {
        flowB = 0;
        init_flow(2);
    }
    if (flowC == 5)
    {
        flowC = 0;
        init_flow(3);
    }
    read_data();
}

int main(int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    wait_for_starting_signal();
    init_flow(0);
    glutInitWindowSize(1900, 1000);
    glutCreateWindow("Chocolate Factory");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    return 0;
}
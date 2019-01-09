/*********************************************************/
/*                    openGL.cc                          */
/*********************************************************/
/*                                                       */
/*********************************************************/ 

#include <ctime>
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <jpeglib.h>
#include <jerror.h>
#define Pi 3.141592654

#include <vector>
#include "FractionRationnelle.h"
#include "PointMassique.h"
#include "Bernstein.h"
#include "Casteljau.h"
#include "Segment.h"
#include "Dessin.h"

using namespace std;

double Scal = 0.1;

double trX = 0.0, trY = 0.0, dist = 0.; //,trZ=0.0
char pressRight, pressLeft;
int anglex, angley, xLeft, yLeft, xLeftOld, yLeftOld;
int xRight, yRight, xRightOld, yRightOld;

/* Prototype des fonctions */
void affichage();//  procedure a modifier en fonction de la scene
void clavier(unsigned char touche, int x, int y); 
void reshape(int x, int y);
void idle();
void mouse(int bouton, int etat, int x, int y);
void mousemotion(int x, int y);

//-************************************************************
//
//  Procedure avec mise en file des sommets des primitives
//
//-***********************************************************
void init();

int main(int argc, char **argv)
{
    /* initialisation de glut et creation de la fenetre */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("RQBC et conique.");
    /* Initialisation d'OpenGL */


    time_t now = time(0);
    tm * ltm = localtime(&now);
    if (ltm->tm_hour < 9 || ltm->tm_hour >= 17)
        glClearColor(0.5, 0.5, 0.5, 0.0);
    else
        glClearColor(1.0, 1.0, 1.0, 0.0);
        
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(2.0);
    glEnable(GL_DEPTH_TEST);


    glColor3f(0.0, 0.0, 0.0);
    //glEdgeFlag(GL_FALSE);
    glEdgeFlag(GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    // glEnable(GL_LIGHTING);
    glDisable(GL_LIGHTING);


    /* enregistrement des fonctions de rappel */
    init();
    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);

    /* Entree dans la boucle principale glut */
    glutMainLoop();
    return 0;
}

/****************************************************************
 **                                                            **
 **                   Fonctions de callback                    **
 **                                                            **
 ****************************************************************/

void zoomIn()
{
    if(dist < 149)
    {
        dist += 0.5;
        Scal += 0.5;
    }
    glutPostRedisplay();
}

void zoomOut()
{
    if(dist >= 0.5)
    {
        dist -= 0.5;
        Scal -= 0.5;
    }
    glutPostRedisplay();
}

void clavier(unsigned char touche, int x, int y)
{
    switch (touche)
    {
        case 'q' : /*la touche 'q' permet de quitter le programme */
        case 'Q' : exit(0);
        case '+' : zoomOut(); break;
        case '-' : zoomIn();  break;
        case '6' : trX -= 0.25;  glutPostRedisplay();  break;
        case '4' : trX += 0.25;  glutPostRedisplay();  break;
        case '8' : trY += 0.25;  glutPostRedisplay();  break;
        case '2' : trY -= 0.25;  glutPostRedisplay();  break;
        case '0' : trX = 0.; trY = 0.; anglex = 0.; angley = 0.; glutPostRedisplay(); break;
    }
}

void reshape(int x, int y)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //taille de la scene
    double Ortho = -300;
    glOrtho(-Ortho, Ortho, -Ortho, Ortho, -Ortho, Ortho); // fenetre
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);
}

void mouse(int button, int state,int x,int y)
{
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN)
            {
                pressLeft = 1;
                xLeftOld = x;
                yLeftOld = y;
            }
            else if(state == GLUT_UP)
                pressLeft = 0;
            break;
        case GLUT_RIGHT_BUTTON:
        {
            if(state == GLUT_DOWN)
            {
                pressRight = 1; /* le booleen presse passe a 1 (vrai) */
                xRightOld = x;  /* on sauvegarde la position de la souris */
                xRightOld = y;
            }
            else if(state == GLUT_UP)
                pressRight = 0;
            break;
        }
        case 3: zoomIn();  break;
        case 4: zoomOut(); break;
    }
}

void mousemotion(int x, int y)
{
    if (pressRight) /* si le bouton gauche est presse */
    {
        /* on modifie les angles de rotation de l'objet
        en fonction de la position actuelle de la souris et de la derniere
        position sauvegardee */
        anglex = anglex + (x - xRightOld);
        angley = angley + (y - yRightOld);
        glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
        xRightOld = x;       /* sauvegarde des valeurs courante de le position de la souris */
        xRightOld = y;
    }
    else if (pressLeft)
    {
        trX -= (x - xLeftOld) * 1 / Scal * 0.08;
        trY -= (y - yLeftOld) * 1 / Scal * 0.08;
        glutPostRedisplay();
        xLeftOld = x; /* sauvegarde des valeurs courante de le position de la souris */
        yLeftOld = y;
    } 
}


/****************************************************************
 **                                                            **
 **                    Affichage de  la scene                  **
 **                                                            **
 ****************************************************************/

//fonction ou les objets sont a definir
void init()
{ 
    Point O(0., 0.), I(1., 0.), J(0., 1.);
    Segment abscisse(-1000000, 0, 1000000, 0), ordonnee(0, -1000000, 0, 1000000);

    double num[] = {2, 0, -1};
    double denom[] = {1, 2, -3};

    Polynome numerateur(num, 3);
    Polynome denominateur(denom, 3);

    FractionRationnelle F(numerateur, denominateur);

    vector<double> n;
    vector<double> d;

    n.push_back(-1);
    n.push_back(0);
    n.push_back(2);

    d.push_back(-3);
    d.push_back(2);
    d.push_back(1);

    // PointMassique A(0, 0.333333, -3);
    // PointMassique B(0.428571, 0.428571, -2.333333);
    // PointMassique C(1, 0.25, -1.3333);
    // PointMassique D(0, 1, 0);
    PointMassique A(0, -1, -3);
    PointMassique B(-1, -1, -2.333333);
    PointMassique C(-1.33, -0.3333, -1.33333);
    PointMassique D(0, 1, 0);
    FractionRationnelle T(A, B, C, D);
    T.m_numerateur = n;
    T.m_denominateur = d;
    // cout << F.to_s() << endl;

    glNewList(1, GL_COMPILE_AND_EXECUTE); //liste numero 1
        trace_point(O, 0., 0., 1., 15.);  //O
        trace_point(I, 1., 0., 0., 10.);  //I
        trace_point(J, 0., 0.5, 0., 10.); //J
        trace_segment(O, I, 1.0, 0.0, 1.0, 2.0);  // on trace [OI]
        trace_segment(O, J, 1.0, 0.50, 0.0, 2.0); // on trace [OJ]
        trace_segment(abscisse, 0., 0., 0., 0.5); // on trace l'axe des abcsisse
        trace_segment(ordonnee, 0., 0., 0., 0.5); // on trace l'axe des ordonnees
    glEndList();

    glNewList(2, GL_COMPILE_AND_EXECUTE); //liste numero 2
        F.trace_assymptotes();
        // T.trace_assymptotes();
    glEndList();

    glNewList(3, GL_COMPILE_AND_EXECUTE); //liste numero 3
        // F.trace_courbe();
        // T.trace_courbe();
        // trace_point(A, 0, 0,0, 5);
        // trace_point(B, 0, 0,0, 5);
        // trace_point(C, 0, 0,0, 5);
        // trace_point(D, 0, 0,0, 5);

        // for(double t = 0.001; t < 1; t += 0.001)X
            de_casteljau(A, B, C, D);
    glEndList();

    cout << "\nDone." << endl;
}

// fonction permettant d'afficher les objets en utilisant des listes   
void affichage() 
{
    /* effacement de l'image avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0, 0.0, dist);
    // Pour la 3D
    // glRotatef(-angley,1.0,0.0,0.0);
    // glRotatef(-anglex,0.0,1.0,0.0);
    // Pour la 2D
    glRotatef(-anglex + angley, 0.0, 0.0, 1.0);
    glScalef(Scal, Scal, Scal); // diminution de la vue de la scene
    glRotatef(180, 0.0, 1.0, 0.0);
    glRotatef(180, 1.0, 0.0, 0.0);
    glTranslatef(-trX, trY, 0.);
        glCallList(1); // appel de la liste numero 1
        glCallList(2);   // appel de la liste numero 2
        glCallList(3);   // appel de la liste numero 3
    glFlush();
    // On echange les buffers
    glutSwapBuffers();
}
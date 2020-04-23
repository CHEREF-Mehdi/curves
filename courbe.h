#include <GL/glut.h>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include "./armadillo/include/armadillo"

using namespace std;

struct Point
{
    float x, y;
    void set(float a, float b)
    {
        x = a;
        y = b;
    }
};

enum MenuEntry
{
    MENU_ADD_POINT = 0,
    MENU_MOVE_POINT = 1,
    MENU_CURVE_CatmullRom = 2,
    MENU_CURVE_BEZIER = 3,
    MENU_CURVE_BSPLINE_UNIF = 4,
    MENU_EXIT = 5
};

enum BsplineEntry
{
    SUB_MENU_CURVE_BSPLINE_NON_UNIF = 1,
    SUB_MENU_INCREASE_ORDER = 2
};

const int NMAX = 100;
Point P[NMAX];

const int winWidth = 800;
const int winHeight = 600;
const int smooth = 15;
char str[9];

arma::vec tVec = arma::linspace(0, 1, smooth);
arma::mat tMat = arma::join_horiz(pow(tVec, 3), pow(tVec, 2), tVec, arma::ones(smooth, 1));

//Bezier
arma::mat bezierMat = {{-1.0, 3.0, -3.0, 1.0},
                       {3.0, -6.0, 3.0, 0.0},
                       {-3.0, 3.0, 0.0, 0.0},
                       {1.0, 0.0, 0.0, 0.0}};
//Catmull Rom
float s = 1.0 / 2.0; //paramettre de tension
arma::mat catmullRomMat = {{-s, 2.0 - s, s - 2.0, s},
                           {2 * s, s - 3, 3 - 2 * s, -s},
                           {-s, 0.0, s, 0.0},
                           {0.0, 1.0, 0.0, 0.0}};
//BSpline Uniforme
arma::mat bSplineUnifMat = {{-1.0, 3.0, -3.0, 1.0},
                            {3.0, -6.0, 3.0, 0.0},
                            {-3.0, 0.0, 3.0, 0.0},
                            {1.0, 4.0, 1.0, 0.0}};

/**************************** curves functions ****************************/
//Bizier, cutmull Rom or Uniforme B-Spline
void drawPortion();
arma::mat getCtrlPoint(int i);
void drawCurve();

//Non Uniforme B-Spline
void resetVecNodal();
void upValVecNodal(int idx);
void downValVecNodal(int idx);
void drawVecNodal();
float bsplineRec(int index, int ordre, float t);
void bSplineNonUnif();
void text();

/**************************** draw points ****************************/
void drawPoint();

/**************************** display & reshape ****************************/
void display();
void reshape(int width, int height);

/**************************** mousse & keyboard functions ****************************/
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void keyboardSpecial(int key, int x, int y);

/**************************** menu functions  ****************************/
void mainMenu(int item);
void secondMenu(int item);
void createMenu();
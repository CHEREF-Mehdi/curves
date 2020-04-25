#include "curve.h"

int N = 0;
int mp = -1, addPoint = 0, inc = 3;
float coef = 1.0;
bool menuSelection = false;

arma::mat portion = arma::mat(smooth, 2);
arma::mat ctrlPoint = arma::mat(4, 2);
arma::mat cubicMat;
arma::vec vecteurNodal = arma::vec();

int selectedIndx = 0, ordre = 4;
bool showBezier = false, showCatmullRom = false, showBSplineUnif = false;
bool showBSplineNonUnif = false;

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Curves");
    glEnable(GL_DEPTH_TEST);

    createMenu();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);

    glutSpecialFunc(keyboardSpecial);

    glutPostRedisplay();
    glutSwapBuffers();
    glutMainLoop();

    return 0;
}

/**************************** display & reshape ****************************/
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
    drawPoint();
    if (showBezier)
    {
        cubicMat = bezierMat;
        inc = 3;
        coef = 1;
        glColor3f(0.0, 0.0, 1.0); //green
        drawCurve();
    }
    if (showCatmullRom)
    {
        cubicMat = catmullRomMat;
        inc = 1;
        coef = 1;
        glColor3f(0.0, 1.0, 1.0); //skyBlue
        drawCurve();
    }
    if (showBSplineUnif)
    {
        cubicMat = bSplineUnifMat;
        inc = 1;
        coef = 1.0 / 6.0;
        glColor3f(0.0, 1.0, 0.0); //blue
        drawCurve();
    }
    if (showBSplineNonUnif)
    {
        drawVecNodal();
        glColor3f(1.0, 0.0, 0.7); //purple
        text();
        bSplineNonUnif();
    }

    glutPostRedisplay();
    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -50.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**************************** curves functions ****************************/
//Bizier, cutmull Rom or Uniforme B-Spline
void drawPortion()
{
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < smooth; j++)
        glVertex2f(portion(j, 0), portion(j, 1));
    glEnd();
}

arma::mat getCtrlPoint(int i)
{
    return {
        {P[i - 3].x, P[i - 3].y},
        {P[i - 2].x, P[i - 2].y},
        {P[i - 1].x, P[i - 1].y},
        {P[i].x, P[i].y}};
}

void drawCurve()
{
    for (int i = 3; i < N; i += inc)
    {
        ctrlPoint = getCtrlPoint(i);
        portion = tMat * coef * cubicMat * ctrlPoint;
        drawPortion();
    }
}

//Non Uniforme B-Spline
void resetVecNodal()
{
    vecteurNodal.resize(ordre + N);
    vecteurNodal = round(arma::linspace(0, 100, ordre + N));
    selectedIndx = 0;
}

void upValVecNodal(int idx)
{
    if ((idx < N + ordre - 1))
    {
        if (vecteurNodal(idx) >= vecteurNodal(idx + 1))
            upValVecNodal(idx + 1);

        if (vecteurNodal(idx) < vecteurNodal(idx + 1))
            vecteurNodal[idx] += 1.0;
    }
    else if (vecteurNodal(idx) < 100)
        vecteurNodal[idx] += 1.0;
}

void downValVecNodal(int idx)
{
    if ((idx > 0))
    {
        if (vecteurNodal(idx) <= vecteurNodal(idx - 1))
            downValVecNodal(idx - 1);
        if (vecteurNodal(idx) > vecteurNodal(idx - 1))
            vecteurNodal[idx] -= 1.0;
    }
    else if (vecteurNodal(idx) > 0)
        vecteurNodal[idx] -= 1.0;
}

void drawVecNodal()
{
    glColor3f(1.0, 1.0, 0.0);
    if (N > 0)
    {
        glPointSize(6.0);
        for (int i = 0; i < ordre + N; i++)
        {
            glBegin(GL_POINTS);
            if (i == selectedIndx)
                glColor3f(0.0, 1.0, 0.0);
            else
                glColor3f(0.0, 0.0, 1.0);
            glVertex2f((glutGet(GLUT_WINDOW_WIDTH) - 40.0) * vecteurNodal(i) / 100.0 + 20, 40.0);
            glEnd();
        }
        //line
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(20.0, 40.0);
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH) - 20.0, 40.0);
        glEnd();
    }
}

float bsplineRec(int index, int ordre, float t)
{
    if (ordre == 1)
        return (vecteurNodal(index) <= t && t < vecteurNodal(index + 1)) ? 1.0 : 0.0;
    else
    {
        float part1, part2, denom1, denom2;

        denom1 = vecteurNodal(index + ordre - 1) - vecteurNodal(index);
        denom2 = vecteurNodal(index + ordre) - vecteurNodal(index + 1);

        part1 = (denom1 == 0) ? 0 : (t - vecteurNodal(index)) / denom1;
        part2 = (denom2 == 0) ? 0 : (vecteurNodal(index + ordre) - t) / denom2;

        return part1 * bsplineRec(index, ordre - 1, t) + part2 * bsplineRec(index + 1, ordre - 1, t);
    }
}

void bSplineNonUnif()
{
    ctrlPoint = arma::mat(ordre, 2);
    arma::rowvec valueBik = arma::rowvec(ordre);
    arma::rowvec res = arma::rowvec(2);
    for (int i = ordre - 1; i < N; i++)
    {
        for (int j = i - ordre + 1; j <= i; j++)
            ctrlPoint.row(j - i + ordre - 1) = arma::rowvec({P[j].x, P[j].y});

        glBegin(GL_LINE_STRIP);
        for (float t = vecteurNodal(i); t < vecteurNodal(i + 1); t += 0.005)
        {
            for (int j = ordre - 1; j >= 0; j--)
                valueBik.col(ordre - 1 - j) = bsplineRec(i - j, ordre, t);

            res = valueBik * ctrlPoint;
            glVertex2f(res(0), res(1));
        }
        glEnd();
    }
}

void text()
{
    str[0] = 'o';
    str[1] = 'r';
    str[2] = 'd';
    str[3] = 'r';
    str[4] = 'e';
    str[5] = ' ';
    str[6] = ':';
    str[7] = ' ';
    str[8] = '0' + ordre;
    for (int i = 0; i < 9; i++)
    {
        glRasterPos2f(winWidth - 100 + i * 10, winHeight - 20);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
}

/**************************** draw points ****************************/
void drawPoint()
{
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(5.0);
    glInitNames();
    glPushName(1);
    for (int i = 0; i < N; i++)
    {
        glLoadName(i);
        glBegin(GL_POINTS);
        glVertex2f(P[i].x, P[i].y);
        glEnd();
    }
}

/**************************** mousse & keyboard functions ****************************/
void Mouse(int button, int state, int x, int y)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    //deplacer des point
    if (button == GLUT_LEFT_BUTTON && addPoint == 0)
    {
        if (state == GLUT_DOWN)
        {
            GLuint *selectBuf = new GLuint[200];
            GLuint *ptr;
            GLint hits;

            glSelectBuffer(200, selectBuf);
            glRenderMode(GL_SELECT);

            glPushMatrix();
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPickMatrix(x - viewport[0], -viewport[1] + viewport[3] - y, 5.0, 5.0, viewport);
            glOrtho(0.0, viewport[2], 0.0, viewport[3], -50.0, 50.0);

            drawPoint();

            glPopMatrix();
            glFlush();

            hits = glRenderMode(GL_RENDER);
            if (hits)
            {
                ptr = (GLuint *)selectBuf;
                ptr += 3;
                mp = *ptr;
            }
        }

        if (state == GLUT_UP)
            mp = -1;

        reshape(viewport[2], viewport[3]);
        glutPostRedisplay();
    }

    //ajouter des point
    if (button == GLUT_LEFT_BUTTON && addPoint == 1 && state == GLUT_UP)
    {
        if (menuSelection)
            menuSelection = false;
        else
        {

            glColor3f(1.0, 1.0, 0.0);
            glPointSize(6.0);
            glPushName(1);

            P[N].x = x - viewport[0];
            P[N].y = -viewport[1] + viewport[3] - y;

            glLoadName(N);
            glBegin(GL_POINTS);
            glVertex2f(P[N].x, P[N].y);
            glEnd();

            N++;
            resetVecNodal();

            glutPostRedisplay();
        }
    }
}

void Motion(int x, int y)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (addPoint == 0 && (mp != -1))
    {
        P[mp].x = x - viewport[0];
        P[mp].y = viewport[3] - y - viewport[1];
        glutPostRedisplay();
    }
}

void keyboardSpecial(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        upValVecNodal(selectedIndx);
        break;
    case GLUT_KEY_DOWN:
        downValVecNodal(selectedIndx);
        break;
    case GLUT_KEY_LEFT:
        selectedIndx > 0 ? selectedIndx-- : selectedIndx = ordre + N - 1;
        break;
    case GLUT_KEY_RIGHT:
        selectedIndx < ordre + N - 1 ? selectedIndx++ : selectedIndx = 0;
        break;
    }
}

/**************************** menu functions  ****************************/
void mainMenu(int item)
{
    switch (item)
    {
    case MENU_ADD_POINT:
        menuSelection = true;
        addPoint = 1;
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        break;
    case MENU_MOVE_POINT:
        menuSelection = true;
        addPoint = 0;
        glutSetCursor(GLUT_CURSOR_UP_DOWN);
        break;
    case MENU_CURVE_CatmullRom:
        menuSelection = true;
        showCatmullRom = !showCatmullRom;
        break;
    case MENU_CURVE_BEZIER:
        menuSelection = true;
        showBezier = !showBezier;
        break;
    case MENU_CURVE_BSPLINE_UNIF:
        menuSelection = true;
        showBSplineUnif = !showBSplineUnif;
        showBSplineNonUnif = false;
        break;
    case MENU_EXIT:
        exit(0);
        break;
    }
}

void secondMenu(int item)
{
    switch (item)
    {
    case SUB_MENU_CURVE_BSPLINE_NON_UNIF:
        menuSelection = true;
        showBSplineNonUnif = !showBSplineNonUnif;
        showBSplineUnif = false;
        break;
    case SUB_MENU_INCREASE_ORDER:
        menuSelection = true;
        if (ordre <= N)
            ordre++;
        else
            ordre = 2;
        resetVecNodal();
        break;
    }
}

void createMenu()
{
    int menuBSpline = glutCreateMenu(secondMenu);
    glutAddMenuEntry("Show curve (purple)", SUB_MENU_CURVE_BSPLINE_NON_UNIF);
    glutAddMenuEntry("Increase order", SUB_MENU_INCREASE_ORDER);

    glutCreateMenu(mainMenu);
    glutAddMenuEntry("Add points", MENU_ADD_POINT);
    glutAddMenuEntry("Move points", MENU_MOVE_POINT);
    glutAddMenuEntry("Bezier curve (blue)", MENU_CURVE_BEZIER);
    glutAddMenuEntry("Catmull Rom curve (Sky blue) ", MENU_CURVE_CatmullRom);
    glutAddMenuEntry("Uniform B-Spline curve (green) ", MENU_CURVE_BSPLINE_UNIF);
    glutAddSubMenu("Non-Uniform B-Spline curve ", menuBSpline);
    glutAddMenuEntry("Quite", MENU_EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
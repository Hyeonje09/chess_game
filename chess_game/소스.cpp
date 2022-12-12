#include <windows.h>       
#include <iostream>
#include <string>
#include <math.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <vector>
#include <fstream>

using namespace std;

#define   PI   3.1415926

float   theta, phi;

float   camera_radius;

float   moveX, moveY;

int      cnt;

GLuint g_pawnID, g_kingID, g_bishopID, g_KnightID, g_queenID, g_rookID;
int temp = 0; //1 : king, 2 : queen, 3, 4 : bishop, 5, 6 : knight, 7, 8 : rook, 9 ~ 16 : pawn;

//********************* Material Data ****************//
typedef struct materialStruct {
    GLfloat   ambient[4];
    GLfloat   diffuse[4];
    GLfloat   specular[4];
    GLfloat   shininess;
}materialStruct;

materialStruct brassMaterials = {
   {0.33, 0.22, 0.03, 0.5},
   {0.78, 0.57, 0.11, 0.5},
   {0.99, 0.91, 0.81, 0.5},
   1.0
};

materialStruct whitePlasticMaterials = {
   {0.5, 0.5, 0.5, 0.0},
   {0.5, 0.5, 0.5, 0.0},
   {0.9, 0.9, 0.9, 0.0},
   5.0
};
materialStruct blackPlasticMaterials = {
   {0.0, 0.0, 0.0, 0.0},
   {0.0, 0.0, 0.0, 0.0},
   {0.1, 0.1, 0.1, 0.0},
   5.0
};
materialStruct redPlasticMaterials = {
   {0.3, 0.0, 0.0, 0.0},
   {0.6, 0.0, 0.0, 0.0},
   {0.8, 0.6, 0.6, 0.0},
   5.0
};
materialStruct greenPlasticMaterials = {
   {0.0, 0.3, 0.0, 0.0},
   {0.0, 0.6, 0.0, 0.0},
   {0.6, 0.8, 0.6, 0.0},
   5.0
};
materialStruct bluePlasticMaterials = {
   {0.0, 0.0, 0.3, 0.0},
   {0.0, 0.0, 0.6, 0.0},
   {0.6, 0.6, 0.8, 0.0},
   5.0
};
materialStruct yellowPlasticMaterials = {
   {0.3, 0.3, 0.0, 0.0},
   {0.6, 0.6, 0.0, 0.0},
   {0.8, 0.8, 0.8, 0.0},
   5.0
};

materialStruct whiteShineyMaterials = {
   {1.0, 1.0, 1.0, 1.0},
   {1.0, 1.0, 1.0, 1.0},
   {1.0, 1.0, 1.0, 1.0},
   10.0
};

//********************* Light Data ****************//
typedef struct lightingStruct {
    GLfloat   ambient[4];
    GLfloat   diffuse[4];
    GLfloat   specular[4];
    GLfloat   position[4];
    GLfloat cutoff;
    GLfloat   exponent;
}lightingStruct;

lightingStruct whiteLighting = {
   {0.0, 0.0, 0.0, 1.0},
   {1.0, 1.0, 1.0, 1.0},
   {1.0, 1.0, 1.0, 1.0},
   {0.0, 0.0, 10.0, 1.0},
   40.0,
   80.0
};
lightingStruct coloredLighting = {
   {1.0, 0.0, 0.0, 1.0},
   {1.0, 1.0, 0.0, 1.0},
   {1.0, 0.0, 0.0, 1.0},
   {0.0, 0.0, 5.0, 1.0},
   0.0,
   1.0
};

materialStruct* currentMaterials;
lightingStruct* currentLighting;

class CPoint2f {
public:
    vector <float> d{ 0, 0 };
};
class CPoint3f {
public:
    vector <float> d{ 0, 0, 0 };
};
class CPoint3i {
public:
    vector <int> d{ 0, 0, 0 };
};
class CFace {
public:
    vector <CPoint3i> v_pairs;
};
class CObj {
public:
    string name;
    vector <CPoint3f> v;
    vector <CPoint2f> vt;
    vector <CPoint3f> vn;
    vector <CFace> f;
};
class CModel {
public:
    vector <CObj> objs;

    vector <float> my_strtok_f(char* str, char* delimeter) {
        vector <float> v;
        char* context;
        char* tok = strtok_s(str, delimeter, &context);

        while (tok != NULL) {
            v.push_back(atof(tok));
            tok = strtok_s(context, delimeter, &context);
        }
        return v;
    }

    vector <string> my_strtok_s(char* str, char* delimeter) {
        vector <string> v;
        char* context;
        char* tok = strtok_s(str, delimeter, &context);

        while (tok != NULL) {
            v.push_back(tok);
            tok = strtok_s(context, delimeter, &context);
        }
        return v;
    }

    vector <int> my_strtok_i(char* str, char* delimeter) {
        vector <int> v;
        char* context;
        char* tok = strtok_s(str, delimeter, &context);

        while (tok != NULL) {
            v.push_back(atoi(tok));
            tok = strtok_s(context, delimeter, &context);
        }
        return v;
    }

    void loadObj(ifstream& fin) {
        string line;
        CObj obj_temp;
        int cnt = 0;

        while (getline(fin, line)) {
            int len = line.length();
            vector <float> vf;
            vector <string> s;
            vector <int> vi;

            CPoint3f p3;
            CPoint2f p2;
            CPoint3i p3i;

            if (line[0] == 'g' && line[1] == ' ') {
                obj_temp.name = line.substr(2, len - 2);
                objs.push_back(obj_temp);
                cnt += 1;
            }

            if (line[0] == 'v' && line[1] == ' ') {
                vf = my_strtok_f((char*)line.substr(2, len - 2).c_str(), (char*)" ");
                p3.d = { vf[0], vf[1], vf[2] };
                objs[cnt - 1].v.push_back(p3);
            }

            else if (line[0] == 'v' && line[1] == 't') {
                vf = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
                p2.d = { vf[0], vf[1] };
                objs[cnt - 1].vt.push_back(p2);
            }

            else if (line[0] == 'v' && line[1] == 'n') {
                vf = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
                p3.d = { vf[0], vf[1], vf[2] };
                objs[cnt - 1].vn.push_back(p3);
            }

            else if (line[0] == 'f' && line[1] == ' ') {
                s = my_strtok_s((char*)line.substr(2, len - 2).c_str(), (char*)" ");
                int nVertexes = s.size();
                CFace face_temp;

                for (int i = 0; i < nVertexes; i++) {
                    vi = my_strtok_i((char*)s[i].c_str(), (char*)"/");
                    p3i.d = { vi[0], vi[1], vi[2] };
                    face_temp.v_pairs.push_back(p3i);
                }
                objs[cnt - 1].f.push_back(face_temp);
            }
        }
    }
};

CModel a, b, c, d, e, f;

void light() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, currentMaterials->ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, currentMaterials->diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, currentMaterials->specular);
    glMaterialf(GL_FRONT, GL_SHININESS, currentMaterials->shininess);
}
void draw_chessboard_b() {
    currentMaterials = &blackPlasticMaterials;
    light();
    glutSolidCube(0.5);
}
void draw_chessboard_w() {
    currentMaterials = &whitePlasticMaterials;
    light();
    glutSolidCube(0.5);
}

void draw_move_Circle() {
    currentMaterials = &greenPlasticMaterials;
    glMaterialfv(GL_FRONT, GL_AMBIENT, currentMaterials->ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, currentMaterials->diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, currentMaterials->specular);
    glMaterialf(GL_FRONT, GL_SHININESS, currentMaterials->shininess);
    glutSolidSphere(0.1, 360, 360);
}

void init(void) {
    moveX = 0.0;
    moveY = -0.55;
    camera_radius = 6.0;

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    currentLighting = &coloredLighting;
    glLightfv(GL_LIGHT0, GL_AMBIENT, currentLighting->ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, currentLighting->diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, currentLighting->specular);
    glLightfv(GL_LIGHT0, GL_POSITION, currentLighting->position);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, currentLighting->cutoff);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, currentLighting->exponent);


    glEnable(GL_LIGHT1);
    currentLighting = &whiteLighting;
    glLightfv(GL_LIGHT1, GL_AMBIENT, currentLighting->ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, currentLighting->diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, currentLighting->specular);
    glLightfv(GL_LIGHT1, GL_POSITION, currentLighting->position);
}

GLfloat ratio;

void changeSize(int w, int h) {
    //창이 아주 작을 때, 0 으로 나누는 것을 예방
    if (h == 0)
        h = 1;

    ratio = 1.0 * w / h;

    //좌표계를 수정하기 전에 초기화
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //뷰포트를 창의 전체 크기로 설정합니다
    glViewport(0, 0, w, h);

    //투시값을 설정합니다.
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, -1.0, 0.0f, 1.0f, 0.0f);
}

void chessman(CModel m) {
    GLfloat cx, cy, cz, nx, ny, nz;
    int v_id, vt_id, vn_id;
    int nFaces;

    nFaces = m.objs[0].f.size();

    for (int k = 0; k < nFaces; k++) {
        int nPoints = m.objs[0].f[k].v_pairs.size();
        glBegin(GL_POLYGON);
        for (int i = 0; i < nPoints; i++) {
            v_id = m.objs[0].f[k].v_pairs[i].d[0];
            vt_id = m.objs[0].f[k].v_pairs[i].d[1];
            vn_id = m.objs[0].f[k].v_pairs[i].d[2];
            cx = m.objs[0].v[v_id - 1].d[0];
            cy = m.objs[0].v[v_id - 1].d[1];
            cz = m.objs[0].v[v_id - 1].d[2];

            nx = m.objs[0].vn[vn_id - 1].d[0];
            ny = m.objs[0].vn[vn_id - 1].d[1];
            nz = m.objs[0].vn[vn_id - 1].d[2];

            glNormal3f(nx, ny, nz);
            glVertex3f(cx, cy, cz);
        }
        glEnd();
    }
}

void cone() {
    currentMaterials = &redPlasticMaterials;
    light();
    glutSolidCone(0.1, 0.5, 10, 10);
}
void pawn() {
    g_pawnID = glGenLists(1);

    glNewList(g_pawnID, GL_COMPILE);

    string filepath;
    filepath = "./chess/Pawn.obj";

    ifstream fin(filepath);
    a.loadObj(fin);

    fin.close();

    chessman(a);

    glEndList();
}
void rook() {
    g_rookID = g_pawnID + 1;
    glNewList(g_rookID, GL_COMPILE);

    string filepath;
    filepath = "./chess/Rook.obj";

    ifstream fin(filepath);
    b.loadObj(fin);

    fin.close();

    chessman(b);

    glEndList();
}
void bishop() {
    g_bishopID = g_rookID + 1;
    glNewList(g_bishopID, GL_COMPILE);
    string filepath;
    filepath = "./chess/Bishop.obj";

    ifstream fin(filepath);
    c.loadObj(fin);

    fin.close();

    chessman(c);

    glEndList();
}
void Knight() {
    g_KnightID = g_bishopID + 1;
    glNewList(g_KnightID, GL_COMPILE);

    string filepath;
    filepath = "./chess/Knight.obj";

    ifstream fin(filepath);
    d.loadObj(fin);

    fin.close();

    chessman(d);

    glEndList();
}
void queen() {
    g_queenID = g_KnightID + 1;
    glNewList(g_queenID, GL_COMPILE);

    string filepath;
    filepath = "./chess/Queen.obj";

    ifstream fin(filepath);
    e.loadObj(fin);

    fin.close();

    chessman(e);

    glEndList();
}
void king() {
    g_kingID = g_queenID + 1;
    glNewList(g_kingID, GL_COMPILE);

    string filepath;
    filepath = "./chess/King.obj";

    ifstream fin(filepath);
    f.loadObj(fin);

    fin.close();

    chessman(f);

    glEndList();
}

struct coordinate {
    float x, y, z;
};

struct coordinate coneC = { 2, -2.0, -1.0 };
struct coordinate kingC = { 0.25, -0.5, -1.5 };
struct coordinate queenC = { 0.25, 0.0, -1.5 };
struct coordinate bishopC[2] = { {0.25, -1.0, -1.5}, {0.25, 0.5, -1.5} };
struct coordinate rookC[2] = { { 0.25, -2.0, -1.5 }, { 0.25, 1.5, -1.5 } };
struct coordinate knightC[2] = { {0.25, -1.5, -1.5}, {0.25, 1.0, -1.5} };
struct coordinate pawnC[8] = {
   { 0.25, -2.0, -1.0 }, { 0.25, -1.5, -1.0 }, { 0.25, -1.0, -1.0 }, { 0.25, 1.0, -1.0 },
   { 0.25, -0.5, -1.0 }, { 0.25, 0.0, -1.0 }, { 0.25, 0.5, -1.0 }, { 0.25, 1.5, -1.0 }
};

struct coordinate kingCircle_front, kingCircle_behind, kingCircle_left, kingCircle_right, kingCircle_left_u, kingCircle_left_d, kingCircle_right_u, kingCircle_right_d;

void movechs(float y, float z) {
    coneC.y = y;
    coneC.z = z;
}

int moving() {
    if (kingC.y == coneC.y && kingC.z == coneC.z) return 1;
    else if (queenC.y == coneC.y && queenC.z == coneC.z) return 2;
    else if (bishopC[0].y == coneC.y && bishopC[0].z == coneC.z) return 3;
    else if (bishopC[1].y == coneC.y && bishopC[1].z == coneC.z) return 4;
    else if (rookC[0].y == coneC.y && rookC[0].z == coneC.z) return 5;
    else if (rookC[1].y == coneC.y && rookC[1].z == coneC.z) return 6;
    else if (knightC[0].y == coneC.y && knightC[0].z == coneC.z) return 7;
    else if (knightC[1].y == coneC.y && knightC[1].z == coneC.z) return 8;
    else {
        if (pawnC[0].y == coneC.y && pawnC[0].z == coneC.z) return 9;
        else if (pawnC[1].y == coneC.y && pawnC[1].z == coneC.z) return 10;
        else if (pawnC[2].y == coneC.y && pawnC[2].z == coneC.z) return 11;
        else if (pawnC[3].y == coneC.y && pawnC[3].z == coneC.z) return 12;
        else if (pawnC[4].y == coneC.y && pawnC[4].z == coneC.z) return 13;
        else if (pawnC[5].y == coneC.y && pawnC[5].z == coneC.z) return 14;
        else if (pawnC[6].y == coneC.y && pawnC[6].z == coneC.z) return 15;
        else if (pawnC[7].y == coneC.y && pawnC[7].z == coneC.z) return 16;
    }
}

void chessman_move() {
    if (coneC.y == kingCircle_behind.y && coneC.z == kingCircle_behind.z) {
        movechs(kingCircle_behind.y, kingCircle_behind.z);
    }
    else if (coneC.y == kingCircle_front.y && coneC.z == kingCircle_front.z) {
        movechs(kingCircle_front.y, kingCircle_front.z);
    }
    else if (coneC.y == kingCircle_left.y && coneC.z == kingCircle_left.z) {
        movechs(kingCircle_left.y, kingCircle_left.z);
    }
    else if (coneC.y == kingCircle_right.y && coneC.z == kingCircle_right.z) {
        movechs(kingCircle_right.y, kingCircle_right.z);
    }
    else if (coneC.y == kingCircle_right_u.y && coneC.z == kingCircle_right_u.z) {
        movechs(kingCircle_right_u.y, kingCircle_right_u.z);
    }
    else if (coneC.y == kingCircle_right_d.y && coneC.z == kingCircle_right_d.z) {
        movechs(kingCircle_right_d.y, kingCircle_right_d.z);
    }
    else if (coneC.y == kingCircle_left_u.y && coneC.z == kingCircle_left_u.z) {
        movechs(kingCircle_left_u.y, kingCircle_left_u.z);
    }
    else if (coneC.y == kingCircle_left_d.y && coneC.z == kingCircle_left_d.z) {
        movechs(kingCircle_left_d.y, kingCircle_left_d.z);
    }
    else {
        movechs(kingC.y, kingC.z);
    }
}

void move_King_Circle() {
    float a = 0.5;
    kingCircle_front = { a, kingC.y, kingC.z + a };
    kingCircle_behind = { a, kingC.y, kingC.z - a };
    kingCircle_left = { a, kingC.y - a, kingC.z };
    kingCircle_right = { a, kingC.y + a, kingC.z };
    kingCircle_left_u = { a, kingC.y - a, kingC.z + a };
    kingCircle_left_d = { a, kingC.y - a, kingC.z - a };
    kingCircle_right_u = { a, kingC.y + a, kingC.z + a };
    kingCircle_right_d = { a, kingC.y + a, kingC.z - a };


    if (kingCircle_right.y <= 1.5) {
        glPushMatrix();
        glTranslatef(kingCircle_right.x, kingCircle_right.y, kingCircle_right.z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (kingCircle_left.y >= -2) {
        glPushMatrix();
        glTranslatef(kingCircle_left.x, kingCircle_left.y, kingCircle_left.z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (kingCircle_front.z <= 2) {
        glPushMatrix();
        glTranslatef(kingCircle_front.x, kingCircle_front.y, kingCircle_front.z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (kingCircle_behind.z >= -1.5) {
        glPushMatrix();
        glTranslatef(kingCircle_behind.x, kingCircle_behind.y, kingCircle_behind.z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (kingCircle_left_d.y >= -2 && kingCircle_left_d.z >= -1.5) {
        glPushMatrix();
        glTranslatef(kingCircle_left_d.x, kingCircle_left_d.y, kingCircle_left_d.z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (kingCircle_right_d.y <= 1.5 && kingCircle_right_d.z >= -1.5) {
        glPushMatrix();
        glTranslatef(kingCircle_right_d.x, kingCircle_right_d.y, kingCircle_right_d.z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (kingCircle_left_u.y >= -2 && kingCircle_left_u.z <= 2) {
        glPushMatrix();
        glTranslatef(kingCircle_left_u.x, kingCircle_left_u.y, kingCircle_left_u.z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (kingCircle_right_u.y <= 1.5 && kingCircle_right_u.z <= 2) {
        glPushMatrix();
        glTranslatef(kingCircle_right_u.x, kingCircle_right_u.y, kingCircle_right_u.z);
        draw_move_Circle();
        glPopMatrix();
    }
}

void move_Rook_Circle(int x) {
    for (float i = 0.5; rookC[x].y + i <= 1.5; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, rookC[x].y + i, rookC[x].z);
        draw_move_Circle();
        glPopMatrix();
    }

    for (float i = 0.5; rookC[x].y - i >= -2; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, rookC[x].y - i, rookC[x].z);
        draw_move_Circle();
        glPopMatrix();
    }
    for (float i = 0.5; rookC[x].z + i <= 2; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, rookC[x].y, rookC[x].z + i);
        draw_move_Circle();
        glPopMatrix();
    }

    for (float i = 0.5; rookC[x].z - i >= -1.5; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, rookC[x].y, rookC[x].z - i);
        draw_move_Circle();
        glPopMatrix();
    }
}
void move_Pawn_Circle(int x) {
    if (pawnC[x].y + 0.5 <= 1.5) {
        glPushMatrix();
        glTranslatef(0.5, pawnC[x].y + 0.5, pawnC[x].z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (pawnC[x].y - 0.5 >= -2) {
        glPushMatrix();
        glTranslatef(0.5, pawnC[x].y - 0.5, pawnC[x].z);
        draw_move_Circle();
        glPopMatrix();
    }
    if (pawnC[x].z + 0.5 <= 2) {
        glPushMatrix();
        glTranslatef(0.5, pawnC[x].y, pawnC[x].z + 0.5);
        draw_move_Circle();
        glPopMatrix();
    }
    if (pawnC[x].z - 0.5 >= -1.5) {
        glPushMatrix();
        glTranslatef(0.5, pawnC[x].y, pawnC[x].z - 0.5);
        draw_move_Circle();
        glPopMatrix();
    }
}
void move_Queen_Circle() {
    for (float i = 0.5; queenC.y + i <= 1.5; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y + i, queenC.z);
        draw_move_Circle();
        glPopMatrix();
    }
    for (float i = 0.5; queenC.y - i >= -2; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y - i, queenC.z);
        draw_move_Circle();
        glPopMatrix();
    }
    for (float i = 0.5; queenC.z + i <= 2; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y, queenC.z + i);
        draw_move_Circle();
        glPopMatrix();
    }
    for (float i = 0.5; queenC.z - i >= -1.5; i += 0.5) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y, queenC.z - i);
        draw_move_Circle();
        glPopMatrix();
    }
    if (queenC.y - 0.5 >= -2 && queenC.z - 0.5 >= -1.5) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y - 0.5, queenC.z - 0.5);
        draw_move_Circle();
        glPopMatrix();
    }
    if (queenC.y + 0.5 <= 1.5 && queenC.z - 0.5 >= -1.5) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y + 0.5, queenC.z - 0.5);
        draw_move_Circle();
        glPopMatrix();
    }
    if (queenC.y - 0.5 >= -2 && queenC.z + 0.5 <= 2) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y - 0.5, queenC.z + 0.5);
        draw_move_Circle();
        glPopMatrix();
    }
    if (queenC.y + 0.5 <= 1.5 && queenC.z + 0.5 <= 2) {
        glPushMatrix();
        glTranslatef(0.5, queenC.y + 0.5, queenC.z + 0.5);
        draw_move_Circle();
        glPopMatrix();
    }
}

void cone_move() {
    glPushMatrix();
    glTranslatef(coneC.x, coneC.y, coneC.z);
    glRotatef(90.0, 0.0, -90.0, 0.0);
    cone();
    glPopMatrix();
}
void pawn_move() {
    glPushMatrix();
    glTranslatef(pawnC[0].x, pawnC[0].y, pawnC[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnC[1].x, pawnC[1].y, pawnC[1].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnC[2].x, pawnC[2].y, pawnC[2].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnC[3].x, pawnC[3].y, pawnC[3].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnC[4].x, pawnC[4].y, pawnC[4].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnC[5].x, pawnC[5].y, pawnC[5].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnC[6].x, pawnC[6].y, pawnC[6].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnC[7].x, pawnC[7].y, pawnC[7].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();
}
void king_move() {
    glPushMatrix();
    glTranslatef(kingC.x, kingC.y, kingC.z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_kingID);
    glPopMatrix();
}
void queen_move() {
    glPushMatrix();
    glTranslatef(queenC.x, queenC.y, queenC.z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_queenID);
    glPopMatrix();
}
void rook_move() {
    glPushMatrix();
    glTranslatef(rookC[0].x, rookC[0].y, rookC[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_rookID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rookC[1].x, rookC[1].y, rookC[1].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_rookID);
    glPopMatrix();
}
void bishop_move() {
    glPushMatrix();
    glTranslatef(bishopC[0].x, bishopC[0].y, bishopC[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_bishopID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bishopC[1].x, bishopC[1].y, bishopC[1].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_bishopID);
    glPopMatrix();
}
void knight_move() {
    glPushMatrix();
    glTranslatef(knightC[0].x, knightC[0].y, knightC[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_KnightID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(knightC[1].x, knightC[1].y, knightC[1].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_KnightID);
    glPopMatrix();
}

void display(void) {
    float   x, y, z;

    glClearColor(0.7, 0.9, 0.96, 0.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    x = camera_radius * cos(moveY) * cos(moveX);
    y = camera_radius * cos(moveY) * sin(moveX);
    z = camera_radius * sin(moveY);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            glPushMatrix();
            glTranslatef(0.0, j - 2.0, i - 1.5);
            draw_chessboard_b();
            glPopMatrix();
        }

        for (int j = 0; j < 4; j++) {
            glPushMatrix();
            glTranslatef(0.0, j - 1.5, i - 1.0);
            draw_chessboard_b();
            glPopMatrix();
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            glPushMatrix();
            glTranslatef(0.0, j - 1.5, i - 1.5);
            draw_chessboard_w();
            glPopMatrix();
        }

        for (int j = 0; j < 4; j++) {
            glPushMatrix();
            glTranslatef(0.0, j - 2.0, i - 1.0);
            draw_chessboard_w();
            glPopMatrix();
        }

    }

    cone_move();

    pawn_move();

    rook_move();

    bishop_move();

    knight_move();

    queen_move();

    king_move();

    if (temp == 1) {
        move_King_Circle();
    }
    else if (temp == 2) {
        move_Queen_Circle();
    }
    else if (temp == 5) {
        move_Rook_Circle(0);
    }
    else if (temp == 6) {
        move_Rook_Circle(1);
    }
    else if (temp == 9) {
        move_Pawn_Circle(0);
    }
    else if (temp == 10) {
        move_Pawn_Circle(1);
    }
    else if (temp == 11) {
        move_Pawn_Circle(2);
    }
    else if (temp == 12) {
        move_Pawn_Circle(3);
    }
    else if (temp == 13) {
        move_Pawn_Circle(4);
    }
    else if (temp == 14) {
        move_Pawn_Circle(5);
    }
    else if (temp == 15) {
        move_Pawn_Circle(6);
    }
    else if (temp == 16) {
        move_Pawn_Circle(7);
    }

    glFlush();
    glutSwapBuffers();
}


void SpecialKey(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:      moveX -= 0.01;
        break;
    case GLUT_KEY_RIGHT:   moveX += 0.01;
        break;
    case GLUT_KEY_UP:      moveY += 0.01;
        break;
    case GLUT_KEY_DOWN:      moveY -= 0.01;
        break;

    case GLUT_KEY_F1:
        cout << "움직임 여부 : " << moving() << endl;
        if (moving() == 1) {
            kingC.x += 0.25;
            temp = 1;
        }
        else if (moving() == 2) {
            queenC.x += 0.25;
            temp = 2;
        }
        else if (moving() == 3) {
            bishopC[0].x += 0.25;
            temp = 3;
        }
        else if (moving() == 4) {
            bishopC[1].x += 0.25;
            temp = 4;
        }
        else if (moving() == 5) {
            rookC[0].x += 0.25;
            temp = 5;
        }
        else if (moving() == 6) {
            rookC[1].x += 0.25;
            temp = 6;
        }
        else if (moving() == 7) {
            knightC[0].x += 0.25;
            temp = 7;
        }
        else if (moving() == 8) {
            knightC[1].x += 0.25;
            temp = 8;
        }
        else if (moving() == 9) {
            pawnC[0].x += 0.25;
            temp = 9;
        }
        else if (moving() == 10) {
            pawnC[1].x += 0.25;
            temp = 10;
        }
        else if (moving() == 11) {
            pawnC[2].x += 0.25;
            temp = 11;
        }
        else if (moving() == 12) {
            pawnC[3].x += 0.25;
            temp = 12;
        }
        else if (moving() == 13) {
            pawnC[4].x += 0.25;
            temp = 13;
        }
        else if (moving() == 14) {
            pawnC[5].x += 0.25;
            temp = 14;
        }
        else if (moving() == 15) {
            pawnC[6].x += 0.25;
            temp = 15;
        }
        else if (moving() == 16) {
            pawnC[7].x += 0.25;
            temp = 16;
        }
        break;

    case GLUT_KEY_F2:
        if (temp == 1) {
            cout << "현재 : " << temp << endl;
            chessman_move();
            kingC.x -= 0.25;
            kingC.y = coneC.y;
            kingC.z = coneC.z;
            temp = 0;
            cout << "처리 후 : " << temp << endl;
        }
        else if (temp == 2) {
            queenC.x -= 0.25;
            queenC.y = coneC.y;
            queenC.z = coneC.z;
            temp = 0;
        }
        else if (temp == 3) {
            bishopC[0].x -= 0.25;
            bishopC[0].y = coneC.y;
            bishopC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 4) {
            bishopC[1].x -= 0.25;
            bishopC[1].y = coneC.y;
            bishopC[1].z = coneC.y;
            temp = 0;
        }
        else if (temp == 5) {
            rookC[0].x -= 0.25;
            rookC[0].y = coneC.y;
            rookC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 6) {
            rookC[1].x -= 0.25;
            rookC[1].y = coneC.y;
            rookC[1].z = coneC.z;
            temp = 0;
        }
        else if (temp == 7) {
            knightC[0].x -= 0.25;
            knightC[0].y = coneC.y;
            knightC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 8) {
            knightC[1].x -= 0.25;
            knightC[1].y = coneC.y;
            knightC[1].z = coneC.z;
            temp = 0;
        }
        else if (temp == 9) {
            pawnC[0].x -= 0.25;
            pawnC[0].y = coneC.y;
            pawnC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 10) {
            pawnC[1].x -= 0.25;
            pawnC[1].y = coneC.y;
            pawnC[1].z = coneC.z;
            temp = 0;
        }
        else if (temp == 11) {
            pawnC[2].x -= 0.25;
            pawnC[2].y = coneC.y;
            pawnC[2].z = coneC.z;
            temp = 0;
        }
        else if (temp == 12) {
            pawnC[3].x -= 0.25;
            pawnC[3].y = coneC.y;
            pawnC[3].z = coneC.z;
            temp = 0;
        }
        else if (temp == 13) {
            pawnC[4].x -= 0.25;
            pawnC[4].y = coneC.y;
            pawnC[4].z = coneC.z;
            temp = 0;
        }
        else if (temp == 14) {
            pawnC[5].x -= 0.25;
            pawnC[5].y = coneC.y;
            pawnC[5].z = coneC.z;
            temp = 0;
        }
        else if (temp == 15) {
            pawnC[6].x -= 0.25;
            pawnC[6].y = coneC.y;
            pawnC[6].z = coneC.z;
            temp = 0;
        }
        else if (temp == 16) {
            pawnC[7].x -= 0.25;
            pawnC[7].y = coneC.y;
            pawnC[7].z = coneC.z;
            temp = 0;
        }

        break;

    default:            break;
    }

    if (moveX > 2.0 * PI)
        moveX -= (2.0 * PI);
    else if (theta < 0.0)
        moveX += (2.0 * PI);

    glutPostRedisplay();
}

void MyKey(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':   coneC.z += 0.5;
        break;
    case 's':   coneC.z -= 0.5;
        break;
    case 'a':   coneC.y -= 0.5;
        break;
    case 'd':   coneC.y += 0.5;
        break;

    default: break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("");

    init();

    cone();
    pawn();
    rook();
    bishop();
    Knight();
    queen();
    king();

    glutDisplayFunc(display);
    glutSpecialFunc(SpecialKey);
    glutKeyboardFunc(MyKey);
    glutReshapeFunc(changeSize);
    glutIdleFunc(display);
    glutMainLoop();
}
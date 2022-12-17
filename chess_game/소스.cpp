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

int      cnt, turn;

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
    GLfloat   cutoff;
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
    light();
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
        currentMaterials = &blackPlasticMaterials;
        light();
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
    if (turn == 0) {
        currentMaterials = &redPlasticMaterials;
    }
    else {
        currentMaterials = &bluePlasticMaterials;
    }
    light();
    glutSolidCone(0.1, 0.5, 10, 10);
}
void pawn() {
    currentMaterials = &blackPlasticMaterials;
    light();
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
struct coordinate coneD = { 2, -2.0, 1.5 };

struct coordinate kingC = { 0.25, -0.5, -1.5 };
struct coordinate queenC = { 0.25, 0.0, -1.5 };
struct coordinate bishopC[2] = { {0.25, -1.0, -1.5}, {0.25, 0.5, -1.5} };
struct coordinate rookC[2] = { { 0.25, -2.0, -1.5 }, { 0.25, 1.5, -1.5 } };
struct coordinate knightC[2] = { {0.25, -1.5, -1.5}, {0.25, 1.0, -1.5} };
struct coordinate pawnC[8] = {
   { 0.25, -2.0, -1.0 }, { 0.25, -1.5, -1.0 }, { 0.25, -1.0, -1.0 }, { 0.25, 1.0, -1.0 },
   { 0.25, -0.5, -1.0 }, { 0.25, 0.0, -1.0 }, { 0.25, 0.5, -1.0 }, { 0.25, 1.5, -1.0 }
};

struct coordinate kingD = { 0.25, -0.5, 2.0 };
struct coordinate queenD = { 0.25, 0.0, 2.0 };
struct coordinate bishopD[2] = { {0.25, -1.0, 2.0}, {0.25, 0.5, 2.0} };
struct coordinate rookD[2] = { { 0.25, -2.0, 2.0 }, { 0.25, 1.5, 2.0 } };
struct coordinate knightD[2] = { {0.25, -1.5, 2.0}, {0.25, 1.0, 2.0} };
struct coordinate pawnD[8] = {
   { 0.25, -2.0, 1.5 }, { 0.25, -1.5, 1.5 }, { 0.25, -1.0, 1.5 }, { 0.25, 1.0, 1.5 },
   { 0.25, -0.5, 1.5}, { 0.25, 0.0, 1.5}, { 0.25, 0.5, 1.5 }, { 0.25, 1.5, 1.5 }
};

struct coordinate kingCircle_front, kingCircle_behind, kingCircle_left, kingCircle_right, kingCircle_left_u, kingCircle_left_d, kingCircle_right_u, kingCircle_right_d;
struct coordinate rookCircle_front[8], rookCircle_right[8], rookCircle_left[8], rookCircle_behind[8];
struct coordinate pawnCircle_front[2], pawnCircle_right, pawnCircle_left, pawnCircle_front_r, pawnCircle_front_l;
struct coordinate queenCircle_front[8], queenCircle_right[8], queenCircle_left[8], queenCircle_behind[8], queenCircle_left_u[8], queenCircle_left_d[8], queenCircle_right_u[8], queenCircle_right_d[8];
struct coordinate bishopCircle_left_u[8], bishopCircle_left_d[8], bishopCircle_right_u[8], bishopCircle_right_d[8];
struct coordinate knightCircle_front_l, knightCircle_front_r, knightCircle_left_u, knightCircle_left_d, knightCircle_right_u, knightCircle_right_d, knightCircle_behind_r, knightCircle_behind_l;
int pawn_moved_W[8], pawn_moved_B[8];

void movechs(float y, float z) {
    if (turn == 0) {
        coneC.y = y;
        coneC.z = z;

        if (y == pawnD[0].y && z == pawnD[0].z) {
            pawnD[0].y = 100;
            pawnD[0].z = 100;
        }

        else if (y == pawnD[1].y && z == pawnD[1].z) {
            pawnD[1].y = 100;
            pawnD[1].z = 100;
        }

        else if (y == pawnD[2].y && z == pawnD[2].z) {
            pawnD[2].y = 100;
            pawnD[2].z = 100;
        }

        else if (y == pawnD[3].y && z == pawnD[3].z) {
            pawnD[3].y = 100;
            pawnD[3].z = 100;
        }

        else if (y == pawnD[4].y && z == pawnD[4].z) {
            pawnD[4].y = 100;
            pawnD[4].z = 100;
        }

        else if (y == pawnD[5].y && z == pawnD[5].z) {
            pawnD[5].y = 100;
            pawnD[5].z = 100;
        }

        else if (y == pawnD[6].y && z == pawnD[6].z) {
            pawnD[6].y = 100;
            pawnD[6].z = 100;
        }

        else if (y == pawnD[7].y && z == pawnD[7].z) {
            pawnD[7].y = 100;
            pawnD[7].z = 100;
        }

        else if (y == kingD.y && z == kingD.z) {
            kingD.y = 100;
            kingD.z = 100;
        }

        else if (y == queenD.y && z == queenD.z) {
            queenD.y = 100;
            queenD.z = 100;
        }

        else if (y == bishopD[0].y && z == bishopD[0].z) {
            bishopD[0].y = 100;
            bishopD[0].z = 100;
        }

        else if (y == bishopD[1].y && z == bishopD[1].z) {
            bishopD[1].y = 100;
            bishopD[1].z = 100;
        }

        else if (y == knightD[0].y && z == knightD[0].z) {
            knightD[0].y = 100;
            knightD[0].z = 100;
        }

        else if (y == knightD[1].y && z == knightD[1].z) {
            knightD[1].y = 100;
            knightD[1].z = 100;
        }

        else if (y == rookD[0].y && z == rookD[0].z) {
            rookD[0].y = 100;
            rookD[0].z = 100;
        }

        else if (y == rookD[1].y && z == rookD[1].z) {
            rookD[1].y = 100;
            rookD[1].z = 100;
        }
    }

    else {
        coneD.y = y;
        coneD.z = z;

        if (y == pawnC[0].y && z == pawnC[0].z) {
            pawnC[0].y = 100;
            pawnC[0].z = 100;
        }

        else if (y == pawnC[1].y && z == pawnC[1].z) {
            pawnC[1].y = 100;
            pawnC[1].z = 100;
        }

        else if (y == pawnC[2].y && z == pawnC[2].z) {
            pawnC[2].y = 100;
            pawnC[2].z = 100;
        }

        else if (y == pawnC[3].y && z == pawnC[3].z) {
            pawnC[3].y = 100;
            pawnC[3].z = 100;
        }

        else if (y == pawnC[4].y && z == pawnC[4].z) {
            pawnC[4].y = 100;
            pawnC[4].z = 100;
        }

        else if (y == pawnC[5].y && z == pawnC[5].z) {
            pawnC[5].y = 100;
            pawnC[5].z = 100;
        }

        else if (y == pawnC[6].y && z == pawnC[6].z) {
            pawnC[6].y = 100;
            pawnC[6].z = 100;
        }

        else if (y == pawnC[7].y && z == pawnC[7].z) {
            pawnC[7].y = 100;
            pawnC[7].z = 100;
        }

        else if (y == kingC.y && z == kingC.z) {
            kingC.y = 100;
            kingC.z = 100;
        }

        else if (y == queenC.y && z == queenC.z) {
            queenC.y = 100;
            queenC.z = 100;
        }

        else if (y == bishopC[0].y && z == bishopC[0].z) {
            bishopC[0].y = 100;
            bishopC[0].z = 100;
        }

        else if (y == bishopC[1].y && z == bishopC[1].z) {
            bishopC[1].y = 100;
            bishopC[1].z = 100;
        }

        else if (y == knightC[0].y && z == knightC[0].z) {
            knightC[0].y = 100;
            knightC[0].z = 100;
        }

        else if (y == knightC[1].y && z == knightC[1].z) {
            knightC[1].y = 100;
            knightC[1].z = 100;
        }

        else if (y == rookC[0].y && z == rookC[0].z) {
            rookC[0].y = 100;
            rookC[0].z = 100;
        }

        else if (y == rookC[1].y && z == rookC[1].z) {
            rookC[1].y = 100;
            rookC[1].z = 100;
        }
    }
}

int moving() {
    if (turn == 0) {
        if (kingC.y == coneC.y && kingC.z == coneC.z) return 1;
        else if (queenC.y == coneC.y && queenC.z == coneC.z) return 2;
        else if (bishopC[0].y == coneC.y && bishopC[0].z == coneC.z) return 3;
        else if (bishopC[1].y == coneC.y && bishopC[1].z == coneC.z) return 4;
        else if (rookC[0].y == coneC.y && rookC[0].z == coneC.z) return 5;
        else if (rookC[1].y == coneC.y && rookC[1].z == coneC.z) return 6;
        else if (knightC[0].y == coneC.y && knightC[0].z == coneC.z) return 7;
        else if (knightC[1].y == coneC.y && knightC[1].z == coneC.z) return 8;
        else if (pawnC[0].y == coneC.y && pawnC[0].z == coneC.z) return 9;
        else if (pawnC[1].y == coneC.y && pawnC[1].z == coneC.z) return 10;
        else if (pawnC[2].y == coneC.y && pawnC[2].z == coneC.z) return 11;
        else if (pawnC[3].y == coneC.y && pawnC[3].z == coneC.z) return 12;
        else if (pawnC[4].y == coneC.y && pawnC[4].z == coneC.z) return 13;
        else if (pawnC[5].y == coneC.y && pawnC[5].z == coneC.z) return 14;
        else if (pawnC[6].y == coneC.y && pawnC[6].z == coneC.z) return 15;
        else if (pawnC[7].y == coneC.y && pawnC[7].z == coneC.z) return 16;
    }
    else {
        if (kingD.y == coneD.y && kingD.z == coneD.z) return 17;
        else if (queenD.y == coneD.y && queenD.z == coneD.z) return 18;
        else if (bishopD[0].y == coneD.y && bishopD[0].z == coneD.z) return 19;
        else if (bishopD[1].y == coneD.y && bishopD[1].z == coneD.z) return 20;
        else if (rookD[0].y == coneD.y && rookD[0].z == coneD.z) return 21;
        else if (rookD[1].y == coneD.y && rookD[1].z == coneD.z) return 22;
        else if (knightD[0].y == coneD.y && knightD[0].z == coneD.z) return 23;
        else if (knightD[1].y == coneD.y && knightD[1].z == coneD.z) return 24;
        else if (pawnD[0].y == coneD.y && pawnD[0].z == coneD.z) return 25;
        else if (pawnD[1].y == coneD.y && pawnD[1].z == coneD.z) return 26;
        else if (pawnD[2].y == coneD.y && pawnD[2].z == coneD.z) return 27;
        else if (pawnD[3].y == coneD.y && pawnD[3].z == coneD.z) return 28;
        else if (pawnD[4].y == coneD.y && pawnD[4].z == coneD.z) return 29;
        else if (pawnD[5].y == coneD.y && pawnD[5].z == coneD.z) return 30;
        else if (pawnD[6].y == coneD.y && pawnD[6].z == coneD.z) return 31;
        else if (pawnD[7].y == coneD.y && pawnD[7].z == coneD.z) return 32;
    }
}

int pawnchk = 0;
void chessman_move(int temp) {
    if (turn == 0) {
        if (temp == 1) {
            if (coneC.y == kingCircle_behind.y && coneC.z == kingCircle_behind.z) {
                movechs(kingCircle_behind.y, kingCircle_behind.z); turn = 1;
            }
            else if (coneC.y == kingCircle_front.y && coneC.z == kingCircle_front.z) {
                movechs(kingCircle_front.y, kingCircle_front.z); turn = 1;
            }
            else if (coneC.y == kingCircle_left.y && coneC.z == kingCircle_left.z) {
                movechs(kingCircle_left.y, kingCircle_left.z); turn = 1;
            }
            else if (coneC.y == kingCircle_right.y && coneC.z == kingCircle_right.z) {
                movechs(kingCircle_right.y, kingCircle_right.z); turn = 1;
            }
            else if (coneC.y == kingCircle_right_u.y && coneC.z == kingCircle_right_u.z) {
                movechs(kingCircle_right_u.y, kingCircle_right_u.z); turn = 1;
            }
            else if (coneC.y == kingCircle_right_d.y && coneC.z == kingCircle_right_d.z) {
                movechs(kingCircle_right_d.y, kingCircle_right_d.z); turn = 1;
            }
            else if (coneC.y == kingCircle_left_u.y && coneC.z == kingCircle_left_u.z) {
                movechs(kingCircle_left_u.y, kingCircle_left_u.z); turn = 1;
            }
            else if (coneC.y == kingCircle_left_d.y && coneC.z == kingCircle_left_d.z) {
                movechs(kingCircle_left_d.y, kingCircle_left_d.z); turn = 1;
            }
            else {
                movechs(kingC.y, kingC.z);
            }
        }
        else if (temp == 2) {
            for (int i = 0; i < 8; i++) {
                if (coneC.y == queenCircle_front[i].y && coneC.z == queenCircle_front[i].z) {
                    movechs(queenCircle_front[i].y, queenCircle_front[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == queenCircle_right[i].y && coneC.z == queenCircle_right[i].z) {
                    movechs(queenCircle_right[i].y, queenCircle_right[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == queenCircle_left[i].y && coneC.z == queenCircle_left[i].z) {
                    movechs(queenCircle_left[i].y, queenCircle_left[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == queenCircle_behind[i].y && coneC.z == queenCircle_behind[i].z) {
                    movechs(queenCircle_behind[i].y, queenCircle_behind[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == queenCircle_left_u[i].y && coneC.z == queenCircle_left_u[i].z) {
                    movechs(queenCircle_left_u[i].y, queenCircle_left_u[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == queenCircle_left_d[i].y && coneC.z == queenCircle_left_d[i].z) {
                    movechs(queenCircle_left_d[i].y, queenCircle_left_d[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == queenCircle_right_u[i].y && coneC.z == queenCircle_right_u[i].z) {
                    movechs(queenCircle_right_u[i].y, queenCircle_right_u[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == queenCircle_right_d[i].y && coneC.z == queenCircle_right_d[i].z) {
                    movechs(queenCircle_right_d[i].y, queenCircle_right_d[i].z); turn = 1;
                    break;
                }
                else if (i == 7) {
                    movechs(queenC.y, queenC.z);
                }
            }
        }
        else if (temp == 3 || temp == 4) { // 비숍
            for (int i = 0; i < 8; i++) {
                if (coneC.y == bishopCircle_left_u[i].y && coneC.z == bishopCircle_left_u[i].z) {
                    movechs(bishopCircle_left_u[i].y, bishopCircle_left_u[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == bishopCircle_left_d[i].y && coneC.z == bishopCircle_left_d[i].z) {
                    movechs(bishopCircle_left_d[i].y, bishopCircle_left_d[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == bishopCircle_right_u[i].y && coneC.z == bishopCircle_right_u[i].z) {
                    movechs(bishopCircle_right_u[i].y, bishopCircle_right_u[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == bishopCircle_right_d[i].y && coneC.z == bishopCircle_right_d[i].z) {
                    movechs(bishopCircle_right_d[i].y, bishopCircle_right_d[i].z); turn = 1;
                    break;
                }
                else if (i == 7) {
                    movechs(bishopC[temp % 3].y, bishopC[temp % 3].z);
                }
            }
        }
        else if (temp == 5 || temp == 6) { // 룩
            for (int i = 0; i < 8; i++) {
                if (coneC.y == rookCircle_front[i].y && coneC.z == rookCircle_front[i].z) {
                    movechs(rookCircle_front[i].y, rookCircle_front[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == rookCircle_right[i].y && coneC.z == rookCircle_right[i].z) {
                    movechs(rookCircle_right[i].y, rookCircle_right[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == rookCircle_left[i].y && coneC.z == rookCircle_left[i].z) {
                    movechs(rookCircle_left[i].y, rookCircle_left[i].z); turn = 1;
                    break;
                }
                else if (coneC.y == rookCircle_behind[i].y && coneC.z == rookCircle_behind[i].z) {
                    movechs(rookCircle_behind[i].y, rookCircle_behind[i].z); turn = 1;
                    break;
                }
                else if (i == 7) {
                    movechs(rookC[temp % 5].y, rookC[temp % 5].z);
                }
            }
        }
        else if (temp == 7 || temp == 8) { // 나이트
            if (coneC.y == knightCircle_front_l.y && coneC.z == knightCircle_front_l.z) {
                movechs(knightCircle_front_l.y, knightCircle_front_l.z); turn = 1;
            }
            else if (coneC.y == knightCircle_front_r.y && coneC.z == knightCircle_front_r.z) {
                movechs(knightCircle_front_r.y, knightCircle_front_r.z); turn = 1;
            }
            else if (coneC.y == knightCircle_left_u.y && coneC.z == knightCircle_left_u.z) {
                movechs(knightCircle_left_u.y, knightCircle_left_u.z); turn = 1;
            }
            else if (coneC.y == knightCircle_left_d.y && coneC.z == knightCircle_left_d.z) {
                movechs(knightCircle_left_d.y, knightCircle_left_d.z); turn = 1;
            }
            else if (coneC.y == knightCircle_right_u.y && coneC.z == knightCircle_right_u.z) {
                movechs(knightCircle_right_u.y, knightCircle_right_u.z); turn = 1;
            }
            else if (coneC.y == knightCircle_right_d.y && coneC.z == knightCircle_right_d.z) {
                movechs(knightCircle_right_d.y, knightCircle_right_d.z); turn = 1;
            }
            else if (coneC.y == knightCircle_behind_r.y && coneC.z == knightCircle_behind_r.z) {
                movechs(knightCircle_behind_r.y, knightCircle_behind_r.z); turn = 1;
            }
            else if (coneC.y == knightCircle_behind_l.y && coneC.z == knightCircle_behind_l.z) {
                movechs(knightCircle_behind_l.y, knightCircle_behind_l.z); turn = 1;
            }
            else {
                movechs(knightC[temp % 7].y, knightC[temp % 7].z);
            }
        }
        else if (temp >= 9 && temp <= 16) {
            if (coneC.y == pawnCircle_front[0].y && coneC.z == pawnCircle_front[0].z) {
                movechs(pawnCircle_front[0].y, pawnCircle_front[0].z); turn = 1;
                pawn_moved_W[temp % 9]++;
            }
            else if (coneC.y == pawnCircle_front[1].y && coneC.z == pawnCircle_front[1].z && pawn_moved_W[pawnchk] == 0) {
                movechs(pawnCircle_front[1].y, pawnCircle_front[1].z); turn = 1;
                pawn_moved_W[temp % 9]++;
            }
            else if (coneC.y == pawnCircle_left.y && coneC.z == pawnCircle_left.z) {
                movechs(pawnCircle_left.y, pawnCircle_left.z); turn = 1;
                pawn_moved_W[temp % 9]++;
            }
            else if (coneC.y == pawnCircle_right.y && coneC.z == pawnCircle_right.z) {
                movechs(pawnCircle_right.y, pawnCircle_right.z); turn = 1;
                pawn_moved_W[temp % 9]++;
            }
            else if (coneC.y == pawnCircle_front_l.y && coneC.z == pawnCircle_front_l.z) {
                movechs(pawnCircle_front_l.y, pawnCircle_front_l.z); turn = 1;
                pawn_moved_W[temp % 9]++;
            }
            else if (coneC.y == pawnCircle_front_r.y && coneC.z == pawnCircle_front_r.z) {
                movechs(pawnCircle_front_r.y, pawnCircle_front_r.z); turn = 1;
                pawn_moved_W[temp % 9]++;
            }
            else {
                movechs(pawnC[temp % 9].y, pawnC[temp % 9].z);
            }
        }
    }
    else {
        if (temp == 17) {
            if (coneD.y == kingCircle_behind.y && coneD.z == kingCircle_behind.z) {
                movechs(kingCircle_behind.y, kingCircle_behind.z); turn = 0;
            }
            else if (coneD.y == kingCircle_front.y && coneD.z == kingCircle_front.z) {
                movechs(kingCircle_front.y, kingCircle_front.z); turn = 0;
            }
            else if (coneD.y == kingCircle_left.y && coneD.z == kingCircle_left.z) {
                movechs(kingCircle_left.y, kingCircle_left.z); turn = 0;
            }
            else if (coneD.y == kingCircle_right.y && coneD.z == kingCircle_right.z) {
                movechs(kingCircle_right.y, kingCircle_right.z); turn = 0;
            }
            else if (coneD.y == kingCircle_right_u.y && coneD.z == kingCircle_right_u.z) {
                movechs(kingCircle_right_u.y, kingCircle_right_u.z); turn = 0;
            }
            else if (coneD.y == kingCircle_right_d.y && coneD.z == kingCircle_right_d.z) {
                movechs(kingCircle_right_d.y, kingCircle_right_d.z); turn = 0;
            }
            else if (coneD.y == kingCircle_left_u.y && coneD.z == kingCircle_left_u.z) {
                movechs(kingCircle_left_u.y, kingCircle_left_u.z); turn = 0;
            }
            else if (coneD.y == kingCircle_left_d.y && coneD.z == kingCircle_left_d.z) {
                movechs(kingCircle_left_d.y, kingCircle_left_d.z); turn = 0;
            }
            else {
                movechs(kingD.y, kingD.z);
            }
        }
        else if (temp == 18) {
            for (int i = 0; i < 8; i++) {
                if (coneD.y == queenCircle_front[i].y && coneD.z == queenCircle_front[i].z) {
                    movechs(queenCircle_front[i].y, queenCircle_front[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == queenCircle_right[i].y && coneD.z == queenCircle_right[i].z) {
                    movechs(queenCircle_right[i].y, queenCircle_right[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == queenCircle_left[i].y && coneD.z == queenCircle_left[i].z) {
                    movechs(queenCircle_left[i].y, queenCircle_left[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == queenCircle_behind[i].y && coneD.z == queenCircle_behind[i].z) {
                    movechs(queenCircle_behind[i].y, queenCircle_behind[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == queenCircle_left_u[i].y && coneD.z == queenCircle_left_u[i].z) {
                    movechs(queenCircle_left_u[i].y, queenCircle_left_u[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == queenCircle_left_d[i].y && coneD.z == queenCircle_left_d[i].z) {
                    movechs(queenCircle_left_d[i].y, queenCircle_left_d[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == queenCircle_right_u[i].y && coneD.z == queenCircle_right_u[i].z) {
                    movechs(queenCircle_right_u[i].y, queenCircle_right_u[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == queenCircle_right_d[i].y && coneD.z == queenCircle_right_d[i].z) {
                    movechs(queenCircle_right_d[i].y, queenCircle_right_d[i].z); turn = 0;
                    break;
                }
                else if (i == 7) {
                    movechs(queenD.y, queenD.z);
                }
            }
        }
        else if (temp == 19 || temp == 20) { // 비숍
            for (int i = 0; i < 8; i++) {
                if (coneD.y == bishopCircle_left_u[i].y && coneD.z == bishopCircle_left_u[i].z) {
                    movechs(bishopCircle_left_u[i].y, bishopCircle_left_u[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == bishopCircle_left_d[i].y && coneD.z == bishopCircle_left_d[i].z) {
                    movechs(bishopCircle_left_d[i].y, bishopCircle_left_d[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == bishopCircle_right_u[i].y && coneD.z == bishopCircle_right_u[i].z) {
                    movechs(bishopCircle_right_u[i].y, bishopCircle_right_u[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == bishopCircle_right_d[i].y && coneD.z == bishopCircle_right_d[i].z) {
                    movechs(bishopCircle_right_d[i].y, bishopCircle_right_d[i].z); turn = 0;
                    break;
                }
                else if (i == 7) {
                    movechs(bishopD[temp % 19].y, bishopD[temp % 19].z);
                }
            }
        }
        else if (temp == 21 || temp == 22) { // 룩
            for (int i = 0; i < 8; i++) {
                if (coneD.y == rookCircle_front[i].y && coneD.z == rookCircle_front[i].z) {
                    movechs(rookCircle_front[i].y, rookCircle_front[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == rookCircle_right[i].y && coneD.z == rookCircle_right[i].z) {
                    movechs(rookCircle_right[i].y, rookCircle_right[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == rookCircle_left[i].y && coneD.z == rookCircle_left[i].z) {
                    movechs(rookCircle_left[i].y, rookCircle_left[i].z); turn = 0;
                    break;
                }
                else if (coneD.y == rookCircle_behind[i].y && coneD.z == rookCircle_behind[i].z) {
                    movechs(rookCircle_behind[i].y, rookCircle_behind[i].z); turn = 0;
                    break;
                }
                else if (i == 7) {
                    movechs(rookD[temp % 21].y, rookD[temp % 21].z);
                }
            }
        }
        else if (temp == 23 || temp == 24) { // 나이트
            if (coneD.y == knightCircle_front_l.y && coneD.z == knightCircle_front_l.z) {
                movechs(knightCircle_front_l.y, knightCircle_front_l.z); turn = 0;
            }
            else if (coneD.y == knightCircle_front_r.y && coneD.z == knightCircle_front_r.z) {
                movechs(knightCircle_front_r.y, knightCircle_front_r.z); turn = 0;
            }
            else if (coneD.y == knightCircle_left_u.y && coneD.z == knightCircle_left_u.z) {
                movechs(knightCircle_left_u.y, knightCircle_left_u.z); turn = 0;
            }
            else if (coneD.y == knightCircle_left_d.y && coneD.z == knightCircle_left_d.z) {
                movechs(knightCircle_left_d.y, knightCircle_left_d.z); turn = 0;
            }
            else if (coneD.y == knightCircle_right_u.y && coneD.z == knightCircle_right_u.z) {
                movechs(knightCircle_right_u.y, knightCircle_right_u.z); turn = 0;
            }
            else if (coneD.y == knightCircle_right_d.y && coneD.z == knightCircle_right_d.z) {
                movechs(knightCircle_right_d.y, knightCircle_right_d.z); turn = 0;
            }
            else if (coneD.y == knightCircle_behind_r.y && coneD.z == knightCircle_behind_r.z) {
                movechs(knightCircle_behind_r.y, knightCircle_behind_r.z); turn = 0;
            }
            else if (coneD.y == knightCircle_behind_l.y && coneD.z == knightCircle_behind_l.z) {
                movechs(knightCircle_behind_l.y, knightCircle_behind_l.z); turn = 0;
            }
            else {
                movechs(knightD[temp % 23].y, knightD[temp % 24].z);
            }
        }
        else if (temp >= 25 && temp <= 32) {
            if (coneD.y == pawnCircle_front[0].y && coneD.z == pawnCircle_front[0].z) {
                movechs(pawnCircle_front[0].y, pawnCircle_front[0].z); turn = 0;
                pawn_moved_B[temp % 25]++;
            }
            else if (coneD.y == pawnCircle_front[1].y && coneD.z == pawnCircle_front[1].z && pawn_moved_B[pawnchk] == 0) {
                movechs(pawnCircle_front[1].y, pawnCircle_front[1].z); turn = 0;
                pawn_moved_B[temp % 25]++;
            }
            else if (coneD.y == pawnCircle_left.y && coneD.z == pawnCircle_left.z) {
                movechs(pawnCircle_left.y, pawnCircle_left.z); turn = 0;
                pawn_moved_B[temp % 25]++;
            }
            else if (coneD.y == pawnCircle_right.y && coneD.z == pawnCircle_right.z) {
                movechs(pawnCircle_right.y, pawnCircle_right.z); turn = 0;
                pawn_moved_B[temp % 25]++;
            }
            else if (coneD.y == pawnCircle_front_l.y && coneD.z == pawnCircle_front_l.z) {
                movechs(pawnCircle_front_l.y, pawnCircle_front_l.z); turn = 0;
                pawn_moved_B[temp % 25]++;
            }
            else if (coneD.y == pawnCircle_front_r.y && coneD.z == pawnCircle_front_r.z) {
                movechs(pawnCircle_front_r.y, pawnCircle_front_r.z); turn = 0;
                pawn_moved_B[temp % 25]++;
            }
            else {
                movechs(pawnD[temp % 25].y, pawnD[temp % 25].z);
            }
        }
    }
}

void move_King_Circle_W() {
    float a = 0.5;
    kingCircle_front = { a, kingC.y, kingC.z + a };
    kingCircle_behind = { a, kingC.y, kingC.z - a };
    kingCircle_left = { a, kingC.y - a, kingC.z };
    kingCircle_right = { a, kingC.y + a, kingC.z };
    kingCircle_left_u = { a, kingC.y - a, kingC.z + a };
    kingCircle_left_d = { a, kingC.y - a, kingC.z - a };
    kingCircle_right_u = { a, kingC.y + a, kingC.z + a };
    kingCircle_right_d = { a, kingC.y + a, kingC.z - a };

    if (kingCircle_right.y == pawnC[0].y && kingCircle_right.z == pawnC[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if ((kingCircle_right.y == pawnC[1].y && kingCircle_right.z == pawnC[1].z)) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnC[2].y && kingCircle_right.z == pawnC[2].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnC[3].y && kingCircle_right.z == pawnC[3].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnC[4].y && kingCircle_right.z == pawnC[4].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnC[5].y && kingCircle_right.z == pawnC[5].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnC[6].y && kingCircle_right.z == pawnC[6].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnC[7].y && kingCircle_right.z == pawnC[7].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == knightC[0].y && kingCircle_right.z == knightC[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == knightC[1].y && kingCircle_right.z == knightC[1].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == bishopC[0].y && kingCircle_right.z == bishopC[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == bishopC[1].y && kingCircle_right.z == bishopC[1].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == rookC[0].y && kingCircle_right.z == rookC[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == rookC[1].y && kingCircle_right.z == rookC[1].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == queenC.y && kingCircle_right.z == queenC.z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else {
        if (kingCircle_right.y <= 1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_right.x, kingCircle_right.y, kingCircle_right.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_left.y == pawnC[0].y && kingCircle_left.z == pawnC[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if ((kingCircle_left.y == pawnC[1].y && kingCircle_left.z == pawnC[1].z)) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnC[2].y && kingCircle_left.z == pawnC[2].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnC[3].y && kingCircle_left.z == pawnC[3].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnC[4].y && kingCircle_left.z == pawnC[4].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnC[5].y && kingCircle_left.z == pawnC[5].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnC[6].y && kingCircle_left.z == pawnC[6].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnC[7].y && kingCircle_left.z == pawnC[7].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == knightC[0].y && kingCircle_left.z == knightC[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == knightC[1].y && kingCircle_left.z == knightC[1].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == bishopC[0].y && kingCircle_left.z == bishopC[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == bishopC[1].y && kingCircle_left.z == bishopC[1].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == rookC[0].y && kingCircle_left.z == rookC[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == rookC[1].y && kingCircle_left.z == rookC[1].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == queenC.y && kingCircle_left.z == queenC.z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else {
        if (kingCircle_left.y >= -2) {
            glPushMatrix();
            glTranslatef(kingCircle_left.x, kingCircle_left.y, kingCircle_left.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_front.y == pawnC[0].y && kingCircle_front.z == pawnC[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if ((kingCircle_front.y == pawnC[1].y && kingCircle_front.z == pawnC[1].z)) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnC[2].y && kingCircle_front.z == pawnC[2].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnC[3].y && kingCircle_front.z == pawnC[3].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnC[4].y && kingCircle_front.z == pawnC[4].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnC[5].y && kingCircle_front.z == pawnC[5].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnC[6].y && kingCircle_front.z == pawnC[6].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnC[7].y && kingCircle_front.z == pawnC[7].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == knightC[0].y && kingCircle_front.z == knightC[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == knightC[1].y && kingCircle_front.z == knightC[1].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == bishopC[0].y && kingCircle_front.z == bishopC[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == bishopC[1].y && kingCircle_front.z == bishopC[1].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == rookC[0].y && kingCircle_front.z == rookC[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == rookC[1].y && kingCircle_front.z == rookC[1].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == queenC.y && kingCircle_front.z == queenC.z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else {
        if (kingCircle_front.z <= 2) {
            glPushMatrix();
            glTranslatef(kingCircle_front.x, kingCircle_front.y, kingCircle_front.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_behind.y == pawnC[0].y && kingCircle_behind.z == pawnC[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if ((kingCircle_behind.y == pawnC[1].y && kingCircle_behind.z == pawnC[1].z)) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnC[2].y && kingCircle_behind.z == pawnC[2].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnC[3].y && kingCircle_behind.z == pawnC[3].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnC[4].y && kingCircle_behind.z == pawnC[4].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnC[5].y && kingCircle_behind.z == pawnC[5].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnC[6].y && kingCircle_behind.z == pawnC[6].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnC[7].y && kingCircle_behind.z == pawnC[7].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == knightC[0].y && kingCircle_behind.z == knightC[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == knightC[1].y && kingCircle_behind.z == knightC[1].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == bishopC[0].y && kingCircle_behind.z == bishopC[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == bishopC[1].y && kingCircle_behind.z == bishopC[1].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == rookC[0].y && kingCircle_behind.z == rookC[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == rookC[1].y && kingCircle_behind.z == rookC[1].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == queenC.y && kingCircle_behind.z == queenC.z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else {
        if (kingCircle_behind.z >= -1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_behind.x, kingCircle_behind.y, kingCircle_behind.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_left_d.y == pawnC[0].y && kingCircle_left_d.z == pawnC[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if ((kingCircle_left_d.y == pawnC[1].y && kingCircle_left_d.z == pawnC[1].z)) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnC[2].y && kingCircle_left_d.z == pawnC[2].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnC[3].y && kingCircle_left_d.z == pawnC[3].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnC[4].y && kingCircle_left_d.z == pawnC[4].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnC[5].y && kingCircle_left_d.z == pawnC[5].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnC[6].y && kingCircle_left_d.z == pawnC[6].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnC[7].y && kingCircle_left_d.z == pawnC[7].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == knightC[0].y && kingCircle_left_d.z == knightC[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == knightC[1].y && kingCircle_left_d.z == knightC[1].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == bishopC[0].y && kingCircle_left_d.z == bishopC[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == bishopC[1].y && kingCircle_left_d.z == bishopC[1].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == rookC[0].y && kingCircle_left_d.z == rookC[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == rookC[1].y && kingCircle_left_d.z == rookC[1].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == queenC.y && kingCircle_left_d.z == queenC.z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else {
        if (kingCircle_left_d.y >= -2 && kingCircle_left_d.z >= -1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_left_d.x, kingCircle_left_d.y, kingCircle_left_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_right_d.y == pawnC[0].y && kingCircle_right_d.z == pawnC[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if ((kingCircle_right_d.y == pawnC[1].y && kingCircle_right_d.z == pawnC[1].z)) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnC[2].y && kingCircle_right_d.z == pawnC[2].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnC[3].y && kingCircle_right_d.z == pawnC[3].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnC[4].y && kingCircle_right_d.z == pawnC[4].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnC[5].y && kingCircle_right_d.z == pawnC[5].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnC[6].y && kingCircle_right_d.z == pawnC[6].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnC[7].y && kingCircle_right_d.z == pawnC[7].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == knightC[0].y && kingCircle_right_d.z == knightC[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == knightC[1].y && kingCircle_right_d.z == knightC[1].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == bishopC[0].y && kingCircle_right_d.z == bishopC[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == bishopC[1].y && kingCircle_right_d.z == bishopC[1].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == rookC[0].y && kingCircle_right_d.z == rookC[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == rookC[1].y && kingCircle_right_d.z == rookC[1].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == queenC.y && kingCircle_right_d.z == queenC.z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else {
        if (kingCircle_right_d.y <= 1.5 && kingCircle_right_d.z >= -1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_right_d.x, kingCircle_right_d.y, kingCircle_right_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_left_u.y == pawnC[0].y && kingCircle_left_u.z == pawnC[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if ((kingCircle_left_u.y == pawnC[1].y && kingCircle_left_u.z == pawnC[1].z)) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnC[2].y && kingCircle_left_u.z == pawnC[2].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnC[3].y && kingCircle_left_u.z == pawnC[3].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnC[4].y && kingCircle_left_u.z == pawnC[4].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnC[5].y && kingCircle_left_u.z == pawnC[5].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnC[6].y && kingCircle_left_u.z == pawnC[6].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnC[7].y && kingCircle_left_u.z == pawnC[7].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == knightC[0].y && kingCircle_left_u.z == knightC[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == knightC[1].y && kingCircle_left_u.z == knightC[1].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == bishopC[0].y && kingCircle_left_u.z == bishopC[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == bishopC[1].y && kingCircle_left_u.z == bishopC[1].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == rookC[0].y && kingCircle_left_u.z == rookC[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == rookC[1].y && kingCircle_left_u.z == rookC[1].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == queenC.y && kingCircle_left_u.z == queenC.z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else {
        if (kingCircle_left_u.y >= -2 && kingCircle_left_u.z <= 2) {
            glPushMatrix();
            glTranslatef(kingCircle_left_u.x, kingCircle_left_u.y, kingCircle_left_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }


    if (kingCircle_right_u.y == pawnC[0].y && kingCircle_right_u.z == pawnC[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if ((kingCircle_right_u.y == pawnC[1].y && kingCircle_right_u.z == pawnC[1].z)) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnC[2].y && kingCircle_right_u.z == pawnC[2].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnC[3].y && kingCircle_right_u.z == pawnC[3].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnC[4].y && kingCircle_right_u.z == pawnC[4].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnC[5].y && kingCircle_right_u.z == pawnC[5].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnC[6].y && kingCircle_right_u.z == pawnC[6].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnC[7].y && kingCircle_right_u.z == pawnC[7].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == knightC[0].y && kingCircle_right_u.z == knightC[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == knightC[1].y && kingCircle_right_u.z == knightC[1].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == bishopC[0].y && kingCircle_right_u.z == bishopC[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == bishopC[1].y && kingCircle_right_u.z == bishopC[1].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == rookC[0].y && kingCircle_right_u.z == rookC[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == rookC[1].y && kingCircle_right_u.z == rookC[1].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == queenC.y && kingCircle_right_u.z == queenC.z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else {
        if (kingCircle_right_u.y <= 1.5 && kingCircle_right_u.z <= 2) {
            glPushMatrix();
            glTranslatef(kingCircle_right_u.x, kingCircle_right_u.y, kingCircle_right_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
}
void move_Queen_Circle_W() {
    float a = 0.5;
    float b = 0.5;
    int  j = 0;
    for (int i = 0; i < 8; i++) {
        queenCircle_front[i] = { a, queenC.y, queenC.z + b };
        queenCircle_right[i] = { a, queenC.y + b, queenC.z };
        queenCircle_left[i] = { a, queenC.y - b, queenC.z };
        queenCircle_behind[i] = { a, queenC.y, queenC.z - b };
        queenCircle_left_u[i] = { a, queenC.y - b, queenC.z + b };
        queenCircle_left_d[i] = { a, queenC.y - b, queenC.z - b };
        queenCircle_right_u[i] = { a, queenC.y + b, queenC.z + b };
        queenCircle_right_d[i] = { a, queenC.y + b, queenC.z - b };

        b += 0.5;
    }

    for (float i = 0.5; queenC.y + i <= 1.5; i += 0.5) {

        if (queenCircle_right[j].y == pawnC[0].y && queenCircle_right[j].z == pawnC[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if ((queenCircle_right[j].y == pawnC[1].y && queenCircle_right[j].z == pawnC[1].z)) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnC[2].y && queenCircle_right[j].z == pawnC[2].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnC[3].y && queenCircle_right[j].z == pawnC[3].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnC[4].y && queenCircle_right[j].z == pawnC[4].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnC[5].y && queenCircle_right[j].z == pawnC[5].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnC[6].y && queenCircle_right[j].z == pawnC[6].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnC[7].y && queenCircle_right[j].z == pawnC[7].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == knightC[0].y && queenCircle_right[j].z == knightC[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == knightC[1].y && queenCircle_right[j].z == knightC[1].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == bishopC[0].y && queenCircle_right[j].z == bishopC[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == bishopC[1].y && queenCircle_right[j].z == bishopC[1].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == rookC[0].y && queenCircle_right[j].z == rookC[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == rookC[1].y && queenCircle_right[j].z == rookC[1].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == kingC.y && queenCircle_right[j].z == kingC.z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_right[j].x, queenCircle_right[j].y, queenCircle_right[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenC.y - i >= -2; i += 0.5) {
        if (queenCircle_left[j].y == pawnC[0].y && queenCircle_left[j].z == pawnC[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if ((queenCircle_left[j].y == pawnC[1].y && queenCircle_left[j].z == pawnC[1].z)) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnC[2].y && queenCircle_left[j].z == pawnC[2].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnC[3].y && queenCircle_left[j].z == pawnC[3].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnC[4].y && queenCircle_left[j].z == pawnC[4].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnC[5].y && queenCircle_left[j].z == pawnC[5].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnC[6].y && queenCircle_left[j].z == pawnC[6].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnC[7].y && queenCircle_left[j].z == pawnC[7].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == knightC[0].y && queenCircle_left[j].z == knightC[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == knightC[1].y && queenCircle_left[j].z == knightC[1].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == bishopC[0].y && queenCircle_left[j].z == bishopC[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == bishopC[1].y && queenCircle_left[j].z == bishopC[1].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == rookC[0].y && queenCircle_left[j].z == rookC[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == rookC[1].y && queenCircle_left[j].z == rookC[1].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == kingC.y && queenCircle_left[j].z == kingC.z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_left[j].x, queenCircle_left[j].y, queenCircle_left[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenC.z + i <= 2; i += 0.5) {
        if (queenCircle_front[j].y == pawnC[0].y && queenCircle_front[j].z == pawnC[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if ((queenCircle_front[j].y == pawnC[1].y && queenCircle_front[j].z == pawnC[1].z)) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnC[2].y && queenCircle_front[j].z == pawnC[2].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnC[3].y && queenCircle_front[j].z == pawnC[3].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnC[4].y && queenCircle_front[j].z == pawnC[4].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnC[5].y && queenCircle_front[j].z == pawnC[5].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnC[6].y && queenCircle_front[j].z == pawnC[6].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnC[7].y && queenCircle_front[j].z == pawnC[7].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == knightC[0].y && queenCircle_front[j].z == knightC[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == knightC[1].y && queenCircle_front[j].z == knightC[1].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == bishopC[0].y && queenCircle_front[j].z == bishopC[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == bishopC[1].y && queenCircle_front[j].z == bishopC[1].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == rookC[0].y && queenCircle_front[j].z == rookC[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == rookC[1].y && queenCircle_front[j].z == rookC[1].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == kingC.y && queenCircle_front[j].z == kingC.z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_front[j].x, queenCircle_front[j].y, queenCircle_front[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenC.z - i >= -1.5; i += 0.5) {
        if (queenCircle_behind[j].y == pawnC[0].y && queenCircle_behind[j].z == pawnC[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if ((queenCircle_behind[j].y == pawnC[1].y && queenCircle_behind[j].z == pawnC[1].z)) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnC[2].y && queenCircle_behind[j].z == pawnC[2].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnC[3].y && queenCircle_behind[j].z == pawnC[3].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnC[4].y && queenCircle_behind[j].z == pawnC[4].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnC[5].y && queenCircle_behind[j].z == pawnC[5].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnC[6].y && queenCircle_behind[j].z == pawnC[6].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnC[7].y && queenCircle_behind[j].z == pawnC[7].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == knightC[0].y && queenCircle_behind[j].z == knightC[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == knightC[1].y && queenCircle_behind[j].z == knightC[1].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == bishopC[0].y && queenCircle_behind[j].z == bishopC[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == bishopC[1].y && queenCircle_behind[j].z == bishopC[1].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == rookC[0].y && queenCircle_behind[j].z == rookC[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == rookC[1].y && queenCircle_behind[j].z == rookC[1].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == kingC.y && queenCircle_behind[j].z == kingC.z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_behind[j].x, queenCircle_behind[j].y, queenCircle_behind[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenC.y - i >= -2 && queenC.z - i >= -1.5; i += 0.5) {
        if (queenCircle_left_d[j].y == pawnC[0].y && queenCircle_left_d[j].z == pawnC[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if ((queenCircle_left_d[j].y == pawnC[1].y && queenCircle_left_d[j].z == pawnC[1].z)) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnC[2].y && queenCircle_left_d[j].z == pawnC[2].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnC[3].y && queenCircle_left_d[j].z == pawnC[3].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnC[4].y && queenCircle_left_d[j].z == pawnC[4].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnC[5].y && queenCircle_left_d[j].z == pawnC[5].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnC[6].y && queenCircle_left_d[j].z == pawnC[6].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnC[7].y && queenCircle_left_d[j].z == pawnC[7].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == knightC[0].y && queenCircle_left_d[j].z == knightC[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == knightC[1].y && queenCircle_left_d[j].z == knightC[1].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == bishopC[0].y && queenCircle_left_d[j].z == bishopC[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == bishopC[1].y && queenCircle_left_d[j].z == bishopC[1].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == rookC[0].y && queenCircle_left_d[j].z == rookC[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == rookC[1].y && queenCircle_left_d[j].z == rookC[1].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == kingC.y && queenCircle_left_d[j].z == kingC.z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_left_d[j].x, queenCircle_left_d[j].y, queenCircle_left_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenC.y + i <= 1.5 && queenC.z - i >= -1.5; i += 0.5) {
        if (queenCircle_right_d[j].y == pawnC[0].y && queenCircle_right_d[j].z == pawnC[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if ((queenCircle_right_d[j].y == pawnC[1].y && queenCircle_right_d[j].z == pawnC[1].z)) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnC[2].y && queenCircle_right_d[j].z == pawnC[2].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnC[3].y && queenCircle_right_d[j].z == pawnC[3].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnC[4].y && queenCircle_right_d[j].z == pawnC[4].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnC[5].y && queenCircle_right_d[j].z == pawnC[5].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnC[6].y && queenCircle_right_d[j].z == pawnC[6].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnC[7].y && queenCircle_right_d[j].z == pawnC[7].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == knightC[0].y && queenCircle_right_d[j].z == knightC[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == knightC[1].y && queenCircle_right_d[j].z == knightC[1].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == bishopC[0].y && queenCircle_right_d[j].z == bishopC[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == bishopC[1].y && queenCircle_right_d[j].z == bishopC[1].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == rookC[0].y && queenCircle_right_d[j].z == rookC[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == rookC[1].y && queenCircle_right_d[j].z == rookC[1].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == kingC.y && queenCircle_right_d[j].z == kingC.z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_right_d[j].x, queenCircle_right_d[j].y, queenCircle_right_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenC.y - i >= -2 && queenC.z + i <= 2; i += 0.5) {
        if (queenCircle_left_u[j].y == pawnC[0].y && queenCircle_left_u[j].z == pawnC[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if ((queenCircle_left_u[j].y == pawnC[1].y && queenCircle_left_u[j].z == pawnC[1].z)) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnC[2].y && queenCircle_left_u[j].z == pawnC[2].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnC[3].y && queenCircle_left_u[j].z == pawnC[3].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnC[4].y && queenCircle_left_u[j].z == pawnC[4].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnC[5].y && queenCircle_left_u[j].z == pawnC[5].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnC[6].y && queenCircle_left_u[j].z == pawnC[6].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnC[7].y && queenCircle_left_u[j].z == pawnC[7].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == knightC[0].y && queenCircle_left_u[j].z == knightC[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == knightC[1].y && queenCircle_left_u[j].z == knightC[1].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == bishopC[0].y && queenCircle_left_u[j].z == bishopC[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == bishopC[1].y && queenCircle_left_u[j].z == bishopC[1].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == rookC[0].y && queenCircle_left_u[j].z == rookC[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == rookC[1].y && queenCircle_left_u[j].z == rookC[1].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == kingC.y && queenCircle_left_u[j].z == kingC.z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_left_u[j].x, queenCircle_left_u[j].y, queenCircle_left_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenC.y + i <= 1.5 && queenC.z + i <= 2; i += 0.5) {
        if (queenCircle_right_u[j].y == pawnC[0].y && queenCircle_right_u[j].z == pawnC[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if ((queenCircle_right_u[j].y == pawnC[1].y && queenCircle_right_u[j].z == pawnC[1].z)) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnC[2].y && queenCircle_right_u[j].z == pawnC[2].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnC[3].y && queenCircle_right_u[j].z == pawnC[3].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnC[4].y && queenCircle_right_u[j].z == pawnC[4].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnC[5].y && queenCircle_right_u[j].z == pawnC[5].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnC[6].y && queenCircle_right_u[j].z == pawnC[6].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnC[7].y && queenCircle_right_u[j].z == pawnC[7].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == knightC[0].y && queenCircle_right_u[j].z == knightC[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == knightC[1].y && queenCircle_right_u[j].z == knightC[1].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == bishopC[0].y && queenCircle_right_u[j].z == bishopC[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == bishopC[1].y && queenCircle_right_u[j].z == bishopC[1].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == rookC[0].y && queenCircle_right_u[j].z == rookC[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == rookC[1].y && queenCircle_right_u[j].z == rookC[1].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == kingC.y && queenCircle_right_u[j].z == kingC.z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_right_u[j].x, queenCircle_right_u[j].y, queenCircle_right_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
}
void move_Bishop_Circle_W(int x) {
    float a = 0.5;
    float b = 0.5;
    int  j = 0;
    for (int i = 0; i < 8; i++) {
        bishopCircle_left_u[i] = { a, bishopC[x].y - b, bishopC[x].z + b };
        bishopCircle_left_d[i] = { a, bishopC[x].y - b, bishopC[x].z - b };
        bishopCircle_right_u[i] = { a, bishopC[x].y + b, bishopC[x].z + b };
        bishopCircle_right_d[i] = { a, bishopC[x].y + b, bishopC[x].z - b };

        b += 0.5;
    }
    for (float i = 0.5; bishopC[x].y - i >= -2 && bishopC[x].z - i >= -1.5; i += 0.5) {
        if (bishopCircle_left_d[j].y == pawnC[0].y && bishopCircle_left_d[j].z == pawnC[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if ((bishopCircle_left_d[j].y == pawnC[1].y && bishopCircle_left_d[j].z == pawnC[1].z)) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnC[2].y && bishopCircle_left_d[j].z == pawnC[2].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnC[3].y && bishopCircle_left_d[j].z == pawnC[3].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnC[4].y && bishopCircle_left_d[j].z == pawnC[4].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnC[5].y && bishopCircle_left_d[j].z == pawnC[5].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnC[6].y && bishopCircle_left_d[j].z == pawnC[6].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnC[7].y && bishopCircle_left_d[j].z == pawnC[7].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == knightC[0].y && bishopCircle_left_d[j].z == knightC[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == knightC[1].y && bishopCircle_left_d[j].z == knightC[1].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == bishopC[0].y && bishopCircle_left_d[j].z == bishopC[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == bishopC[1].y && bishopCircle_left_d[j].z == bishopC[1].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == rookC[0].y && bishopCircle_left_d[j].z == rookC[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == rookC[1].y && bishopCircle_left_d[j].z == rookC[1].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == queenC.y && bishopCircle_left_d[j].z == queenC.z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == kingC.y && bishopCircle_left_d[j].z == kingC.z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_left_d[j].x, bishopCircle_left_d[j].y, bishopCircle_left_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; bishopC[x].y + i <= 1.5 && bishopC[x].z - i >= -1.5; i += 0.5) {
        if (bishopCircle_right_d[j].y == pawnC[0].y && bishopCircle_right_d[j].z == pawnC[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if ((bishopCircle_right_d[j].y == pawnC[1].y && bishopCircle_right_d[j].z == pawnC[1].z)) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnC[2].y && bishopCircle_right_d[j].z == pawnC[2].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnC[3].y && bishopCircle_right_d[j].z == pawnC[3].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnC[4].y && bishopCircle_right_d[j].z == pawnC[4].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnC[5].y && bishopCircle_right_d[j].z == pawnC[5].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnC[6].y && bishopCircle_right_d[j].z == pawnC[6].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnC[7].y && bishopCircle_right_d[j].z == pawnC[7].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == knightC[0].y && bishopCircle_right_d[j].z == knightC[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == knightC[1].y && bishopCircle_right_d[j].z == knightC[1].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == bishopC[0].y && bishopCircle_right_d[j].z == bishopC[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == bishopC[1].y && bishopCircle_right_d[j].z == bishopC[1].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == rookC[0].y && bishopCircle_right_d[j].z == rookC[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == rookC[1].y && bishopCircle_right_d[j].z == rookC[1].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == queenC.y && bishopCircle_right_d[j].z == queenC.z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == kingC.y && bishopCircle_right_d[j].z == kingC.z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_right_d[j].x, bishopCircle_right_d[j].y, bishopCircle_right_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; bishopC[x].y - i >= -2 && bishopC[x].z + i <= 2; i += 0.5) {
        if (bishopCircle_left_u[j].y == pawnC[0].y && bishopCircle_left_u[j].z == pawnC[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if ((bishopCircle_left_u[j].y == pawnC[1].y && bishopCircle_left_u[j].z == pawnC[1].z)) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnC[2].y && bishopCircle_left_u[j].z == pawnC[2].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnC[3].y && bishopCircle_left_u[j].z == pawnC[3].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnC[4].y && bishopCircle_left_u[j].z == pawnC[4].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnC[5].y && bishopCircle_left_u[j].z == pawnC[5].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnC[6].y && bishopCircle_left_u[j].z == pawnC[6].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnC[7].y && bishopCircle_left_u[j].z == pawnC[7].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == knightC[0].y && bishopCircle_left_u[j].z == knightC[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == knightC[1].y && bishopCircle_left_u[j].z == knightC[1].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == bishopC[0].y && bishopCircle_left_u[j].z == bishopC[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == bishopC[1].y && bishopCircle_left_u[j].z == bishopC[1].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == rookC[0].y && bishopCircle_left_u[j].z == rookC[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == rookC[1].y && bishopCircle_left_u[j].z == rookC[1].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == queenC.y && bishopCircle_left_u[j].z == queenC.z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == kingC.y && bishopCircle_left_u[j].z == kingC.z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_left_u[j].x, bishopCircle_left_u[j].y, bishopCircle_left_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; bishopC[x].y + i <= 1.5 && bishopC[x].z + i <= 2; i += 0.5) {
        if (bishopCircle_right_u[j].y == pawnC[0].y && bishopCircle_right_u[j].z == pawnC[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if ((bishopCircle_right_u[j].y == pawnC[1].y && bishopCircle_right_u[j].z == pawnC[1].z)) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnC[2].y && bishopCircle_right_u[j].z == pawnC[2].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnC[3].y && bishopCircle_right_u[j].z == pawnC[3].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnC[4].y && bishopCircle_right_u[j].z == pawnC[4].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnC[5].y && bishopCircle_right_u[j].z == pawnC[5].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnC[6].y && bishopCircle_right_u[j].z == pawnC[6].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnC[7].y && bishopCircle_right_u[j].z == pawnC[7].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == knightC[0].y && bishopCircle_right_u[j].z == knightC[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == knightC[1].y && bishopCircle_right_u[j].z == knightC[1].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == bishopC[0].y && bishopCircle_right_u[j].z == bishopC[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == bishopC[1].y && bishopCircle_right_u[j].z == bishopC[1].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == rookC[0].y && bishopCircle_right_u[j].z == rookC[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == rookC[1].y && bishopCircle_right_u[j].z == rookC[1].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == queenC.y && bishopCircle_right_u[j].z == queenC.z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == kingC.y && bishopCircle_right_u[j].z == kingC.z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_right_u[j].x, bishopCircle_right_u[j].y, bishopCircle_right_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
}
void move_Rook_Circle_W(int x) {
    float a = 0.5;
    float b = 0.5;
    for (int i = 0; i < 8; i++) {
        rookCircle_front[i].x = a;
        rookCircle_front[i].y = rookC[x].y;
        rookCircle_front[i].z = rookC[x].z + b;

        rookCircle_behind[i].x = a;
        rookCircle_behind[i].y = rookC[x].y;
        rookCircle_behind[i].z = rookC[x].z - b;

        rookCircle_right[i].x = a;
        rookCircle_right[i].y = rookC[x].y + b;
        rookCircle_right[i].z = rookC[x].z;

        rookCircle_left[i].x = a;
        rookCircle_left[i].y = rookC[x].y - b;
        rookCircle_left[i].z = rookC[x].z;
        b += 0.5;
    }
    int j = 0;
    for (float i = 0.5; rookC[x].y + i <= 1.5; i += 0.5) {
        if (rookCircle_right[j].y == pawnC[0].y && rookCircle_right[j].z == pawnC[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if ((rookCircle_right[j].y == pawnC[1].y && rookCircle_right[j].z == pawnC[1].z)) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnC[2].y && rookCircle_right[j].z == pawnC[2].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnC[3].y && rookCircle_right[j].z == pawnC[3].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnC[4].y && rookCircle_right[j].z == pawnC[4].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnC[5].y && rookCircle_right[j].z == pawnC[5].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnC[6].y && rookCircle_right[j].z == pawnC[6].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnC[7].y && rookCircle_right[j].z == pawnC[7].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == knightC[0].y && rookCircle_right[j].z == knightC[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == knightC[1].y && rookCircle_right[j].z == knightC[1].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == bishopC[0].y && rookCircle_right[j].z == bishopC[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == bishopC[1].y && rookCircle_right[j].z == bishopC[1].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == rookC[0].y && rookCircle_right[j].z == rookC[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == rookC[1].y && rookCircle_right[j].z == rookC[1].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == queenC.y && rookCircle_right[j].z == queenC.z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == kingC.y && rookCircle_right[j].z == kingC.z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_right[j].x, rookCircle_right[j].y, rookCircle_right[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; rookC[x].y - i >= -2; i += 0.5) {
        if (rookCircle_left[j].y == pawnC[0].y && rookCircle_left[j].z == pawnC[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if ((rookCircle_left[j].y == pawnC[1].y && rookCircle_left[j].z == pawnC[1].z)) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnC[2].y && rookCircle_left[j].z == pawnC[2].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnC[3].y && rookCircle_left[j].z == pawnC[3].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnC[4].y && rookCircle_left[j].z == pawnC[4].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnC[5].y && rookCircle_left[j].z == pawnC[5].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnC[6].y && rookCircle_left[j].z == pawnC[6].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnC[7].y && rookCircle_left[j].z == pawnC[7].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == knightC[0].y && rookCircle_left[j].z == knightC[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == knightC[1].y && rookCircle_left[j].z == knightC[1].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == bishopC[0].y && rookCircle_left[j].z == bishopC[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == bishopC[1].y && rookCircle_left[j].z == bishopC[1].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == rookC[0].y && rookCircle_left[j].z == rookC[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == rookC[1].y && rookCircle_left[j].z == rookC[1].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == queenC.y && rookCircle_left[j].z == queenC.z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == kingC.y && rookCircle_left[j].z == kingC.z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_left[j].x, rookCircle_left[j].y, rookCircle_left[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; rookC[x].z + i <= 2; i += 0.5) {
        if (rookCircle_front[j].y == pawnC[0].y && rookCircle_front[j].z == pawnC[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if ((rookCircle_front[j].y == pawnC[1].y && rookCircle_front[j].z == pawnC[1].z)) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnC[2].y && rookCircle_front[j].z == pawnC[2].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnC[3].y && rookCircle_front[j].z == pawnC[3].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnC[4].y && rookCircle_front[j].z == pawnC[4].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnC[5].y && rookCircle_front[j].z == pawnC[5].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnC[6].y && rookCircle_front[j].z == pawnC[6].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnC[7].y && rookCircle_front[j].z == pawnC[7].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == knightC[0].y && rookCircle_front[j].z == knightC[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == knightC[1].y && rookCircle_front[j].z == knightC[1].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == bishopC[0].y && rookCircle_front[j].z == bishopC[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == bishopC[1].y && rookCircle_front[j].z == bishopC[1].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == rookC[0].y && rookCircle_front[j].z == rookC[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == rookC[1].y && rookCircle_front[j].z == rookC[1].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == queenC.y && rookCircle_front[j].z == queenC.z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == kingC.y && rookCircle_front[j].z == kingC.z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_front[j].x, rookCircle_front[j].y, rookCircle_front[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; rookC[x].z - i >= -1.5; i += 0.5) {
        if (rookCircle_behind[j].y == pawnC[0].y && rookCircle_behind[j].z == pawnC[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if ((rookCircle_behind[j].y == pawnC[1].y && rookCircle_behind[j].z == pawnC[1].z)) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnC[2].y && rookCircle_behind[j].z == pawnC[2].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnC[3].y && rookCircle_behind[j].z == pawnC[3].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnC[4].y && rookCircle_behind[j].z == pawnC[4].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnC[5].y && rookCircle_behind[j].z == pawnC[5].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnC[6].y && rookCircle_behind[j].z == pawnC[6].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnC[7].y && rookCircle_behind[j].z == pawnC[7].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == knightC[0].y && rookCircle_behind[j].z == knightC[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == knightC[1].y && rookCircle_behind[j].z == knightC[1].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == bishopC[0].y && rookCircle_behind[j].z == bishopC[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == bishopC[1].y && rookCircle_behind[j].z == bishopC[1].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == rookC[0].y && rookCircle_behind[j].z == rookC[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == rookC[1].y && rookCircle_behind[j].z == rookC[1].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == queenC.y && rookCircle_behind[j].z == queenC.z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == kingC.y && rookCircle_behind[j].z == kingC.z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_behind[j].x, rookCircle_behind[j].y, rookCircle_behind[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
}
void move_Knight_Circle_W(int x) {
    float a = 0.5;
    float b = 1.0;
    knightCircle_front_r = { a, knightC[x].y + a, knightC[x].z + b };
    knightCircle_behind_r = { a, knightC[x].y + a, knightC[x].z - b };
    knightCircle_front_l = { a, knightC[x].y - a, knightC[x].z + b };
    knightCircle_behind_l = { a, knightC[x].y - a, knightC[x].z - b };
    knightCircle_left_u = { a, knightC[x].y - b, knightC[x].z + a };
    knightCircle_right_u = { a, knightC[x].y + b, knightC[x].z + a };
    knightCircle_left_d = { a, knightC[x].y - b, knightC[x].z - a };
    knightCircle_right_d = { a, knightC[x].y + b, knightC[x].z - a };

    if (knightCircle_front_r.y == pawnC[0].y && knightCircle_front_r.z == pawnC[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if ((knightCircle_front_r.y == pawnC[1].y && knightCircle_front_r.z == pawnC[1].z)) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnC[2].y && knightCircle_front_r.z == pawnC[2].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnC[3].y && knightCircle_front_r.z == pawnC[3].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnC[4].y && knightCircle_front_r.z == pawnC[4].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnC[5].y && knightCircle_front_r.z == pawnC[5].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnC[6].y && knightCircle_front_r.z == pawnC[6].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnC[7].y && knightCircle_front_r.z == pawnC[7].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == knightC[0].y && knightCircle_front_r.z == knightC[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == knightC[1].y && knightCircle_front_r.z == knightC[1].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == bishopC[0].y && knightCircle_front_r.z == bishopC[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == bishopC[1].y && knightCircle_front_r.z == bishopC[1].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == rookC[0].y && knightCircle_front_r.z == rookC[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == rookC[1].y && knightCircle_front_r.z == rookC[1].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == queenC.y && knightCircle_front_r.z == queenC.z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == kingC.y && knightCircle_front_r.z == kingC.z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else {
        if (knightCircle_front_r.y <= 1.5 && knightCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(knightCircle_front_r.x, knightCircle_front_r.y, knightCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_behind_r.y == pawnC[0].y && knightCircle_behind_r.z == pawnC[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if ((knightCircle_behind_r.y == pawnC[1].y && knightCircle_behind_r.z == pawnC[1].z)) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnC[2].y && knightCircle_behind_r.z == pawnC[2].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnC[3].y && knightCircle_behind_r.z == pawnC[3].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnC[4].y && knightCircle_behind_r.z == pawnC[4].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnC[5].y && knightCircle_behind_r.z == pawnC[5].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnC[6].y && knightCircle_behind_r.z == pawnC[6].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnC[7].y && knightCircle_behind_r.z == pawnC[7].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == knightC[0].y && knightCircle_behind_r.z == knightC[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == knightC[1].y && knightCircle_behind_r.z == knightC[1].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == bishopC[0].y && knightCircle_behind_r.z == bishopC[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == bishopC[1].y && knightCircle_behind_r.z == bishopC[1].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == rookC[0].y && knightCircle_behind_r.z == rookC[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == rookC[1].y && knightCircle_behind_r.z == rookC[1].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == queenC.y && knightCircle_behind_r.z == queenC.z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == kingC.y && knightCircle_behind_r.z == kingC.z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else {
        if (knightCircle_behind_r.y <= 1.5 && knightCircle_behind_r.z >= -1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_behind_r.x, knightCircle_behind_r.y, knightCircle_behind_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_front_l.y == pawnC[0].y && knightCircle_front_l.z == pawnC[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if ((knightCircle_front_l.y == pawnC[1].y && knightCircle_front_l.z == pawnC[1].z)) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnC[2].y && knightCircle_front_l.z == pawnC[2].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnC[3].y && knightCircle_front_l.z == pawnC[3].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnC[4].y && knightCircle_front_l.z == pawnC[4].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnC[5].y && knightCircle_front_l.z == pawnC[5].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnC[6].y && knightCircle_front_l.z == pawnC[6].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnC[7].y && knightCircle_front_l.z == pawnC[7].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == knightC[0].y && knightCircle_front_l.z == knightC[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == knightC[1].y && knightCircle_front_l.z == knightC[1].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == bishopC[0].y && knightCircle_front_l.z == bishopC[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == bishopC[1].y && knightCircle_front_l.z == bishopC[1].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == rookC[0].y && knightCircle_front_l.z == rookC[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == rookC[1].y && knightCircle_front_l.z == rookC[1].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == queenC.y && knightCircle_front_l.z == queenC.z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == kingC.y && knightCircle_front_l.z == kingC.z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else {
        if (knightCircle_front_l.y >= -2 && knightCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(knightCircle_front_l.x, knightCircle_front_l.y, knightCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_behind_l.y == pawnC[0].y && knightCircle_behind_l.z == pawnC[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if ((knightCircle_behind_l.y == pawnC[1].y && knightCircle_behind_l.z == pawnC[1].z)) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnC[2].y && knightCircle_behind_l.z == pawnC[2].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnC[3].y && knightCircle_behind_l.z == pawnC[3].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnC[4].y && knightCircle_behind_l.z == pawnC[4].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnC[5].y && knightCircle_behind_l.z == pawnC[5].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnC[6].y && knightCircle_behind_l.z == pawnC[6].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnC[7].y && knightCircle_behind_l.z == pawnC[7].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == knightC[0].y && knightCircle_behind_l.z == knightC[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == knightC[1].y && knightCircle_behind_l.z == knightC[1].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == bishopC[0].y && knightCircle_behind_l.z == bishopC[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == bishopC[1].y && knightCircle_behind_l.z == bishopC[1].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == rookC[0].y && knightCircle_behind_l.z == rookC[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == rookC[1].y && knightCircle_behind_l.z == rookC[1].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == queenC.y && knightCircle_behind_l.z == queenC.z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == kingC.y && knightCircle_behind_l.z == kingC.z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else {
        if (knightCircle_behind_l.y >= -2 && knightCircle_behind_l.z >= -1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_behind_l.x, knightCircle_behind_l.y, knightCircle_behind_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_left_u.y == pawnC[0].y && knightCircle_left_u.z == pawnC[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if ((knightCircle_left_u.y == pawnC[1].y && knightCircle_left_u.z == pawnC[1].z)) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnC[2].y && knightCircle_left_u.z == pawnC[2].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnC[3].y && knightCircle_left_u.z == pawnC[3].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnC[4].y && knightCircle_left_u.z == pawnC[4].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnC[5].y && knightCircle_left_u.z == pawnC[5].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnC[6].y && knightCircle_left_u.z == pawnC[6].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnC[7].y && knightCircle_left_u.z == pawnC[7].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == knightC[0].y && knightCircle_left_u.z == knightC[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == knightC[1].y && knightCircle_left_u.z == knightC[1].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == bishopC[0].y && knightCircle_left_u.z == bishopC[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == bishopC[1].y && knightCircle_left_u.z == bishopC[1].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == rookC[0].y && knightCircle_left_u.z == rookC[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == rookC[1].y && knightCircle_left_u.z == rookC[1].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == queenC.y && knightCircle_left_u.z == queenC.z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == kingC.y && knightCircle_left_u.z == kingC.z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else {
        if (knightCircle_left_u.z <= 2 && knightCircle_left_u.y >= -2) {
            glPushMatrix();
            glTranslatef(knightCircle_left_u.x, knightCircle_left_u.y, knightCircle_left_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_right_u.y == pawnC[0].y && knightCircle_right_u.z == pawnC[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if ((knightCircle_right_u.y == pawnC[1].y && knightCircle_right_u.z == pawnC[1].z)) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnC[2].y && knightCircle_right_u.z == pawnC[2].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnC[3].y && knightCircle_right_u.z == pawnC[3].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnC[4].y && knightCircle_right_u.z == pawnC[4].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnC[5].y && knightCircle_right_u.z == pawnC[5].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnC[6].y && knightCircle_right_u.z == pawnC[6].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnC[7].y && knightCircle_right_u.z == pawnC[7].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == knightC[0].y && knightCircle_right_u.z == knightC[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == knightC[1].y && knightCircle_right_u.z == knightC[1].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == bishopC[0].y && knightCircle_right_u.z == bishopC[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == bishopC[1].y && knightCircle_right_u.z == bishopC[1].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == rookC[0].y && knightCircle_right_u.z == rookC[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == rookC[1].y && knightCircle_right_u.z == rookC[1].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == queenC.y && knightCircle_right_u.z == queenC.z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == kingC.y && knightCircle_right_u.z == kingC.z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else {
        if (knightCircle_right_u.z <= 2 && knightCircle_right_u.y <= 1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_right_u.x, knightCircle_right_u.y, knightCircle_right_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_left_d.y == pawnC[0].y && knightCircle_left_d.z == pawnC[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if ((knightCircle_left_d.y == pawnC[1].y && knightCircle_left_d.z == pawnC[1].z)) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnC[2].y && knightCircle_left_d.z == pawnC[2].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnC[3].y && knightCircle_left_d.z == pawnC[3].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnC[4].y && knightCircle_left_d.z == pawnC[4].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnC[5].y && knightCircle_left_d.z == pawnC[5].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnC[6].y && knightCircle_left_d.z == pawnC[6].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnC[7].y && knightCircle_left_d.z == pawnC[7].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == knightC[0].y && knightCircle_left_d.z == knightC[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == knightC[1].y && knightCircle_left_d.z == knightC[1].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == bishopC[0].y && knightCircle_left_d.z == bishopC[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == bishopC[1].y && knightCircle_left_d.z == bishopC[1].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == rookC[0].y && knightCircle_left_d.z == rookC[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == rookC[1].y && knightCircle_left_d.z == rookC[1].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == queenC.y && knightCircle_left_d.z == queenC.z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == kingC.y && knightCircle_left_d.z == kingC.z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else {
        if (knightCircle_left_d.z >= -1.5 && knightCircle_left_d.y >= -2) {
            glPushMatrix();
            glTranslatef(knightCircle_left_d.x, knightCircle_left_d.y, knightCircle_left_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_right_d.y == pawnC[0].y && knightCircle_right_d.z == pawnC[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if ((knightCircle_right_d.y == pawnC[1].y && knightCircle_right_d.z == pawnC[1].z)) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnC[2].y && knightCircle_right_d.z == pawnC[2].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnC[3].y && knightCircle_right_d.z == pawnC[3].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnC[4].y && knightCircle_right_d.z == pawnC[4].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnC[5].y && knightCircle_right_d.z == pawnC[5].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnC[6].y && knightCircle_right_d.z == pawnC[6].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnC[7].y && knightCircle_right_d.z == pawnC[7].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == knightC[0].y && knightCircle_right_d.z == knightC[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == knightC[1].y && knightCircle_right_d.z == knightC[1].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == bishopC[0].y && knightCircle_right_d.z == bishopC[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == bishopC[1].y && knightCircle_right_d.z == bishopC[1].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == rookC[0].y && knightCircle_right_d.z == rookC[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == rookC[1].y && knightCircle_right_d.z == rookC[1].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == queenC.y && knightCircle_right_d.z == queenC.z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == kingC.y && knightCircle_right_d.z == kingC.z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else {
        if (knightCircle_right_d.z >= -1.5 && knightCircle_right_d.y <= 1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_right_d.x, knightCircle_right_d.y, knightCircle_right_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
}
void move_Pawn_Circle_W(int x) {
    pawnchk = x;
    float a = 0.5;
    float b = 1.0;
    pawnCircle_front[0].x = a;
    pawnCircle_front[0].y = pawnC[x].y;
    pawnCircle_front[0].z = pawnC[x].z + a;

    pawnCircle_front[1].x = a;
    pawnCircle_front[1].y = pawnC[x].y;
    pawnCircle_front[1].z = pawnC[x].z + b;

    pawnCircle_front_l.x = a;
    pawnCircle_front_l.y = pawnC[x].y - a;
    pawnCircle_front_l.z = pawnC[x].z + a;

    pawnCircle_front_r.x = a;
    pawnCircle_front_r.y = pawnC[x].y + a;
    pawnCircle_front_r.z = pawnC[x].z + a;

   


    if (pawnCircle_front[0].y == pawnC[0].y && pawnCircle_front[0].z == pawnC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if ((pawnCircle_front[0].y == pawnC[1].y && pawnCircle_front[0].z == pawnC[1].z)) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[2].y && pawnCircle_front[0].z == pawnC[2].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[3].y && pawnCircle_front[0].z == pawnC[3].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[4].y && pawnCircle_front[0].z == pawnC[4].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[5].y && pawnCircle_front[0].z == pawnC[5].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[6].y && pawnCircle_front[0].z == pawnC[6].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[7].y && pawnCircle_front[0].z == pawnC[7].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightC[0].y && pawnCircle_front[0].z == knightC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightC[1].y && pawnCircle_front[0].z == knightC[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopC[0].y && pawnCircle_front[0].z == bishopC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopC[1].y && pawnCircle_front[0].z == bishopC[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookC[0].y && pawnCircle_front[0].z == rookC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookC[1].y && pawnCircle_front[0].z == rookC[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == queenC.y && pawnCircle_front[0].z == queenC.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == kingC.y && pawnCircle_front[0].z == kingC.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[0].y && pawnCircle_front[0].z == pawnD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if ((pawnCircle_front[0].y == pawnD[1].y && pawnCircle_front[0].z == pawnD[1].z)) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[2].y && pawnCircle_front[0].z == pawnD[2].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[3].y && pawnCircle_front[0].z == pawnD[3].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[4].y && pawnCircle_front[0].z == pawnD[4].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[5].y && pawnCircle_front[0].z == pawnD[5].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[6].y && pawnCircle_front[0].z == pawnD[6].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[7].y && pawnCircle_front[0].z == pawnD[7].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightD[0].y && pawnCircle_front[0].z == knightD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightD[1].y && pawnCircle_front[0].z == knightD[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopD[0].y && pawnCircle_front[0].z == bishopD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopD[1].y && pawnCircle_front[0].z == bishopD[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookD[0].y && pawnCircle_front[0].z == rookD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookD[1].y && pawnCircle_front[0].z == rookD[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == queenD.y && pawnCircle_front[0].z == queenD.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == kingD.y && pawnCircle_front[0].z == kingD.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else {
        if (pawnC[x].z + a <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front[0].x, pawnCircle_front[0].y, pawnCircle_front[0].z);
            draw_move_Circle();
            glPopMatrix();

            if (pawn_moved_W[x] == 0) {
                if (pawnCircle_front[1].y == pawnC[0].y && pawnCircle_front[1].z == pawnC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if ((pawnCircle_front[1].y == pawnC[1].y && pawnCircle_front[1].z == pawnC[1].z)) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[2].y && pawnCircle_front[1].z == pawnC[2].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[3].y && pawnCircle_front[1].z == pawnC[3].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[4].y && pawnCircle_front[1].z == pawnC[4].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[5].y && pawnCircle_front[1].z == pawnC[5].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[6].y && pawnCircle_front[1].z == pawnC[6].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[7].y && pawnCircle_front[1].z == pawnC[7].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightC[0].y && pawnCircle_front[1].z == knightC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightC[1].y && pawnCircle_front[1].z == knightC[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopC[0].y && pawnCircle_front[1].z == bishopC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopC[1].y && pawnCircle_front[1].z == bishopC[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookC[0].y && pawnCircle_front[1].z == rookC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookC[1].y && pawnCircle_front[1].z == rookC[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == queenC.y && pawnCircle_front[1].z == queenC.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == kingC.y && pawnCircle_front[1].z == kingC.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[0].y && pawnCircle_front[1].z == pawnD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[1].y && pawnCircle_front[1].z == pawnD[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[2].y && pawnCircle_front[1].z == pawnD[2].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[3].y && pawnCircle_front[1].z == pawnD[3].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[4].y && pawnCircle_front[1].z == pawnD[4].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[5].y && pawnCircle_front[1].z == pawnD[5].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[6].y && pawnCircle_front[1].z == pawnD[6].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[7].y && pawnCircle_front[1].z == pawnD[7].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightD[0].y && pawnCircle_front[1].z == knightD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightD[1].y && pawnCircle_front[1].z == knightD[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopD[0].y && pawnCircle_front[1].z == bishopD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopD[1].y && pawnCircle_front[1].z == bishopD[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookD[0].y && pawnCircle_front[1].z == rookD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookD[1].y && pawnCircle_front[1].z == rookD[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == queenD.y && pawnCircle_front[1].z == queenD.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == kingD.y && pawnCircle_front[1].z == kingD.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else {
                    glPushMatrix();
                    glTranslatef(pawnCircle_front[1].x, pawnCircle_front[1].y, pawnCircle_front[1].z);
                    draw_move_Circle();
                    glPopMatrix();
                }
            }
        }
    }

    if (pawnCircle_front_l.y == pawnD[0].y && pawnCircle_front_l.z == pawnD[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if ((pawnCircle_front_l.y == pawnD[1].y && pawnCircle_front_l.z == pawnD[1].z)) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnD[2].y && pawnCircle_front_l.z == pawnD[2].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnD[3].y && pawnCircle_front_l.z == pawnD[3].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnD[4].y && pawnCircle_front_l.z == pawnD[4].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnD[5].y && pawnCircle_front_l.z == pawnD[5].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnD[6].y && pawnCircle_front_l.z == pawnD[6].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnD[7].y && pawnCircle_front_l.z == pawnD[7].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == knightD[0].y && pawnCircle_front_l.z == knightD[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == knightD[1].y && pawnCircle_front_l.z == knightD[1].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == bishopD[0].y && pawnCircle_front_l.z == bishopD[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == bishopD[1].y && pawnCircle_front_l.z == bishopD[1].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == rookD[0].y && pawnCircle_front_l.z == rookD[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == rookD[1].y && pawnCircle_front_l.z == rookD[1].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == queenD.y && pawnCircle_front_l.z == queenD.z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == kingD.y && pawnCircle_front_l.z == kingD.z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else {
        pawnCircle_front_l.y = 100;
        pawnCircle_front_l.z = 100;
    }

    if (pawnCircle_front_r.y == pawnD[0].y && pawnCircle_front_r.z == pawnD[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if ((pawnCircle_front_r.y == pawnD[1].y && pawnCircle_front_r.z == pawnD[1].z)) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnD[2].y && pawnCircle_front_r.z == pawnD[2].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnD[3].y && pawnCircle_front_r.z == pawnD[3].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnD[4].y && pawnCircle_front_r.z == pawnD[4].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnD[5].y && pawnCircle_front_r.z == pawnD[5].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnD[6].y && pawnCircle_front_r.z == pawnD[6].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnD[7].y && pawnCircle_front_r.z == pawnD[7].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == knightD[0].y && pawnCircle_front_r.z == knightD[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == knightD[1].y && pawnCircle_front_r.z == knightD[1].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == bishopD[0].y && pawnCircle_front_r.z == bishopD[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == bishopD[1].y && pawnCircle_front_r.z == bishopD[1].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == rookD[0].y && pawnCircle_front_r.z == rookD[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == rookD[1].y && pawnCircle_front_r.z == rookD[1].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == queenD.y && pawnCircle_front_r.z == queenD.z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == kingD.y && pawnCircle_front_r.z == kingD.z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else {
        pawnCircle_front_r.y = 100;
        pawnCircle_front_r.z = 100;
    }
}

void move_King_Circle_B() {
    float a = 0.5;
    kingCircle_front = { a, kingD.y, kingD.z + a };
    kingCircle_behind = { a, kingD.y, kingD.z - a };
    kingCircle_left = { a, kingD.y - a, kingD.z };
    kingCircle_right = { a, kingD.y + a, kingD.z };
    kingCircle_left_u = { a, kingD.y - a, kingD.z + a };
    kingCircle_left_d = { a, kingD.y - a, kingD.z - a };
    kingCircle_right_u = { a, kingD.y + a, kingD.z + a };
    kingCircle_right_d = { a, kingD.y + a, kingD.z - a };

    if (kingCircle_right.y == pawnD[0].y && kingCircle_right.z == pawnD[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if ((kingCircle_right.y == pawnD[1].y && kingCircle_right.z == pawnD[1].z)) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnD[2].y && kingCircle_right.z == pawnD[2].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnD[3].y && kingCircle_right.z == pawnD[3].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnD[4].y && kingCircle_right.z == pawnD[4].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnD[5].y && kingCircle_right.z == pawnD[5].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnD[6].y && kingCircle_right.z == pawnD[6].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == pawnD[7].y && kingCircle_right.z == pawnD[7].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == knightD[0].y && kingCircle_right.z == knightD[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == knightD[1].y && kingCircle_right.z == knightD[1].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == bishopD[0].y && kingCircle_right.z == bishopD[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == bishopD[1].y && kingCircle_right.z == bishopD[1].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == rookD[0].y && kingCircle_right.z == rookD[0].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == rookD[1].y && kingCircle_right.z == rookD[1].z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else if (kingCircle_right.y == queenD.y && kingCircle_right.z == queenD.z) {
        kingCircle_right.y = 100;
        kingCircle_right.z = 100;
    }
    else {
        if (kingCircle_right.y <= 1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_right.x, kingCircle_right.y, kingCircle_right.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_left.y == pawnD[0].y && kingCircle_left.z == pawnD[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if ((kingCircle_left.y == pawnD[1].y && kingCircle_left.z == pawnD[1].z)) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnD[2].y && kingCircle_left.z == pawnD[2].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnD[3].y && kingCircle_left.z == pawnD[3].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnD[4].y && kingCircle_left.z == pawnD[4].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnD[5].y && kingCircle_left.z == pawnD[5].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnD[6].y && kingCircle_left.z == pawnD[6].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == pawnD[7].y && kingCircle_left.z == pawnD[7].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == knightD[0].y && kingCircle_left.z == knightD[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == knightD[1].y && kingCircle_left.z == knightD[1].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == bishopD[0].y && kingCircle_left.z == bishopD[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == bishopD[1].y && kingCircle_left.z == bishopD[1].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == rookD[0].y && kingCircle_left.z == rookD[0].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == rookD[1].y && kingCircle_left.z == rookD[1].z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else if (kingCircle_left.y == queenD.y && kingCircle_left.z == queenD.z) {
        kingCircle_left.y = 100;
        kingCircle_left.z = 100;
    }
    else {
        if (kingCircle_left.y >= -2) {
            glPushMatrix();
            glTranslatef(kingCircle_left.x, kingCircle_left.y, kingCircle_left.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_front.y == pawnD[0].y && kingCircle_front.z == pawnD[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if ((kingCircle_front.y == pawnD[1].y && kingCircle_front.z == pawnD[1].z)) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnD[2].y && kingCircle_front.z == pawnD[2].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnD[3].y && kingCircle_front.z == pawnD[3].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnD[4].y && kingCircle_front.z == pawnD[4].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnD[5].y && kingCircle_front.z == pawnD[5].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnD[6].y && kingCircle_front.z == pawnD[6].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == pawnD[7].y && kingCircle_front.z == pawnD[7].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == knightD[0].y && kingCircle_front.z == knightD[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == knightD[1].y && kingCircle_front.z == knightD[1].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == bishopD[0].y && kingCircle_front.z == bishopD[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == bishopD[1].y && kingCircle_front.z == bishopD[1].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == rookD[0].y && kingCircle_front.z == rookD[0].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == rookD[1].y && kingCircle_front.z == rookD[1].z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else if (kingCircle_front.y == queenD.y && kingCircle_front.z == queenD.z) {
        kingCircle_front.y = 100;
        kingCircle_front.z = 100;
    }
    else {
        if (kingCircle_front.z <= 2) {
            glPushMatrix();
            glTranslatef(kingCircle_front.x, kingCircle_front.y, kingCircle_front.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_behind.y == pawnD[0].y && kingCircle_behind.z == pawnD[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if ((kingCircle_behind.y == pawnD[1].y && kingCircle_behind.z == pawnD[1].z)) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnD[2].y && kingCircle_behind.z == pawnD[2].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnD[3].y && kingCircle_behind.z == pawnD[3].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnD[4].y && kingCircle_behind.z == pawnD[4].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnD[5].y && kingCircle_behind.z == pawnD[5].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnD[6].y && kingCircle_behind.z == pawnD[6].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == pawnD[7].y && kingCircle_behind.z == pawnD[7].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == knightD[0].y && kingCircle_behind.z == knightD[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == knightD[1].y && kingCircle_behind.z == knightD[1].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == bishopD[0].y && kingCircle_behind.z == bishopD[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == bishopD[1].y && kingCircle_behind.z == bishopD[1].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == rookD[0].y && kingCircle_behind.z == rookD[0].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == rookD[1].y && kingCircle_behind.z == rookD[1].z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else if (kingCircle_behind.y == queenD.y && kingCircle_behind.z == queenD.z) {
        kingCircle_behind.y = 100;
        kingCircle_behind.z = 100;
    }
    else {
        if (kingCircle_behind.z >= -1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_behind.x, kingCircle_behind.y, kingCircle_behind.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_left_d.y == pawnD[0].y && kingCircle_left_d.z == pawnD[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if ((kingCircle_left_d.y == pawnD[1].y && kingCircle_left_d.z == pawnD[1].z)) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnD[2].y && kingCircle_left_d.z == pawnD[2].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnD[3].y && kingCircle_left_d.z == pawnD[3].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnD[4].y && kingCircle_left_d.z == pawnD[4].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnD[5].y && kingCircle_left_d.z == pawnD[5].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnD[6].y && kingCircle_left_d.z == pawnD[6].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == pawnD[7].y && kingCircle_left_d.z == pawnD[7].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == knightD[0].y && kingCircle_left_d.z == knightD[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == knightD[1].y && kingCircle_left_d.z == knightD[1].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == bishopD[0].y && kingCircle_left_d.z == bishopD[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == bishopD[1].y && kingCircle_left_d.z == bishopD[1].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == rookD[0].y && kingCircle_left_d.z == rookD[0].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == rookD[1].y && kingCircle_left_d.z == rookD[1].z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else if (kingCircle_left_d.y == queenD.y && kingCircle_left_d.z == queenD.z) {
        kingCircle_left_d.y = 100;
        kingCircle_left_d.z = 100;
    }
    else {
        if (kingCircle_left_d.y >= -2 && kingCircle_left_d.z >= -1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_left_d.x, kingCircle_left_d.y, kingCircle_left_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_right_d.y == pawnD[0].y && kingCircle_right_d.z == pawnD[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if ((kingCircle_right_d.y == pawnD[1].y && kingCircle_right_d.z == pawnD[1].z)) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnD[2].y && kingCircle_right_d.z == pawnD[2].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnD[3].y && kingCircle_right_d.z == pawnD[3].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnD[4].y && kingCircle_right_d.z == pawnD[4].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnD[5].y && kingCircle_right_d.z == pawnD[5].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnD[6].y && kingCircle_right_d.z == pawnD[6].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == pawnD[7].y && kingCircle_right_d.z == pawnD[7].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == knightD[0].y && kingCircle_right_d.z == knightD[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == knightD[1].y && kingCircle_right_d.z == knightD[1].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == bishopD[0].y && kingCircle_right_d.z == bishopD[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == bishopD[1].y && kingCircle_right_d.z == bishopD[1].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == rookD[0].y && kingCircle_right_d.z == rookD[0].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == rookD[1].y && kingCircle_right_d.z == rookD[1].z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else if (kingCircle_right_d.y == queenD.y && kingCircle_right_d.z == queenD.z) {
        kingCircle_right_d.y = 100;
        kingCircle_right_d.z = 100;
    }
    else {
        if (kingCircle_right_d.y <= 1.5 && kingCircle_right_d.z >= -1.5) {
            glPushMatrix();
            glTranslatef(kingCircle_right_d.x, kingCircle_right_d.y, kingCircle_right_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (kingCircle_left_u.y == pawnD[0].y && kingCircle_left_u.z == pawnD[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if ((kingCircle_left_u.y == pawnD[1].y && kingCircle_left_u.z == pawnD[1].z)) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnD[2].y && kingCircle_left_u.z == pawnD[2].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnD[3].y && kingCircle_left_u.z == pawnD[3].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnD[4].y && kingCircle_left_u.z == pawnD[4].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnD[5].y && kingCircle_left_u.z == pawnD[5].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnD[6].y && kingCircle_left_u.z == pawnD[6].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == pawnD[7].y && kingCircle_left_u.z == pawnD[7].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == knightD[0].y && kingCircle_left_u.z == knightD[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == knightD[1].y && kingCircle_left_u.z == knightD[1].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == bishopD[0].y && kingCircle_left_u.z == bishopD[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == bishopD[1].y && kingCircle_left_u.z == bishopD[1].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == rookD[0].y && kingCircle_left_u.z == rookD[0].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == rookD[1].y && kingCircle_left_u.z == rookD[1].z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else if (kingCircle_left_u.y == queenD.y && kingCircle_left_u.z == queenD.z) {
        kingCircle_left_u.y = 100;
        kingCircle_left_u.z = 100;
    }
    else {
        if (kingCircle_left_u.y >= -2 && kingCircle_left_u.z <= 2) {
            glPushMatrix();
            glTranslatef(kingCircle_left_u.x, kingCircle_left_u.y, kingCircle_left_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }


    if (kingCircle_right_u.y == pawnD[0].y && kingCircle_right_u.z == pawnD[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if ((kingCircle_right_u.y == pawnD[1].y && kingCircle_right_u.z == pawnD[1].z)) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnD[2].y && kingCircle_right_u.z == pawnD[2].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnD[3].y && kingCircle_right_u.z == pawnD[3].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnD[4].y && kingCircle_right_u.z == pawnD[4].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnD[5].y && kingCircle_right_u.z == pawnD[5].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnD[6].y && kingCircle_right_u.z == pawnD[6].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == pawnD[7].y && kingCircle_right_u.z == pawnD[7].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == knightD[0].y && kingCircle_right_u.z == knightD[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == knightD[1].y && kingCircle_right_u.z == knightD[1].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == bishopD[0].y && kingCircle_right_u.z == bishopD[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == bishopD[1].y && kingCircle_right_u.z == bishopD[1].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == rookD[0].y && kingCircle_right_u.z == rookD[0].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == rookD[1].y && kingCircle_right_u.z == rookD[1].z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else if (kingCircle_right_u.y == queenD.y && kingCircle_right_u.z == queenD.z) {
        kingCircle_right_u.y = 100;
        kingCircle_right_u.z = 100;
    }
    else {
        if (kingCircle_right_u.y <= 1.5 && kingCircle_right_u.z <= 2) {
            glPushMatrix();
            glTranslatef(kingCircle_right_u.x, kingCircle_right_u.y, kingCircle_right_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
}
void move_Queen_Circle_B() {
    float a = 0.5;
    float b = 0.5;
    int  j = 0;
    for (int i = 0; i < 8; i++) {
        queenCircle_front[i] = { a, queenD.y, queenD.z + b };
        queenCircle_right[i] = { a, queenD.y + b, queenD.z };
        queenCircle_left[i] = { a, queenD.y - b, queenD.z };
        queenCircle_behind[i] = { a, queenD.y, queenD.z - b };
        queenCircle_left_u[i] = { a, queenD.y - b, queenD.z + b };
        queenCircle_left_d[i] = { a, queenD.y - b, queenD.z - b };
        queenCircle_right_u[i] = { a, queenD.y + b, queenD.z + b };
        queenCircle_right_d[i] = { a, queenD.y + b, queenD.z - b };

        b += 0.5;
    }

    for (float i = 0.5; queenD.y + i <= 1.5; i += 0.5) {

        if (queenCircle_right[j].y == pawnD[0].y && queenCircle_right[j].z == pawnD[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if ((queenCircle_right[j].y == pawnD[1].y && queenCircle_right[j].z == pawnD[1].z)) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnD[2].y && queenCircle_right[j].z == pawnD[2].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnD[3].y && queenCircle_right[j].z == pawnD[3].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnD[4].y && queenCircle_right[j].z == pawnD[4].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnD[5].y && queenCircle_right[j].z == pawnD[5].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnD[6].y && queenCircle_right[j].z == pawnD[6].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == pawnD[7].y && queenCircle_right[j].z == pawnD[7].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == knightD[0].y && queenCircle_right[j].z == knightD[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == knightD[1].y && queenCircle_right[j].z == knightD[1].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == bishopD[0].y && queenCircle_right[j].z == bishopD[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == bishopD[1].y && queenCircle_right[j].z == bishopD[1].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == rookD[0].y && queenCircle_right[j].z == rookD[0].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == rookD[1].y && queenCircle_right[j].z == rookD[1].z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else if (queenCircle_right[j].y == kingD.y && queenCircle_right[j].z == kingD.z) {
            queenCircle_right[j].y = 100;
            queenCircle_right[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_right[j].x, queenCircle_right[j].y, queenCircle_right[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenD.y - i >= -2; i += 0.5) {
        if (queenCircle_left[j].y == pawnD[0].y && queenCircle_left[j].z == pawnD[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if ((queenCircle_left[j].y == pawnD[1].y && queenCircle_left[j].z == pawnD[1].z)) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnD[2].y && queenCircle_left[j].z == pawnD[2].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnD[3].y && queenCircle_left[j].z == pawnD[3].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnD[4].y && queenCircle_left[j].z == pawnD[4].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnD[5].y && queenCircle_left[j].z == pawnD[5].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnD[6].y && queenCircle_left[j].z == pawnD[6].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == pawnD[7].y && queenCircle_left[j].z == pawnD[7].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == knightD[0].y && queenCircle_left[j].z == knightD[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == knightD[1].y && queenCircle_left[j].z == knightD[1].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == bishopD[0].y && queenCircle_left[j].z == bishopD[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == bishopD[1].y && queenCircle_left[j].z == bishopD[1].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == rookD[0].y && queenCircle_left[j].z == rookD[0].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == rookD[1].y && queenCircle_left[j].z == rookD[1].z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else if (queenCircle_left[j].y == kingD.y && queenCircle_left[j].z == kingD.z) {
            queenCircle_left[j].y = 100;
            queenCircle_left[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_left[j].x, queenCircle_left[j].y, queenCircle_left[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenD.z + i <= 2; i += 0.5) {
        if (queenCircle_front[j].y == pawnD[0].y && queenCircle_front[j].z == pawnD[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if ((queenCircle_front[j].y == pawnD[1].y && queenCircle_front[j].z == pawnD[1].z)) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnD[2].y && queenCircle_front[j].z == pawnD[2].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnD[3].y && queenCircle_front[j].z == pawnD[3].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnD[4].y && queenCircle_front[j].z == pawnD[4].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnD[5].y && queenCircle_front[j].z == pawnD[5].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnD[6].y && queenCircle_front[j].z == pawnD[6].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == pawnD[7].y && queenCircle_front[j].z == pawnD[7].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == knightD[0].y && queenCircle_front[j].z == knightD[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == knightD[1].y && queenCircle_front[j].z == knightD[1].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == bishopD[0].y && queenCircle_front[j].z == bishopD[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == bishopD[1].y && queenCircle_front[j].z == bishopD[1].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == rookD[0].y && queenCircle_front[j].z == rookD[0].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == rookD[1].y && queenCircle_front[j].z == rookD[1].z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else if (queenCircle_front[j].y == kingD.y && queenCircle_front[j].z == kingD.z) {
            queenCircle_front[j].y = 100;
            queenCircle_front[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_front[j].x, queenCircle_front[j].y, queenCircle_front[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenD.z - i >= -1.5; i += 0.5) {
        if (queenCircle_behind[j].y == pawnD[0].y && queenCircle_behind[j].z == pawnD[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if ((queenCircle_behind[j].y == pawnD[1].y && queenCircle_behind[j].z == pawnD[1].z)) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnD[2].y && queenCircle_behind[j].z == pawnD[2].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnD[3].y && queenCircle_behind[j].z == pawnD[3].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnD[4].y && queenCircle_behind[j].z == pawnD[4].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnD[5].y && queenCircle_behind[j].z == pawnD[5].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnD[6].y && queenCircle_behind[j].z == pawnD[6].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == pawnD[7].y && queenCircle_behind[j].z == pawnD[7].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == knightD[0].y && queenCircle_behind[j].z == knightD[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == knightD[1].y && queenCircle_behind[j].z == knightD[1].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == bishopD[0].y && queenCircle_behind[j].z == bishopD[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == bishopD[1].y && queenCircle_behind[j].z == bishopD[1].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == rookD[0].y && queenCircle_behind[j].z == rookD[0].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == rookD[1].y && queenCircle_behind[j].z == rookD[1].z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else if (queenCircle_behind[j].y == kingD.y && queenCircle_behind[j].z == kingD.z) {
            queenCircle_behind[j].y = 100;
            queenCircle_behind[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_behind[j].x, queenCircle_behind[j].y, queenCircle_behind[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenD.y - i >= -2 && queenD.z - i >= -1.5; i += 0.5) {
        if (queenCircle_left_d[j].y == pawnD[0].y && queenCircle_left_d[j].z == pawnD[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if ((queenCircle_left_d[j].y == pawnD[1].y && queenCircle_left_d[j].z == pawnD[1].z)) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnD[2].y && queenCircle_left_d[j].z == pawnD[2].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnD[3].y && queenCircle_left_d[j].z == pawnD[3].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnD[4].y && queenCircle_left_d[j].z == pawnD[4].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnD[5].y && queenCircle_left_d[j].z == pawnD[5].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnD[6].y && queenCircle_left_d[j].z == pawnD[6].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == pawnD[7].y && queenCircle_left_d[j].z == pawnD[7].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == knightD[0].y && queenCircle_left_d[j].z == knightD[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == knightD[1].y && queenCircle_left_d[j].z == knightD[1].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == bishopD[0].y && queenCircle_left_d[j].z == bishopD[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == bishopD[1].y && queenCircle_left_d[j].z == bishopD[1].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == rookD[0].y && queenCircle_left_d[j].z == rookD[0].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == rookD[1].y && queenCircle_left_d[j].z == rookD[1].z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else if (queenCircle_left_d[j].y == kingD.y && queenCircle_left_d[j].z == kingD.z) {
            queenCircle_left_d[j].y = 100;
            queenCircle_left_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_left_d[j].x, queenCircle_left_d[j].y, queenCircle_left_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenD.y + i <= 1.5 && queenD.z - i >= -1.5; i += 0.5) {
        if (queenCircle_right_d[j].y == pawnD[0].y && queenCircle_right_d[j].z == pawnD[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if ((queenCircle_right_d[j].y == pawnD[1].y && queenCircle_right_d[j].z == pawnD[1].z)) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnD[2].y && queenCircle_right_d[j].z == pawnD[2].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnD[3].y && queenCircle_right_d[j].z == pawnD[3].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnD[4].y && queenCircle_right_d[j].z == pawnD[4].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnD[5].y && queenCircle_right_d[j].z == pawnD[5].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnD[6].y && queenCircle_right_d[j].z == pawnD[6].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == pawnD[7].y && queenCircle_right_d[j].z == pawnD[7].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == knightD[0].y && queenCircle_right_d[j].z == knightD[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == knightD[1].y && queenCircle_right_d[j].z == knightD[1].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == bishopD[0].y && queenCircle_right_d[j].z == bishopD[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == bishopD[1].y && queenCircle_right_d[j].z == bishopD[1].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == rookD[0].y && queenCircle_right_d[j].z == rookD[0].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == rookD[1].y && queenCircle_right_d[j].z == rookD[1].z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else if (queenCircle_right_d[j].y == kingD.y && queenCircle_right_d[j].z == kingD.z) {
            queenCircle_right_d[j].y = 100;
            queenCircle_right_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_right_d[j].x, queenCircle_right_d[j].y, queenCircle_right_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenD.y - i >= -2 && queenD.z + i <= 2; i += 0.5) {
        if (queenCircle_left_u[j].y == pawnD[0].y && queenCircle_left_u[j].z == pawnD[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if ((queenCircle_left_u[j].y == pawnD[1].y && queenCircle_left_u[j].z == pawnD[1].z)) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnD[2].y && queenCircle_left_u[j].z == pawnD[2].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnD[3].y && queenCircle_left_u[j].z == pawnD[3].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnD[4].y && queenCircle_left_u[j].z == pawnD[4].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnD[5].y && queenCircle_left_u[j].z == pawnD[5].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnD[6].y && queenCircle_left_u[j].z == pawnD[6].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == pawnD[7].y && queenCircle_left_u[j].z == pawnD[7].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == knightD[0].y && queenCircle_left_u[j].z == knightD[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == knightD[1].y && queenCircle_left_u[j].z == knightD[1].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == bishopD[0].y && queenCircle_left_u[j].z == bishopD[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == bishopD[1].y && queenCircle_left_u[j].z == bishopD[1].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == rookD[0].y && queenCircle_left_u[j].z == rookD[0].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == rookD[1].y && queenCircle_left_u[j].z == rookD[1].z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else if (queenCircle_left_u[j].y == kingD.y && queenCircle_left_u[j].z == kingD.z) {
            queenCircle_left_u[j].y = 100;
            queenCircle_left_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_left_u[j].x, queenCircle_left_u[j].y, queenCircle_left_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; queenD.y + i <= 1.5 && queenD.z + i <= 2; i += 0.5) {
        if (queenCircle_right_u[j].y == pawnD[0].y && queenCircle_right_u[j].z == pawnD[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if ((queenCircle_right_u[j].y == pawnD[1].y && queenCircle_right_u[j].z == pawnD[1].z)) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnD[2].y && queenCircle_right_u[j].z == pawnD[2].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnD[3].y && queenCircle_right_u[j].z == pawnD[3].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnD[4].y && queenCircle_right_u[j].z == pawnD[4].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnD[5].y && queenCircle_right_u[j].z == pawnD[5].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnD[6].y && queenCircle_right_u[j].z == pawnD[6].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == pawnD[7].y && queenCircle_right_u[j].z == pawnD[7].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == knightD[0].y && queenCircle_right_u[j].z == knightD[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == knightD[1].y && queenCircle_right_u[j].z == knightD[1].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == bishopD[0].y && queenCircle_right_u[j].z == bishopD[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == bishopD[1].y && queenCircle_right_u[j].z == bishopD[1].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == rookD[0].y && queenCircle_right_u[j].z == rookD[0].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == rookD[1].y && queenCircle_right_u[j].z == rookD[1].z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else if (queenCircle_right_u[j].y == kingD.y && queenCircle_right_u[j].z == kingD.z) {
            queenCircle_right_u[j].y = 100;
            queenCircle_right_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(queenCircle_right_u[j].x, queenCircle_right_u[j].y, queenCircle_right_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
}
void move_Bishop_Circle_B(int x) {
    float a = 0.5;
    float b = 0.5;
    int  j = 0;
    for (int i = 0; i < 8; i++) {
        bishopCircle_left_u[i] = { a, bishopD[x].y - b, bishopD[x].z + b };
        bishopCircle_left_d[i] = { a, bishopD[x].y - b, bishopD[x].z - b };
        bishopCircle_right_u[i] = { a, bishopD[x].y + b, bishopD[x].z + b };
        bishopCircle_right_d[i] = { a, bishopD[x].y + b, bishopD[x].z - b };

        b += 0.5;
    }

    for (float i = 0.5; bishopD[x].y - i >= -2 && bishopD[x].z - i >= -1.5; i += 0.5) {
        if (bishopCircle_left_d[j].y == pawnD[0].y && bishopCircle_left_d[j].z == pawnD[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if ((bishopCircle_left_d[j].y == pawnD[1].y && bishopCircle_left_d[j].z == pawnD[1].z)) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnD[2].y && bishopCircle_left_d[j].z == pawnD[2].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnD[3].y && bishopCircle_left_d[j].z == pawnD[3].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnD[4].y && bishopCircle_left_d[j].z == pawnD[4].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnD[5].y && bishopCircle_left_d[j].z == pawnD[5].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnD[6].y && bishopCircle_left_d[j].z == pawnD[6].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == pawnD[7].y && bishopCircle_left_d[j].z == pawnD[7].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == knightD[0].y && bishopCircle_left_d[j].z == knightD[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == knightD[1].y && bishopCircle_left_d[j].z == knightD[1].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == bishopD[0].y && bishopCircle_left_d[j].z == bishopD[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == bishopD[1].y && bishopCircle_left_d[j].z == bishopD[1].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == rookD[0].y && bishopCircle_left_d[j].z == rookD[0].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == rookD[1].y && bishopCircle_left_d[j].z == rookD[1].z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == queenD.y && bishopCircle_left_d[j].z == queenD.z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else if (bishopCircle_left_d[j].y == kingD.y && bishopCircle_left_d[j].z == kingD.z) {
            bishopCircle_left_d[j].y = 100;
            bishopCircle_left_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_left_d[j].x, bishopCircle_left_d[j].y, bishopCircle_left_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; bishopD[x].y + i <= 1.5 && bishopD[x].z - i >= -1.5; i += 0.5) {
        if (bishopCircle_right_d[j].y == pawnD[0].y && bishopCircle_right_d[j].z == pawnD[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if ((bishopCircle_right_d[j].y == pawnD[1].y && bishopCircle_right_d[j].z == pawnD[1].z)) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnD[2].y && bishopCircle_right_d[j].z == pawnD[2].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnD[3].y && bishopCircle_right_d[j].z == pawnD[3].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnD[4].y && bishopCircle_right_d[j].z == pawnD[4].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnD[5].y && bishopCircle_right_d[j].z == pawnD[5].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnD[6].y && bishopCircle_right_d[j].z == pawnD[6].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == pawnD[7].y && bishopCircle_right_d[j].z == pawnD[7].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == knightD[0].y && bishopCircle_right_d[j].z == knightD[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == knightD[1].y && bishopCircle_right_d[j].z == knightD[1].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == bishopD[0].y && bishopCircle_right_d[j].z == bishopD[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == bishopD[1].y && bishopCircle_right_d[j].z == bishopD[1].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == rookD[0].y && bishopCircle_right_d[j].z == rookD[0].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == rookD[1].y && bishopCircle_right_d[j].z == rookD[1].z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == queenD.y && bishopCircle_right_d[j].z == queenD.z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else if (bishopCircle_right_d[j].y == kingD.y && bishopCircle_right_d[j].z == kingD.z) {
            bishopCircle_right_d[j].y = 100;
            bishopCircle_right_d[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_right_d[j].x, bishopCircle_right_d[j].y, bishopCircle_right_d[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; bishopD[x].y - i >= -2 && bishopD[x].z + i <= 2; i += 0.5) {
        if (bishopCircle_left_u[j].y == pawnD[0].y && bishopCircle_left_u[j].z == pawnD[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if ((bishopCircle_left_u[j].y == pawnD[1].y && bishopCircle_left_u[j].z == pawnD[1].z)) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnD[2].y && bishopCircle_left_u[j].z == pawnD[2].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnD[3].y && bishopCircle_left_u[j].z == pawnD[3].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnD[4].y && bishopCircle_left_u[j].z == pawnD[4].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnD[5].y && bishopCircle_left_u[j].z == pawnD[5].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnD[6].y && bishopCircle_left_u[j].z == pawnD[6].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == pawnD[7].y && bishopCircle_left_u[j].z == pawnD[7].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == knightD[0].y && bishopCircle_left_u[j].z == knightD[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == knightD[1].y && bishopCircle_left_u[j].z == knightD[1].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == bishopD[0].y && bishopCircle_left_u[j].z == bishopD[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == bishopD[1].y && bishopCircle_left_u[j].z == bishopD[1].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == rookD[0].y && bishopCircle_left_u[j].z == rookD[0].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == rookD[1].y && bishopCircle_left_u[j].z == rookD[1].z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == queenD.y && bishopCircle_left_u[j].z == queenD.z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else if (bishopCircle_left_u[j].y == kingD.y && bishopCircle_left_u[j].z == kingD.z) {
            bishopCircle_left_u[j].y = 100;
            bishopCircle_left_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_left_u[j].x, bishopCircle_left_u[j].y, bishopCircle_left_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; bishopD[x].y + i <= 1.5 && bishopD[x].z + i <= 2; i += 0.5) {
        if (bishopCircle_right_u[j].y == pawnD[0].y && bishopCircle_right_u[j].z == pawnD[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if ((bishopCircle_right_u[j].y == pawnD[1].y && bishopCircle_right_u[j].z == pawnD[1].z)) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnD[2].y && bishopCircle_right_u[j].z == pawnD[2].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnD[3].y && bishopCircle_right_u[j].z == pawnD[3].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnD[4].y && bishopCircle_right_u[j].z == pawnD[4].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnD[5].y && bishopCircle_right_u[j].z == pawnD[5].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnD[6].y && bishopCircle_right_u[j].z == pawnD[6].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == pawnD[7].y && bishopCircle_right_u[j].z == pawnD[7].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == knightD[0].y && bishopCircle_right_u[j].z == knightD[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == knightD[1].y && bishopCircle_right_u[j].z == knightD[1].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == bishopD[0].y && bishopCircle_right_u[j].z == bishopD[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == bishopD[1].y && bishopCircle_right_u[j].z == bishopD[1].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == rookD[0].y && bishopCircle_right_u[j].z == rookD[0].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == rookD[1].y && bishopCircle_right_u[j].z == rookD[1].z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == queenD.y && bishopCircle_right_u[j].z == queenD.z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else if (bishopCircle_right_u[j].y == kingD.y && bishopCircle_right_u[j].z == kingD.z) {
            bishopCircle_right_u[j].y = 100;
            bishopCircle_right_u[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(bishopCircle_right_u[j].x, bishopCircle_right_u[j].y, bishopCircle_right_u[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
}
void move_Rook_Circle_B(int x) {
    float a = 0.5;
    float b = 0.5;
    for (int i = 0; i < 8; i++) {
        rookCircle_front[i].x = a;
        rookCircle_front[i].y = rookD[x].y;
        rookCircle_front[i].z = rookD[x].z + b;

        rookCircle_behind[i].x = a;
        rookCircle_behind[i].y = rookD[x].y;
        rookCircle_behind[i].z = rookD[x].z - b;

        rookCircle_right[i].x = a;
        rookCircle_right[i].y = rookD[x].y + b;
        rookCircle_right[i].z = rookD[x].z;

        rookCircle_left[i].x = a;
        rookCircle_left[i].y = rookD[x].y - b;
        rookCircle_left[i].z = rookD[x].z;
        b += 0.5;
    }
    int j = 0;
    for (float i = 0.5; rookD[x].y + i <= 1.5; i += 0.5) {
        if (rookCircle_right[j].y == pawnD[0].y && rookCircle_right[j].z == pawnD[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if ((rookCircle_right[j].y == pawnD[1].y && rookCircle_right[j].z == pawnD[1].z)) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnD[2].y && rookCircle_right[j].z == pawnD[2].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnD[3].y && rookCircle_right[j].z == pawnD[3].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnD[4].y && rookCircle_right[j].z == pawnD[4].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnD[5].y && rookCircle_right[j].z == pawnD[5].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnD[6].y && rookCircle_right[j].z == pawnD[6].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == pawnD[7].y && rookCircle_right[j].z == pawnD[7].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == knightD[0].y && rookCircle_right[j].z == knightD[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == knightD[1].y && rookCircle_right[j].z == knightD[1].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == bishopD[0].y && rookCircle_right[j].z == bishopD[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == bishopD[1].y && rookCircle_right[j].z == bishopD[1].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == rookD[0].y && rookCircle_right[j].z == rookD[0].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == rookD[1].y && rookCircle_right[j].z == rookD[1].z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == queenD.y && rookCircle_right[j].z == queenD.z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else if (rookCircle_right[j].y == kingD.y && rookCircle_right[j].z == kingD.z) {
            rookCircle_right[j].y = 100;
            rookCircle_right[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_right[j].x, rookCircle_right[j].y, rookCircle_right[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; rookD[x].y - i >= -2; i += 0.5) {
        if (rookCircle_left[j].y == pawnD[0].y && rookCircle_left[j].z == pawnD[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if ((rookCircle_left[j].y == pawnD[1].y && rookCircle_left[j].z == pawnD[1].z)) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnD[2].y && rookCircle_left[j].z == pawnD[2].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnD[3].y && rookCircle_left[j].z == pawnD[3].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnD[4].y && rookCircle_left[j].z == pawnD[4].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnD[5].y && rookCircle_left[j].z == pawnD[5].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnD[6].y && rookCircle_left[j].z == pawnD[6].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == pawnD[7].y && rookCircle_left[j].z == pawnD[7].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == knightD[0].y && rookCircle_left[j].z == knightD[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == knightD[1].y && rookCircle_left[j].z == knightD[1].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == bishopD[0].y && rookCircle_left[j].z == bishopD[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == bishopD[1].y && rookCircle_left[j].z == bishopD[1].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == rookD[0].y && rookCircle_left[j].z == rookD[0].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == rookD[1].y && rookCircle_left[j].z == rookD[1].z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == queenD.y && rookCircle_left[j].z == queenD.z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else if (rookCircle_left[j].y == kingD.y && rookCircle_left[j].z == kingD.z) {
            rookCircle_left[j].y = 100;
            rookCircle_left[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_left[j].x, rookCircle_left[j].y, rookCircle_left[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; rookD[x].z + i <= 2; i += 0.5) {
        if (rookCircle_front[j].y == pawnD[0].y && rookCircle_front[j].z == pawnD[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if ((rookCircle_front[j].y == pawnD[1].y && rookCircle_front[j].z == pawnD[1].z)) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnD[2].y && rookCircle_front[j].z == pawnD[2].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnD[3].y && rookCircle_front[j].z == pawnD[3].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnD[4].y && rookCircle_front[j].z == pawnD[4].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnD[5].y && rookCircle_front[j].z == pawnD[5].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnD[6].y && rookCircle_front[j].z == pawnD[6].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == pawnD[7].y && rookCircle_front[j].z == pawnD[7].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == knightD[0].y && rookCircle_front[j].z == knightD[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == knightD[1].y && rookCircle_front[j].z == knightD[1].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == bishopD[0].y && rookCircle_front[j].z == bishopD[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == bishopD[1].y && rookCircle_front[j].z == bishopD[1].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == rookD[0].y && rookCircle_front[j].z == rookD[0].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == rookD[1].y && rookCircle_front[j].z == rookD[1].z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == queenD.y && rookCircle_front[j].z == queenD.z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else if (rookCircle_front[j].y == kingD.y && rookCircle_front[j].z == kingD.z) {
            rookCircle_front[j].y = 100;
            rookCircle_front[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_front[j].x, rookCircle_front[j].y, rookCircle_front[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
    j = 0;
    for (float i = 0.5; rookD[x].z - i >= -1.5; i += 0.5) {
        if (rookCircle_behind[j].y == pawnD[0].y && rookCircle_behind[j].z == pawnD[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if ((rookCircle_behind[j].y == pawnD[1].y && rookCircle_behind[j].z == pawnD[1].z)) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnD[2].y && rookCircle_behind[j].z == pawnD[2].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnD[3].y && rookCircle_behind[j].z == pawnD[3].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnD[4].y && rookCircle_behind[j].z == pawnD[4].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnD[5].y && rookCircle_behind[j].z == pawnD[5].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnD[6].y && rookCircle_behind[j].z == pawnD[6].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == pawnD[7].y && rookCircle_behind[j].z == pawnD[7].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == knightD[0].y && rookCircle_behind[j].z == knightD[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == knightD[1].y && rookCircle_behind[j].z == knightD[1].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == bishopD[0].y && rookCircle_behind[j].z == bishopD[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == bishopD[1].y && rookCircle_behind[j].z == bishopD[1].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == rookD[0].y && rookCircle_behind[j].z == rookD[0].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == rookD[1].y && rookCircle_behind[j].z == rookD[1].z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == queenD.y && rookCircle_behind[j].z == queenD.z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else if (rookCircle_behind[j].y == kingD.y && rookCircle_behind[j].z == kingD.z) {
            rookCircle_behind[j].y = 100;
            rookCircle_behind[j].z = 100;
            break;
        }
        else {
            glPushMatrix();
            glTranslatef(rookCircle_behind[j].x, rookCircle_behind[j].y, rookCircle_behind[j].z);
            draw_move_Circle();
            glPopMatrix();
        }
        j++;
    }
}
void move_Knight_Circle_B(int x) {
    float a = 0.5;
    float b = 1.0;
    knightCircle_front_r = { a, knightD[x].y + a, knightD[x].z + b };
    knightCircle_behind_r = { a, knightD[x].y + a, knightD[x].z - b };
    knightCircle_front_l = { a, knightD[x].y - a, knightD[x].z + b };
    knightCircle_behind_l = { a, knightD[x].y - a, knightD[x].z - b };
    knightCircle_left_u = { a, knightD[x].y - b, knightD[x].z + a };
    knightCircle_right_u = { a, knightD[x].y + b, knightD[x].z + a };
    knightCircle_left_d = { a, knightD[x].y - b, knightD[x].z - a };
    knightCircle_right_d = { a, knightD[x].y + b, knightD[x].z - a };

    if (knightCircle_front_r.y == pawnD[0].y && knightCircle_front_r.z == pawnD[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if ((knightCircle_front_r.y == pawnD[1].y && knightCircle_front_r.z == pawnD[1].z)) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnD[2].y && knightCircle_front_r.z == pawnD[2].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnD[3].y && knightCircle_front_r.z == pawnD[3].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnD[4].y && knightCircle_front_r.z == pawnD[4].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnD[5].y && knightCircle_front_r.z == pawnD[5].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnD[6].y && knightCircle_front_r.z == pawnD[6].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == pawnD[7].y && knightCircle_front_r.z == pawnD[7].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == knightD[0].y && knightCircle_front_r.z == knightD[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == knightD[1].y && knightCircle_front_r.z == knightD[1].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == bishopD[0].y && knightCircle_front_r.z == bishopD[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == bishopD[1].y && knightCircle_front_r.z == bishopD[1].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == rookD[0].y && knightCircle_front_r.z == rookD[0].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == rookD[1].y && knightCircle_front_r.z == rookD[1].z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == queenD.y && knightCircle_front_r.z == queenD.z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else if (knightCircle_front_r.y == kingD.y && knightCircle_front_r.z == kingD.z) {
        knightCircle_front_r.y = 100;
        knightCircle_front_r.z = 100;
    }
    else {
        if (knightCircle_front_r.y <= 1.5 && knightCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(knightCircle_front_r.x, knightCircle_front_r.y, knightCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_behind_r.y == pawnD[0].y && knightCircle_behind_r.z == pawnD[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if ((knightCircle_behind_r.y == pawnD[1].y && knightCircle_behind_r.z == pawnD[1].z)) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnD[2].y && knightCircle_behind_r.z == pawnD[2].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnD[3].y && knightCircle_behind_r.z == pawnD[3].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnD[4].y && knightCircle_behind_r.z == pawnD[4].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnD[5].y && knightCircle_behind_r.z == pawnD[5].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnD[6].y && knightCircle_behind_r.z == pawnD[6].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == pawnD[7].y && knightCircle_behind_r.z == pawnD[7].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == knightD[0].y && knightCircle_behind_r.z == knightD[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == knightD[1].y && knightCircle_behind_r.z == knightD[1].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == bishopD[0].y && knightCircle_behind_r.z == bishopD[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == bishopD[1].y && knightCircle_behind_r.z == bishopD[1].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == rookD[0].y && knightCircle_behind_r.z == rookD[0].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == rookD[1].y && knightCircle_behind_r.z == rookD[1].z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == queenD.y && knightCircle_behind_r.z == queenD.z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else if (knightCircle_behind_r.y == kingD.y && knightCircle_behind_r.z == kingD.z) {
        knightCircle_behind_r.y = 100;
        knightCircle_behind_r.z = 100;
    }
    else {
        if (knightCircle_behind_r.y <= 1.5 && knightCircle_behind_r.z >= -1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_behind_r.x, knightCircle_behind_r.y, knightCircle_behind_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_front_l.y == pawnD[0].y && knightCircle_front_l.z == pawnD[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if ((knightCircle_front_l.y == pawnD[1].y && knightCircle_front_l.z == pawnD[1].z)) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnD[2].y && knightCircle_front_l.z == pawnD[2].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnD[3].y && knightCircle_front_l.z == pawnD[3].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnD[4].y && knightCircle_front_l.z == pawnD[4].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnD[5].y && knightCircle_front_l.z == pawnD[5].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnD[6].y && knightCircle_front_l.z == pawnD[6].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == pawnD[7].y && knightCircle_front_l.z == pawnD[7].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == knightD[0].y && knightCircle_front_l.z == knightD[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == knightD[1].y && knightCircle_front_l.z == knightD[1].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == bishopD[0].y && knightCircle_front_l.z == bishopD[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == bishopD[1].y && knightCircle_front_l.z == bishopD[1].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == rookD[0].y && knightCircle_front_l.z == rookD[0].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == rookD[1].y && knightCircle_front_l.z == rookD[1].z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == queenD.y && knightCircle_front_l.z == queenD.z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else if (knightCircle_front_l.y == kingD.y && knightCircle_front_l.z == kingD.z) {
        knightCircle_front_l.y = 100;
        knightCircle_front_l.z = 100;
    }
    else {
        if (knightCircle_front_l.y >= -2 && knightCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(knightCircle_front_l.x, knightCircle_front_l.y, knightCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_behind_l.y == pawnD[0].y && knightCircle_behind_l.z == pawnD[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if ((knightCircle_behind_l.y == pawnD[1].y && knightCircle_behind_l.z == pawnD[1].z)) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnD[2].y && knightCircle_behind_l.z == pawnD[2].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnD[3].y && knightCircle_behind_l.z == pawnD[3].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnD[4].y && knightCircle_behind_l.z == pawnD[4].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnD[5].y && knightCircle_behind_l.z == pawnD[5].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnD[6].y && knightCircle_behind_l.z == pawnD[6].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == pawnD[7].y && knightCircle_behind_l.z == pawnD[7].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == knightD[0].y && knightCircle_behind_l.z == knightD[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == knightD[1].y && knightCircle_behind_l.z == knightD[1].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == bishopD[0].y && knightCircle_behind_l.z == bishopD[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == bishopD[1].y && knightCircle_behind_l.z == bishopD[1].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == rookD[0].y && knightCircle_behind_l.z == rookD[0].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == rookD[1].y && knightCircle_behind_l.z == rookD[1].z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == queenD.y && knightCircle_behind_l.z == queenD.z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else if (knightCircle_behind_l.y == kingD.y && knightCircle_behind_l.z == kingD.z) {
        knightCircle_behind_l.y = 100;
        knightCircle_behind_l.z = 100;
    }
    else {
        if (knightCircle_behind_l.y >= -2 && knightCircle_behind_l.z >= -1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_behind_l.x, knightCircle_behind_l.y, knightCircle_behind_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_left_u.y == pawnD[0].y && knightCircle_left_u.z == pawnD[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if ((knightCircle_left_u.y == pawnD[1].y && knightCircle_left_u.z == pawnD[1].z)) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnD[2].y && knightCircle_left_u.z == pawnD[2].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnD[3].y && knightCircle_left_u.z == pawnD[3].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnD[4].y && knightCircle_left_u.z == pawnD[4].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnD[5].y && knightCircle_left_u.z == pawnD[5].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnD[6].y && knightCircle_left_u.z == pawnD[6].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == pawnD[7].y && knightCircle_left_u.z == pawnD[7].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == knightD[0].y && knightCircle_left_u.z == knightD[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == knightD[1].y && knightCircle_left_u.z == knightD[1].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == bishopD[0].y && knightCircle_left_u.z == bishopD[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == bishopD[1].y && knightCircle_left_u.z == bishopD[1].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == rookD[0].y && knightCircle_left_u.z == rookD[0].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == rookD[1].y && knightCircle_left_u.z == rookD[1].z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == queenD.y && knightCircle_left_u.z == queenD.z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else if (knightCircle_left_u.y == kingD.y && knightCircle_left_u.z == kingD.z) {
        knightCircle_left_u.y = 100;
        knightCircle_left_u.z = 100;
    }
    else {
        if (knightCircle_left_u.z <= 2 && knightCircle_left_u.y >= -2) {
            glPushMatrix();
            glTranslatef(knightCircle_left_u.x, knightCircle_left_u.y, knightCircle_left_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_right_u.y == pawnD[0].y && knightCircle_right_u.z == pawnD[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if ((knightCircle_right_u.y == pawnD[1].y && knightCircle_right_u.z == pawnD[1].z)) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnD[2].y && knightCircle_right_u.z == pawnD[2].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnD[3].y && knightCircle_right_u.z == pawnD[3].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnD[4].y && knightCircle_right_u.z == pawnD[4].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnD[5].y && knightCircle_right_u.z == pawnD[5].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnD[6].y && knightCircle_right_u.z == pawnD[6].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == pawnD[7].y && knightCircle_right_u.z == pawnD[7].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == knightD[0].y && knightCircle_right_u.z == knightD[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == knightD[1].y && knightCircle_right_u.z == knightD[1].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == bishopD[0].y && knightCircle_right_u.z == bishopD[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == bishopD[1].y && knightCircle_right_u.z == bishopD[1].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == rookD[0].y && knightCircle_right_u.z == rookD[0].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == rookD[1].y && knightCircle_right_u.z == rookD[1].z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == queenD.y && knightCircle_right_u.z == queenD.z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else if (knightCircle_right_u.y == kingD.y && knightCircle_right_u.z == kingD.z) {
        knightCircle_right_u.y = 100;
        knightCircle_right_u.z = 100;
    }
    else {
        if (knightCircle_right_u.z <= 2 && knightCircle_right_u.y <= 1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_right_u.x, knightCircle_right_u.y, knightCircle_right_u.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_left_d.y == pawnD[0].y && knightCircle_left_d.z == pawnD[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if ((knightCircle_left_d.y == pawnD[1].y && knightCircle_left_d.z == pawnD[1].z)) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnD[2].y && knightCircle_left_d.z == pawnD[2].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnD[3].y && knightCircle_left_d.z == pawnD[3].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnD[4].y && knightCircle_left_d.z == pawnD[4].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnD[5].y && knightCircle_left_d.z == pawnD[5].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnD[6].y && knightCircle_left_d.z == pawnD[6].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == pawnD[7].y && knightCircle_left_d.z == pawnD[7].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == knightD[0].y && knightCircle_left_d.z == knightD[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == knightD[1].y && knightCircle_left_d.z == knightD[1].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == bishopD[0].y && knightCircle_left_d.z == bishopD[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == bishopD[1].y && knightCircle_left_d.z == bishopD[1].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == rookD[0].y && knightCircle_left_d.z == rookD[0].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == rookD[1].y && knightCircle_left_d.z == rookD[1].z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == queenD.y && knightCircle_left_d.z == queenD.z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else if (knightCircle_left_d.y == kingD.y && knightCircle_left_d.z == kingD.z) {
        knightCircle_left_d.y = 100;
        knightCircle_left_d.z = 100;
    }
    else {
        if (knightCircle_left_d.z >= -1.5 && knightCircle_left_d.y >= -2) {
            glPushMatrix();
            glTranslatef(knightCircle_left_d.x, knightCircle_left_d.y, knightCircle_left_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }

    if (knightCircle_right_d.y == pawnD[0].y && knightCircle_right_d.z == pawnD[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if ((knightCircle_right_d.y == pawnD[1].y && knightCircle_right_d.z == pawnD[1].z)) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnD[2].y && knightCircle_right_d.z == pawnD[2].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnD[3].y && knightCircle_right_d.z == pawnD[3].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnD[4].y && knightCircle_right_d.z == pawnD[4].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnD[5].y && knightCircle_right_d.z == pawnD[5].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnD[6].y && knightCircle_right_d.z == pawnD[6].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == pawnD[7].y && knightCircle_right_d.z == pawnD[7].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == knightD[0].y && knightCircle_right_d.z == knightD[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == knightD[1].y && knightCircle_right_d.z == knightD[1].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == bishopD[0].y && knightCircle_right_d.z == bishopD[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == bishopD[1].y && knightCircle_right_d.z == bishopD[1].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == rookD[0].y && knightCircle_right_d.z == rookD[0].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == rookD[1].y && knightCircle_right_d.z == rookD[1].z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == queenD.y && knightCircle_right_d.z == queenD.z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else if (knightCircle_right_d.y == kingD.y && knightCircle_right_d.z == kingD.z) {
        knightCircle_right_d.y = 100;
        knightCircle_right_d.z = 100;
    }
    else {
        if (knightCircle_right_d.z >= -1.5 && knightCircle_right_d.y <= 1.5) {
            glPushMatrix();
            glTranslatef(knightCircle_right_d.x, knightCircle_right_d.y, knightCircle_right_d.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
}
void move_Pawn_Circle_B(int x) {
    pawnchk = x;
    float a = 0.5;
    float b = 1.0;
    pawnCircle_front[0].x = a;
    pawnCircle_front[0].y = pawnD[x].y;
    pawnCircle_front[0].z = pawnD[x].z - a;

    pawnCircle_front[1].x = a;
    pawnCircle_front[1].y = pawnD[x].y;
    pawnCircle_front[1].z = pawnD[x].z - b;

    pawnCircle_front_l.x = a;
    pawnCircle_front_l.y = pawnD[x].y - a;
    pawnCircle_front_l.z = pawnD[x].z - a;

    pawnCircle_front_r.x = a;
    pawnCircle_front_r.y = pawnD[x].y + a;
    pawnCircle_front_r.z = pawnD[x].z - a;

    if (pawnCircle_front[0].y == pawnD[0].y && pawnCircle_front[0].z == pawnD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if ((pawnCircle_front[0].y == pawnD[1].y && pawnCircle_front[0].z == pawnD[1].z)) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[2].y && pawnCircle_front[0].z == pawnD[2].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[3].y && pawnCircle_front[0].z == pawnD[3].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[4].y && pawnCircle_front[0].z == pawnD[4].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[5].y && pawnCircle_front[0].z == pawnD[5].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[6].y && pawnCircle_front[0].z == pawnD[6].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnD[7].y && pawnCircle_front[0].z == pawnD[7].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightD[0].y && pawnCircle_front[0].z == knightD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightD[1].y && pawnCircle_front[0].z == knightD[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopD[0].y && pawnCircle_front[0].z == bishopD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopD[1].y && pawnCircle_front[0].z == bishopD[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookD[0].y && pawnCircle_front[0].z == rookD[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookD[1].y && pawnCircle_front[0].z == rookD[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == queenD.y && pawnCircle_front[0].z == queenD.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == kingD.y && pawnCircle_front[0].z == kingD.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[0].y && pawnCircle_front[0].z == pawnC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if ((pawnCircle_front[0].y == pawnC[1].y && pawnCircle_front[0].z == pawnC[1].z)) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[2].y && pawnCircle_front[0].z == pawnC[2].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[3].y && pawnCircle_front[0].z == pawnC[3].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[4].y && pawnCircle_front[0].z == pawnC[4].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[5].y && pawnCircle_front[0].z == pawnC[5].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[6].y && pawnCircle_front[0].z == pawnC[6].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == pawnC[7].y && pawnCircle_front[0].z == pawnC[7].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightC[0].y && pawnCircle_front[0].z == knightC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == knightC[1].y && pawnCircle_front[0].z == knightC[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopC[0].y && pawnCircle_front[0].z == bishopC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == bishopC[1].y && pawnCircle_front[0].z == bishopC[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookC[0].y && pawnCircle_front[0].z == rookC[0].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == rookC[1].y && pawnCircle_front[0].z == rookC[1].z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == queenC.y && pawnCircle_front[0].z == queenC.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else if (pawnCircle_front[0].y == kingC.y && pawnCircle_front[0].z == kingC.z) {
        pawnCircle_front[0].y = 100;
        pawnCircle_front[0].z = 100;
    }
    else {
        if (pawnD[x].z + a <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front[0].x, pawnCircle_front[0].y, pawnCircle_front[0].z);
            draw_move_Circle();
            glPopMatrix();
            if (pawn_moved_B[x] == 0) {
                if (pawnCircle_front[1].y == pawnD[0].y && pawnCircle_front[1].z == pawnD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if ((pawnCircle_front[1].y == pawnD[1].y && pawnCircle_front[1].z == pawnD[1].z)) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[2].y && pawnCircle_front[1].z == pawnD[2].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[3].y && pawnCircle_front[1].z == pawnD[3].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[4].y && pawnCircle_front[1].z == pawnD[4].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[5].y && pawnCircle_front[1].z == pawnD[5].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[6].y && pawnCircle_front[1].z == pawnD[6].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnD[7].y && pawnCircle_front[1].z == pawnD[7].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightD[0].y && pawnCircle_front[1].z == knightD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightD[1].y && pawnCircle_front[1].z == knightD[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopD[0].y && pawnCircle_front[1].z == bishopD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopD[1].y && pawnCircle_front[1].z == bishopD[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookD[0].y && pawnCircle_front[1].z == rookD[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookD[1].y && pawnCircle_front[1].z == rookD[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == queenD.y && pawnCircle_front[1].z == queenD.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == kingD.y && pawnCircle_front[1].z == kingD.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[0].y && pawnCircle_front[1].z == pawnC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if ((pawnCircle_front[1].y == pawnC[1].y && pawnCircle_front[1].z == pawnC[1].z)) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[2].y && pawnCircle_front[1].z == pawnC[2].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[3].y && pawnCircle_front[1].z == pawnC[3].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[4].y && pawnCircle_front[1].z == pawnC[4].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[5].y && pawnCircle_front[1].z == pawnC[5].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[6].y && pawnCircle_front[1].z == pawnC[6].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == pawnC[7].y && pawnCircle_front[1].z == pawnC[7].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightC[0].y && pawnCircle_front[1].z == knightC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == knightC[1].y && pawnCircle_front[1].z == knightC[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopC[0].y && pawnCircle_front[1].z == bishopC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == bishopC[1].y && pawnCircle_front[1].z == bishopC[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookC[0].y && pawnCircle_front[1].z == rookC[0].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == rookC[1].y && pawnCircle_front[1].z == rookC[1].z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == queenC.y && pawnCircle_front[1].z == queenC.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else if (pawnCircle_front[1].y == kingC.y && pawnCircle_front[1].z == kingC.z) {
                    pawnCircle_front[1].y = 100;
                    pawnCircle_front[1].z = 100;
                }
                else {
                    glPushMatrix();
                    glTranslatef(pawnCircle_front[1].x, pawnCircle_front[1].y, pawnCircle_front[1].z);
                    draw_move_Circle();
                    glPopMatrix();
                }
            }
        }
    }



    if (pawnCircle_front_r.y == pawnC[0].y && pawnCircle_front_r.z == pawnC[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if ((pawnCircle_front_r.y == pawnC[1].y && pawnCircle_front_r.z == pawnC[1].z)) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnC[2].y && pawnCircle_front_r.z == pawnC[2].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnC[3].y && pawnCircle_front_r.z == pawnC[3].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnC[4].y && pawnCircle_front_r.z == pawnC[4].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnC[5].y && pawnCircle_front_r.z == pawnC[5].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnC[6].y && pawnCircle_front_r.z == pawnC[6].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == pawnC[7].y && pawnCircle_front_r.z == pawnC[7].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == knightC[0].y && pawnCircle_front_r.z == knightC[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == knightC[1].y && pawnCircle_front_r.z == knightC[1].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == bishopC[0].y && pawnCircle_front_r.z == bishopC[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == bishopC[1].y && pawnCircle_front_r.z == bishopC[1].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == rookC[0].y && pawnCircle_front_r.z == rookC[0].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == rookC[1].y && pawnCircle_front_r.z == rookC[1].z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == queenC.y && pawnCircle_front_r.z == queenC.z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_r.y == kingC.y && pawnCircle_front_r.z == kingC.z) {
        if (pawnCircle_front_r.y <= 1.5 && pawnCircle_front_r.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_r.x, pawnCircle_front_r.y, pawnCircle_front_r.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else {
        pawnCircle_front_r.y = 100;
        pawnCircle_front_r.z = 100;
    }

    if (pawnCircle_front_l.y == pawnC[0].y && pawnCircle_front_l.z == pawnC[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if ((pawnCircle_front_l.y == pawnC[1].y && pawnCircle_front_l.z == pawnC[1].z)) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnC[2].y && pawnCircle_front_l.z == pawnC[2].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnC[3].y && pawnCircle_front_l.z == pawnC[3].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnC[4].y && pawnCircle_front_l.z == pawnC[4].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnC[5].y && pawnCircle_front_l.z == pawnC[5].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnC[6].y && pawnCircle_front_l.z == pawnC[6].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == pawnC[7].y && pawnCircle_front_l.z == pawnC[7].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == knightC[0].y && pawnCircle_front_l.z == knightC[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == knightC[1].y && pawnCircle_front_l.z == knightC[1].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == bishopC[0].y && pawnCircle_front_l.z == bishopC[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == bishopC[1].y && pawnCircle_front_l.z == bishopC[1].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == rookC[0].y && pawnCircle_front_l.z == rookC[0].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == rookC[1].y && pawnCircle_front_l.z == rookC[1].z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == queenC.y && pawnCircle_front_l.z == queenC.z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else if (pawnCircle_front_l.y == kingC.y && pawnCircle_front_l.z == kingC.z) {
        if (pawnCircle_front_l.y >= -2 && pawnCircle_front_l.z <= 2) {
            glPushMatrix();
            glTranslatef(pawnCircle_front_l.x, pawnCircle_front_l.y, pawnCircle_front_l.z);
            draw_move_Circle();
            glPopMatrix();
        }
    }
    else {
        pawnCircle_front_l.y = 100;
        pawnCircle_front_l.z = 100;
    }
}

void torus() {
    currentMaterials = &yellowPlasticMaterials;
    light();
    glutSolidTorus(0.08, 0.18, 50, 50);
}
void cone_move() {
    if (turn == 0) {
        glPushMatrix();
        glTranslatef(coneC.x, coneC.y, coneC.z);
        glRotatef(90.0, 0.0, -90.0, 0.0);
        cone();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.2, coneC.y, coneC.z);
        glRotatef(90.0, 0.0, -90.0, 0.0);
        torus();
        glPopMatrix();
    }

    else {
        glPushMatrix();
        glTranslatef(coneD.x, coneD.y, coneD.z);
        glRotatef(90.0, 0.0, -90.0, 0.0);
        cone();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.2, coneD.y, coneD.z);
        glRotatef(90.0, 0.0, -90.0, 0.0);
        torus();
        glPopMatrix();
    }


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

    /* ▲백                                       ▼ 흑 */

    glPushMatrix();
    glTranslatef(pawnD[0].x, pawnD[0].y, pawnD[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnD[1].x, pawnD[1].y, pawnD[1].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnD[2].x, pawnD[2].y, pawnD[2].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnD[3].x, pawnD[3].y, pawnD[3].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnD[4].x, pawnD[4].y, pawnD[4].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnD[5].x, pawnD[5].y, pawnD[5].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnD[6].x, pawnD[6].y, pawnD[6].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_pawnID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(pawnD[7].x, pawnD[7].y, pawnD[7].z);
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
    /* ▲백                                       ▼흑*/
    glPushMatrix();
    glTranslatef(kingD.x, kingD.y, kingD.z);
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
    /* ▲백                                       ▼흑*/
    glPushMatrix();
    glTranslatef(queenD.x, queenD.y, queenD.z);
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
    /* ▲백                                       ▼흑*/
    glPushMatrix();
    glTranslatef(rookD[0].x, rookD[0].y, rookD[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_rookID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rookD[1].x, rookD[1].y, rookD[1].z);
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
    /* ▲백                                       ▼흑*/
    glPushMatrix();
    glTranslatef(bishopD[0].x, bishopD[0].y, bishopD[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_bishopID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bishopD[1].x, bishopD[1].y, bishopD[1].z);
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
    /* ▲백                                       ▼흑*/
    glPushMatrix();
    glTranslatef(knightD[0].x, knightD[0].y, knightD[0].z);
    glRotatef(90.0, 0.0, 0.0, -90.0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(g_KnightID);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(knightD[1].x, knightD[1].y, knightD[1].z);
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

    gluLookAt(x, y, z, -0.5, 0.0, 0.5, 0.0, 0.0, 1.0);

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
    king_move();
    pawn_move();
    rook_move();
    bishop_move();
    knight_move();
    queen_move();

    if (temp >= 1 && temp <= 16) {
        if (temp == 1) {
            move_King_Circle_W();
        }
        else if (temp == 2) {
            move_Queen_Circle_W();
        }
        else if (temp == 3) {
            move_Bishop_Circle_W(0);
        }
        else if (temp == 4) {
            move_Bishop_Circle_W(1);
        }
        else if (temp == 5) {
            move_Rook_Circle_W(0);
        }
        else if (temp == 6) {
            move_Rook_Circle_W(1);
        }
        else if (temp == 7) {
            move_Knight_Circle_W(0);
        }
        else if (temp == 8) {
            move_Knight_Circle_W(1);
        }
        else if (temp >= 9 && temp <= 16) {
            move_Pawn_Circle_W(temp % 9);
        }
    }

    else if (temp >= 17 && temp <= 32) {
        if (temp == 17) {
            move_King_Circle_B();
        }
        else if (temp == 18) {
            move_Queen_Circle_B();
        }
        else if (temp == 19) {
            move_Bishop_Circle_B(0);
        }
        else if (temp == 20) {
            move_Bishop_Circle_B(1);
        }
        else if (temp == 21) {
            move_Rook_Circle_B(0);
        }
        else if (temp == 22) {
            move_Rook_Circle_B(1);
        }
        else if (temp == 23) {
            move_Knight_Circle_B(0);
        }
        else if (temp == 24) {
            move_Knight_Circle_B(1);
        }
        else if (temp >= 25 && temp <= 32) {
            move_Pawn_Circle_B(temp % 25);
        }
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
        if (moving() == 1) {
            kingC.x = 0.5;
            temp = 1;
        }
        else if (moving() == 2) {
            queenC.x = 0.5;
            temp = 2;
        }
        else if (moving() == 3) {
            bishopC[0].x = 0.5;
            temp = 3;
        }
        else if (moving() == 4) {
            bishopC[1].x = 0.5;
            temp = 4;
        }
        else if (moving() == 5) {
            rookC[0].x = 0.5;
            temp = 5;
        }
        else if (moving() == 6) {
            rookC[1].x = 0.5;
            temp = 6;
        }
        else if (moving() == 7) {
            knightC[0].x = 0.5;
            temp = 7;
        }
        else if (moving() == 8) {
            knightC[1].x = 0.5;
            temp = 8;
        }
        else if (moving() == 9) {
            pawnC[0].x = 0.5;
            temp = 9;
        }
        else if (moving() == 10) {
            pawnC[1].x = 0.5;
            temp = 10;
        }
        else if (moving() == 11) {
            pawnC[2].x = 0.5;
            temp = 11;
        }
        else if (moving() == 12) {
            pawnC[3].x = 0.5;
            temp = 12;
        }
        else if (moving() == 13) {
            pawnC[4].x = 0.5;
            temp = 13;
        }
        else if (moving() == 14) {
            pawnC[5].x = 0.5;
            temp = 14;
        }
        else if (moving() == 15) {
            pawnC[6].x = 0.5;
            temp = 15;
        }
        else if (moving() == 16) {
            pawnC[7].x = 0.5;
            temp = 16;
        }

        else if (moving() == 17) {
            kingD.x = 0.5;
            temp = 17;
        }
        else if (moving() == 18) {
            queenD.x = 0.5;
            temp = 18;
        }
        else if (moving() == 19) {
            bishopD[0].x = 0.5;
            temp = 19;
        }
        else if (moving() == 20) {
            bishopD[1].x = 0.5;
            temp = 20;
        }
        else if (moving() == 21) {
            rookD[0].x = 0.5;
            temp = 21;
        }
        else if (moving() == 22) {
            rookD[1].x = 0.5;
            temp = 22;
        }
        else if (moving() == 23) {
            knightD[0].x = 0.5;
            temp = 23;
        }
        else if (moving() == 24) {
            knightD[1].x = 0.5;
            temp = 24;
        }
        else if (moving() == 25) {
            pawnD[0].x = 0.5;
            temp = 25;
        }
        else if (moving() == 26) {
            pawnD[1].x = 0.5;
            temp = 26;
        }
        else if (moving() == 27) {
            pawnD[2].x = 0.5;
            temp = 27;
        }
        else if (moving() == 28) {
            pawnD[3].x = 0.5;
            temp = 28;
        }
        else if (moving() == 29) {
            pawnD[4].x = 0.5;
            temp = 29;
        }
        else if (moving() == 30) {
            pawnD[5].x = 0.5;
            temp = 30;
        }
        else if (moving() == 31) {
            pawnD[6].x = 0.5;
            temp = 31;
        }
        else if (moving() == 32) {
            pawnD[7].x = 0.5;
            temp = 32;
        }
        break;

    case GLUT_KEY_F2:
        if (temp == 1) {
            chessman_move(temp);
            kingC.x = 0.25;
            kingC.y = coneC.y;
            kingC.z = coneC.z;
            temp = 0;
        }
        else if (temp == 2) {
            chessman_move(temp);
            queenC.x = 0.25;
            queenC.y = coneC.y;
            queenC.z = coneC.z;
            temp = 0;
        }
        else if (temp == 3) {
            chessman_move(temp);
            bishopC[0].x = 0.25;
            bishopC[0].y = coneC.y;
            bishopC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 4) {
            chessman_move(temp);
            bishopC[1].x = 0.25;
            bishopC[1].y = coneC.y;
            bishopC[1].z = coneC.y;
            temp = 0;
        }
        else if (temp == 5) {
            chessman_move(temp);
            rookC[0].x = 0.25;
            rookC[0].y = coneC.y;
            rookC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 6) {
            chessman_move(temp);
            rookC[1].x = 0.25;
            rookC[1].y = coneC.y;
            rookC[1].z = coneC.z;
            temp = 0;
        }
        else if (temp == 7) {
            chessman_move(temp);
            knightC[0].x = 0.25;
            knightC[0].y = coneC.y;
            knightC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 8) {
            chessman_move(temp);
            knightC[1].x = 0.25;
            knightC[1].y = coneC.y;
            knightC[1].z = coneC.z;
            temp = 0;
        }
        else if (temp >= 9 && temp <= 16) {
            chessman_move(temp);
            pawnC[temp % 9].x = 0.25;
            pawnC[temp % 9].y = coneC.y;
            pawnC[temp % 9].z = coneC.z;
            temp = 0;
        }

        else if (temp == 17) {
            chessman_move(temp);
            kingD.x = 0.25;
            kingD.y = coneD.y;
            kingD.z = coneD.z;
            temp = 0;
        }
        else if (temp == 18) {
            chessman_move(temp);
            queenD.x = 0.25;
            queenD.y = coneD.y;
            queenD.z = coneD.z;
            temp = 0;
        }
        else if (temp == 19) {
            chessman_move(temp);
            bishopD[0].x = 0.25;
            bishopD[0].y = coneD.y;
            bishopD[0].z = coneD.z;
            temp = 0;
        }
        else if (temp == 20) {
            chessman_move(temp);
            bishopD[1].x = 0.25;
            bishopD[1].y = coneD.y;
            bishopD[1].z = coneD.y;
            temp = 0;
        }
        else if (temp == 21) {
            chessman_move(temp);
            rookD[0].x = 0.25;
            rookD[0].y = coneD.y;
            rookD[0].z = coneD.z;
            temp = 0;
        }
        else if (temp == 22) {
            chessman_move(temp);
            rookD[1].x = 0.25;
            rookD[1].y = coneD.y;
            rookD[1].z = coneD.z;
            temp = 0;
        }
        else if (temp == 23) {
            chessman_move(temp);
            knightD[0].x = 0.25;
            knightD[0].y = coneD.y;
            knightD[0].z = coneD.z;
            temp = 0;
        }
        else if (temp == 24) {
            chessman_move(temp);
            knightD[1].x = 0.25;
            knightD[1].y = coneD.y;
            knightD[1].z = coneD.z;
            temp = 0;
        }
        else if (temp >= 25 && temp <= 32) {
            chessman_move(temp);
            pawnD[temp % 25].x = 0.25;
            pawnD[temp % 25].y = coneD.y;
            pawnD[temp % 25].z = coneD.z;
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
    case 'w': {
        if (turn == 0) {
            if (coneC.z < 2) {
                coneC.z += 0.5;
            }
        }
        else {
            if (coneD.z < 2) {
                coneD.z += 0.5;
            }
        }
        break;
    }
    case 's': {
        if (turn == 0) {
            if (coneC.z > -1.5) {
                coneC.z -= 0.5;
            }
        }
        else {
            if (coneD.z > -1.5) {
                coneD.z -= 0.5;
            }
        }
        break;
    }
    case 'a': {
        if (turn == 0) {
            if (coneC.y > -2) {
                coneC.y -= 0.5;
            }
            else {
                coneD.y -= 0.5;
            }
        }
        else {
            if (coneD.y > -2) {
                coneD.y -= 0.5;
            }
        }
        break;
    }
    case 'd': {
        if (turn == 0) {
            if (coneC.y < 1.5) {
                coneC.y += 0.5;
            }
        }
        else {
            if (coneD.y < 1.5) {
                coneD.y += 0.5;
            }
        }
        break;
    }
    case 32: {
        if (moving() == 1) {
            kingC.x = 0.5;
            temp = 1;
        }
        else if (moving() == 2) {
            queenC.x = 0.5;
            temp = 2;
        }
        else if (moving() == 3) {
            bishopC[0].x = 0.5;
            temp = 3;
        }
        else if (moving() == 4) {
            bishopC[1].x = 0.5;
            temp = 4;
        }
        else if (moving() == 5) {
            rookC[0].x = 0.5;
            temp = 5;
        }
        else if (moving() == 6) {
            rookC[1].x = 0.5;
            temp = 6;
        }
        else if (moving() == 7) {
            knightC[0].x = 0.5;
            temp = 7;
        }
        else if (moving() == 8) {
            knightC[1].x = 0.5;
            temp = 8;
        }
        else if (moving() == 9) {
            pawnC[0].x = 0.5;
            temp = 9;
        }
        else if (moving() == 10) {
            pawnC[1].x = 0.5;
            temp = 10;
        }
        else if (moving() == 11) {
            pawnC[2].x = 0.5;
            temp = 11;
        }
        else if (moving() == 12) {
            pawnC[3].x = 0.5;
            temp = 12;
        }
        else if (moving() == 13) {
            pawnC[4].x = 0.5;
            temp = 13;
        }
        else if (moving() == 14) {
            pawnC[5].x = 0.5;
            temp = 14;
        }
        else if (moving() == 15) {
            pawnC[6].x = 0.5;
            temp = 15;
        }
        else if (moving() == 16) {
            pawnC[7].x = 0.5;
            temp = 16;
        }

        else if (moving() == 17) {
            kingD.x = 0.5;
            temp = 17;
        }
        else if (moving() == 18) {
            queenD.x = 0.5;
            temp = 18;
        }
        else if (moving() == 19) {
            bishopD[0].x = 0.5;
            temp = 19;
        }
        else if (moving() == 20) {
            bishopD[1].x = 0.5;
            temp = 20;
        }
        else if (moving() == 21) {
            rookD[0].x = 0.5;
            temp = 21;
        }
        else if (moving() == 22) {
            rookD[1].x = 0.5;
            temp = 22;
        }
        else if (moving() == 23) {
            knightD[0].x = 0.5;
            temp = 23;
        }
        else if (moving() == 24) {
            knightD[1].x = 0.5;
            temp = 24;
        }
        else if (moving() == 25) {
            pawnD[0].x = 0.5;
            temp = 25;
        }
        else if (moving() == 26) {
            pawnD[1].x = 0.5;
            temp = 26;
        }
        else if (moving() == 27) {
            pawnD[2].x = 0.5;
            temp = 27;
        }
        else if (moving() == 28) {
            pawnD[3].x = 0.5;
            temp = 28;
        }
        else if (moving() == 29) {
            pawnD[4].x = 0.5;
            temp = 29;
        }
        else if (moving() == 30) {
            pawnD[5].x = 0.5;
            temp = 30;
        }
        else if (moving() == 31) {
            pawnD[6].x = 0.5;
            temp = 31;
        }
        else if (moving() == 32) {
            pawnD[7].x = 0.5;
            temp = 32;
        }
        break;
    }
    case 13: {
        if (temp == 1) {
            chessman_move(temp);
            kingC.x = 0.25;
            kingC.y = coneC.y;
            kingC.z = coneC.z;
            temp = 0;
        }
        else if (temp == 2) {
            chessman_move(temp);
            queenC.x = 0.25;
            queenC.y = coneC.y;
            queenC.z = coneC.z;
            temp = 0;
        }
        else if (temp == 3) {
            chessman_move(temp);
            bishopC[0].x = 0.25;
            bishopC[0].y = coneC.y;
            bishopC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 4) {
            chessman_move(temp);
            bishopC[1].x = 0.25;
            bishopC[1].y = coneC.y;
            bishopC[1].z = coneC.y;
            temp = 0;
        }
        else if (temp == 5) {
            chessman_move(temp);
            rookC[0].x = 0.25;
            rookC[0].y = coneC.y;
            rookC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 6) {
            chessman_move(temp);
            rookC[1].x = 0.25;
            rookC[1].y = coneC.y;
            rookC[1].z = coneC.z;
            temp = 0;
        }
        else if (temp == 7) {
            chessman_move(temp);
            knightC[0].x = 0.25;
            knightC[0].y = coneC.y;
            knightC[0].z = coneC.z;
            temp = 0;
        }
        else if (temp == 8) {
            chessman_move(temp);
            knightC[1].x = 0.25;
            knightC[1].y = coneC.y;
            knightC[1].z = coneC.z;
            temp = 0;
        }
        else if (temp >= 9 && temp <= 16) {
            chessman_move(temp);
            pawnC[temp % 9].x = 0.25;
            pawnC[temp % 9].y = coneC.y;
            pawnC[temp % 9].z = coneC.z;
            temp = 0;
        }

        else if (temp == 17) {
            chessman_move(temp);
            kingD.x = 0.25;
            kingD.y = coneD.y;
            kingD.z = coneD.z;
            temp = 0;
        }
        else if (temp == 18) {
            chessman_move(temp);
            queenD.x = 0.25;
            queenD.y = coneD.y;
            queenD.z = coneD.z;
            temp = 0;
        }
        else if (temp == 19) {
            chessman_move(temp);
            bishopD[0].x = 0.25;
            bishopD[0].y = coneD.y;
            bishopD[0].z = coneD.z;
            temp = 0;
        }
        else if (temp == 20) {
            chessman_move(temp);
            bishopD[1].x = 0.25;
            bishopD[1].y = coneD.y;
            bishopD[1].z = coneD.y;
            temp = 0;
        }
        else if (temp == 21) {
            chessman_move(temp);
            rookD[0].x = 0.25;
            rookD[0].y = coneD.y;
            rookD[0].z = coneD.z;
            temp = 0;
        }
        else if (temp == 22) {
            chessman_move(temp);
            rookD[1].x = 0.25;
            rookD[1].y = coneD.y;
            rookD[1].z = coneD.z;
            temp = 0;
        }
        else if (temp == 23) {
            chessman_move(temp);
            knightD[0].x = 0.25;
            knightD[0].y = coneD.y;
            knightD[0].z = coneD.z;
            temp = 0;
        }
        else if (temp == 24) {
            chessman_move(temp);
            knightD[1].x = 0.25;
            knightD[1].y = coneD.y;
            knightD[1].z = coneD.z;
            temp = 0;
        }
        else if (temp >= 25 && temp <= 32) {
            chessman_move(temp);
            pawnD[temp % 25].x = 0.25;
            pawnD[temp % 25].y = coneD.y;
            pawnD[temp % 25].z = coneD.z;
            temp = 0;
        }

        break;
    }
    case 27: {
        exit(1);
        break;
    }

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
    torus();
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
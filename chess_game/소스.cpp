#include <windows.h>       
#include <iostream>
#include <string>
#include <math.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <vector>
#include <fstream>

using namespace std;

#define	PI	3.1415926

float	theta, phi;

float	camera_radius;

float	moveX, moveY;

float	moveW = 0.0;

//********************* Material Data ****************//
typedef struct materialStruct {
	GLfloat	ambient[4];
	GLfloat	diffuse[4];
	GLfloat	specular[4];
	GLfloat	shininess;
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
	GLfloat	ambient[4];
	GLfloat	diffuse[4];
	GLfloat	specular[4];
	GLfloat	position[4];
	GLfloat cutoff;
	GLfloat	exponent;
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

CModel m;

void draw_chessboard_b() {
	currentMaterials = &blackPlasticMaterials;
	glMaterialfv(GL_FRONT, GL_AMBIENT, currentMaterials->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, currentMaterials->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, currentMaterials->specular);
	glMaterialf(GL_FRONT, GL_SHININESS, currentMaterials->shininess);
	glutSolidCube(0.5);
}
void draw_chessboard_w() {
	currentMaterials = &whitePlasticMaterials;
	glMaterialfv(GL_FRONT, GL_AMBIENT, currentMaterials->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, currentMaterials->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, currentMaterials->specular);
	glMaterialf(GL_FRONT, GL_SHININESS, currentMaterials->shininess);
	glutSolidCube(0.5);
}


void init(void)
{
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

void changeSize(int w, int h)
{
	//창이 아주 작을 때, 0 으로 나누는 것을 예방
	if (h == 0)
		h = 1;
	float ratio = 1.0 * w / h;

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

void chessman() {
	GLfloat cx, cy, cz, nx, ny, nz;
	int v_id, vt_id, vn_id;
	int nFaces = m.objs[0].f.size();

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

void display(void)
{
	float	x, y, z;

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
			glTranslatef(0.0, j - 1.0, i - 1.0);
			draw_chessboard_b();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0, j - 0.5, i - 0.5);
			draw_chessboard_b();
			glPopMatrix();
		}

	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			glPushMatrix();
			glTranslatef(0.0, j - 0.5, i - 1.0);
			draw_chessboard_w();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0, j - 1.0, i - 0.5);
			draw_chessboard_w();
			glPopMatrix();
		}
	}

	glPushMatrix();
	glTranslatef(moveW, 0.0, 0.0);
	glRotatef(90.0, 0.0, 0.0, -90.0);
	glScalef(0.04, 0.04, 0.04);
	chessman();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:	moveX -= 0.01;
		break;
	case GLUT_KEY_RIGHT:	moveX += 0.01;
		break;
	case GLUT_KEY_UP:		moveY += 0.01;
		break;
	case GLUT_KEY_DOWN:	moveY -= 0.01;
		break;
	default:				break;
	}
	if (moveX > 2.0 * PI)
		moveX -= (2.0 * PI);
	else if (theta < 0.0)
		moveX += (2.0 * PI);
	glutPostRedisplay();

}

void MyKey(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':	moveW += 1;
		break;

	default: break;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	string filepath = "./chess/King.obj";
	ifstream fin(filepath);

	m.loadObj(fin);

	fin.close();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("");
	init();
	glutDisplayFunc(display);
	glutSpecialFunc(SpecialKey);
	glutReshapeFunc(changeSize);
	glutIdleFunc(display);
	glutMainLoop();
}


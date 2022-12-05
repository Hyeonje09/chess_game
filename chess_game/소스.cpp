#include <gl/glut.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

float	radius, theta, phi;

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

#define _WINDOW_WIDTH 500
#define _WINDOW_HEIGTH 500

GLfloat camx = 100, camy = 100, camz = 100;
GLfloat cam2x = 20, cam2y = 20, cam2z = 20;
GLfloat cam_upx = 0, cam_upy = 1, cam_upz = 0;

CModel m;

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat ratio = (float)width / height;
	gluPerspective(40, ratio, 0.1, 1000);
}

void axis() {
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0); // xÃà 
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0); // yÃà 
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);

	glColor3f(0.0, 0.0, 1.0); // zÃà 
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);
	glEnd();
 }

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	axis();
	gluLookAt(camx, camy, camz, cam2x, cam2y, cam2z, cam_upx, cam_upy, cam_upz);

	GLfloat x, y, z, nx, ny, nz;
	int v_id, vt_id, vn_id;
	int nFaces = m.objs[0].f.size();

	for (int k = 0; k < nFaces; k++) {
		int nPoints = m.objs[0].f[k].v_pairs.size();
		glBegin(GL_POLYGON);
		for (int i = 0; i < nPoints; i++) {
			v_id = m.objs[0].f[k].v_pairs[i].d[0];
			vt_id = m.objs[0].f[k].v_pairs[i].d[1];
			vn_id = m.objs[0].f[k].v_pairs[i].d[2];
			x = m.objs[0].v[v_id - 1].d[0];
			y = m.objs[0].v[v_id - 1].d[1];
			z = m.objs[0].v[v_id - 1].d[2];

			nx = m.objs[0].vn[vn_id - 1].d[0];
			ny = m.objs[0].vn[vn_id - 1].d[1];
			nz = m.objs[0].vn[vn_id - 1].d[2];

			glNormal3f(nx, ny, nz);
			glVertex3f(x, y, z);
		}
		glEnd();
	}
	glFlush();
}

int main(int argc, char** argv) {
	string filepath = "./chess/King.obj";
	ifstream fin(filepath);

	m.loadObj(fin);
	
	fin.close();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(_WINDOW_WIDTH, _WINDOW_HEIGTH);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0,0.0,0.0,0.0);

	glutCreateWindow("OBJ load");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();
}

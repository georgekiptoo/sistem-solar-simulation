// Solar System Simulation
// Student: George Kiptoo
// Faculty of Electrical Engineering and Computer Science
// Transilvania University of Brasov
// Subject: Computer Graphics
// Date: 2025

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>

#define PI 3.14159265

// window size
int W = 1200;
int H = 700;

// pause/speed controls
bool paused = false;
float spd = 1.0f;

// i used a struct to hold planet info
// learned this from lab 4
struct Planet {
    const char* name;
    float dist;      // distance from sun
    float radius;    // planet size
    float orbitSpd;  // how fast it goes around
    float spinSpd;   // rotation on own axis
    float r, g, b;   // colour
    float angle;     // current position in orbit
    float spin;      // current spin
};

// planet data - i got the relative sizes and speeds from wikipedia
// not 100% accurate but looks good
Planet p[] = {
    // name      dist   radius  orbitSpd spinSpd  R     G     B     angle spin
    { "Mercury", 2.2f,  0.10f,  4.7f,   10.0f,  0.72f,0.72f,0.72f, 0.0f, 0.0f },
    { "Venus",   3.2f,  0.18f,  3.5f,    6.5f,  0.95f,0.75f,0.30f, 0.0f, 0.0f },
    { "Earth",   4.4f,  0.20f,  3.0f,    5.0f,  0.20f,0.50f,0.90f, 0.0f, 0.0f },
    { "Mars",    5.6f,  0.14f,  2.4f,    4.8f,  0.80f,0.30f,0.10f, 0.0f, 0.0f },
    { "Jupiter", 7.5f,  0.50f,  1.3f,    2.4f,  0.82f,0.65f,0.45f, 0.0f, 0.0f },
    { "Saturn",  9.5f,  0.42f,  1.0f,    2.2f,  0.94f,0.84f,0.55f, 0.0f, 0.0f },
    { "Uranus", 11.5f,  0.30f,  0.7f,    1.4f,  0.55f,0.85f,0.95f, 0.0f, 0.0f },
    { "Neptune",13.5f,  0.28f,  0.54f,   1.1f,  0.20f,0.35f,0.85f, 0.0f, 0.0f }
};

// moon variables (only did Earth's moon, TODO: add moons for other planets)
float moonAngle = 0.0f;

// stars array - random positions
// TODO: maybe make them twinkle later
struct Star { float x, y, z; };
Star stars[250];

void initStars() {
    srand(12345); // fixed seed so stars dont move every run
    for (int i = 0; i < 250; i++) {
        // put stars far away in random directions
        float a = ((float)rand()/RAND_MAX) * 2 * PI;
        float b2 = ((float)rand()/RAND_MAX) * PI;
        float r = 18.0f + ((float)rand()/RAND_MAX) * 4.0f;
        stars[i].x = r * sin(b2) * cos(a);
        stars[i].y = r * cos(b2);
        stars[i].z = r * sin(b2) * sin(a);
    }
}

// draws orbit circle for a planet
void drawOrbit(float radius) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float rad = i * PI / 180.0f;
        glVertex3f(radius * cos(rad), 0.0f, radius * sin(rad));
    }
    glEnd();
}

// draws saturn rings - just two circles with triangle strip
void drawRings(float inner, float outer) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 360; i++) {
        float rad = i * PI / 180.0f;
        glVertex3f(outer * cos(rad), 0.0f, outer * sin(rad));
        glVertex3f(inner * cos(rad), 0.0f, inner * sin(rad));
    }
    glEnd();
}

// puts planet name above it
void drawName(float x, float y, float z, const char* txt) {
    glRasterPos3f(x, y + 0.18f, z);
    for (int i = 0; txt[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, txt[i]);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // camera angle - tried a few values, this one looks best
    gluLookAt(0.0, 12.0, 20.0,
              0.0,  0.0,  0.0,
              0.0,  1.0,  0.0);

    // draw stars first (background)
    glDisable(GL_LIGHTING);
    glColor3f(0.9f, 0.9f, 0.9f);
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 250; i++)
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    glEnd();

    // draw the sun
    glEnable(GL_LIGHTING);
    GLfloat sunGlow[] = { 1.0f, 0.85f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, sunGlow);
    glColor3f(1.0f, 0.85f, 0.0f);
    glutSolidSphere(0.8, 40, 40);

    // turn off glow for everything else
    GLfloat noGlow[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, noGlow);

    // draw orbit rings
    glDisable(GL_LIGHTING);
    glColor3f(0.22f, 0.22f, 0.22f);
    for (int i = 0; i < 8; i++)
        drawOrbit(p[i].dist);

    // draw each planet
    for (int i = 0; i < 8; i++) {
        // calculate x,z position from angle
        float px = p[i].dist * cos(p[i].angle * PI / 180.0f);
        float pz = p[i].dist * sin(p[i].angle * PI / 180.0f);

        glPushMatrix();
            glTranslatef(px, 0.0f, pz);
            glRotatef(p[i].spin, 0.0f, 1.0f, 0.0f);

            // set planet colour
            glEnable(GL_LIGHTING);
            GLfloat col[]  = { p[i].r, p[i].g, p[i].b, 1.0f };
            GLfloat amb[]  = { p[i].r * 0.25f, p[i].g * 0.25f, p[i].b * 0.25f, 1.0f };
            glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
            glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
            glutSolidSphere(p[i].radius, 30, 30);

            // saturn rings (index 5 = saturn)
            if (i == 5) {
                glDisable(GL_LIGHTING);
                glColor4f(0.85f, 0.78f, 0.55f, 0.55f);
                glRotatef(22.0f, 1.0f, 0.0f, 0.0f);
                drawRings(p[i].radius + 0.09f, p[i].radius + 0.38f);
            }

            // earths moon (index 2 = earth)
            if (i == 2) {
                glDisable(GL_LIGHTING);
                // undo earth spin before rotating moon
                glRotatef(-p[i].spin, 0.0f, 1.0f, 0.0f);
                glRotatef(moonAngle, 0.0f, 1.0f, 0.0f);
                glTranslatef(0.45f, 0.0f, 0.0f);
                glEnable(GL_LIGHTING);
                GLfloat moonCol[] = { 0.75f, 0.75f, 0.75f, 1.0f };
                glMaterialfv(GL_FRONT, GL_DIFFUSE, moonCol);
                glutSolidSphere(0.06f, 15, 15);
            }

        glPopMatrix();

        // label
        glDisable(GL_LIGHTING);
        glColor3f(0.80f, 0.80f, 0.80f);
        drawName(px, p[i].radius, pz, p[i].name);
    }

    // show controls at bottom of screen
    // switching to 2D for this
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, W, 0, H);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glDisable(GL_LIGHTING);
            glColor3f(0.55f, 0.55f, 0.55f);
            glRasterPos2i(10, 12);
            const char* msg = paused
                ? "PAUSED | P: Resume | +/-: Speed | Q: Quit"
                : "P: Pause | +: Speed Up | -: Slow Down | Q: Quit";
            for (int i = 0; msg[i]; i++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, msg[i]);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

// update called every 16ms (approx 60fps)
void update(int val) {
    if (!paused) {
        for (int i = 0; i < 8; i++) {
            p[i].angle += p[i].orbitSpd * spd * 0.3f;
            if (p[i].angle >= 360.0f) p[i].angle -= 360.0f;

            p[i].spin += p[i].spinSpd * spd;
            if (p[i].spin >= 360.0f) p[i].spin -= 360.0f;
        }
        moonAngle += 13.0f * spd * 0.3f;
        if (moonAngle >= 360.0f) moonAngle -= 360.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'p' || key == 'P') paused = !paused;
    if (key == '+' || key == '=') { spd += 0.2f; if (spd > 5.0f) spd = 5.0f; }
    if (key == '-' || key == '_') { spd -= 0.2f; if (spd < 0.1f) spd = 0.1f; }
    if (key == 'q' || key == 'Q' || key == 27) exit(0);
}

void reshape(int w, int h) {
    W = w; H = h;
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, (float)w/h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.04f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);

    // light coming from the sun position (origin)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lpos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat ldif[] = { 1.0f, 1.0f, 0.9f, 1.0f };
    GLfloat lamb[] = { 0.05f, 0.05f, 0.05f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  ldif);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lamb);

    initStars();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("Solar System - George Kiptoo");

    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);
    glutMainLoop();

    return 0;
}

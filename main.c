#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// cámara:
float angulo = 0.0f, beta = 0.0f, vel = 10.0f;
float camX = -9.0f, camY = 1.0f, camZ = 0.0f;
float r = 10.0f;
float fov = 30.0f;
#define PI 3.1416
int ratonOrigenX, ratonOrigenY, ratonPulsado = 0;
float velRaton = 0.5f;
int modoDibujo = 0;
int proyeccion = 0;

GLuint listaCogaRecu, texturaCogaRecu;

GLuint cargarTexturaPNG(const char *ruta)
{
    int ancho, alto, canales;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *datos = stbi_load(ruta, &ancho, &alto, &canales, STBI_rgb_alpha);
    
    if (!datos)
    {
        printf("Error al cargar la imagen: %s\n", stbi_failure_reason());
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        ancho, alto, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, datos);

    // opciones de filtrado y wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(datos);
    return texID;
}

void reloj_triangle()
{
    glPolygonMode(GL_FRONT, GL_LINE);
    glLineWidth(2.0f);
    glColor3d(0, 0, 0);
    glBegin(GL_TRIANGLES);

    // octágono
    glVertex3f(1, 4, 0);
    glVertex3f(-1, 4, 0);
    glVertex3f(0, 0, 0);

    glVertex3f(3, 2, 0);
    glVertex3f(1, 4, 0);
    glVertex3f(0, 0, 0);

    glVertex3f(3, -2, 0);
    glVertex3f(3, 2, 0);
    glVertex3f(0, 0, 0);

    glVertex3f(1, -4, 0);
    glVertex3f(3, -2, 0);
    glVertex3f(0, 0, 0);

    glVertex3f(-1, -4, 0);
    glVertex3f(1, -4, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(-3, -2, 0);
    glVertex3f(-1, -4, 0);
    glVertex3f(0, 0, 0);

    glVertex3f(-3, 2, 0);
    glVertex3f(-3, -2, 0);
    glVertex3f(0, 0, 0);

    glVertex3f(-1, 4, 0);
    glVertex3f(-3, 2, 0);
    glVertex3f(0, 0, 0);

    glBegin(GL_TRIANGLES);
    glColor3d(1, 0, 0);
    glVertex3f(0.2, 0, 0);
    glVertex3f(0, 4, 0);
    glVertex3f(-0.2, 0, 0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.2, 0, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(-0.2, 0, 0);
    glEnd();
    glLineWidth(1.0f);
}

void reloj_strip()
{
    glLineWidth(2.0f);

    glPolygonMode(GL_FRONT, GL_LINE);
    glColor3d(0, 0, 0);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(1, 4, 0);
    glVertex3f(-1, 4, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(-3, 2, 0);
    glVertex3f(-3, -2, 0);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-3, -2, 0);
    glVertex3f(-1, -4, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, -4, 0);
    glVertex3f(3, -2, 0);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(3, -2, 0);
    glVertex3f(3, 2, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 4, 0);
    glEnd();

    // agujas (creo que é imposible aproveitar vertices para as agullas con strip e con fan)
    // esta e a unica forma na que non che quedan triangulos innecesarios apuntando ao revés
    glBegin(GL_TRIANGLE_STRIP);
    glColor3d(1, 0, 0);
    glVertex3f(0.2, 0, 0);
    glVertex3f(0, 4, 0);
    glVertex3f(-0.2, 0, 0);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(0.2, 0, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(-0.2, 0, 0);
    glEnd();
    glLineWidth(1.0f);
}

void segmento(float x1, float y1, float x2, float y2, char color)
{
    switch (color)
    {
    case 'r':
        glColor3f(1.0f, 0.0f, 0.0f);
        break;
    case 'g':
        glColor3f(0.0f, 1.0f, 0.0f);
        break;
    case 'b':
        glColor3f(0.0f, 0.0f, 1.0f);
        break;
    }
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glLineWidth(3.0f);

    glVertex3f(x1, y1, 0);
    glVertex3f(x2, y2, 0);
    glEnd();
    glLineWidth(1.0f);
}

void reloj_fan()
{
    // octágono
    glPolygonMode(GL_FRONT, GL_LINE);
    glLineWidth(2.0f);
    glColor3d(0, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 4, 0);
    glVertex3f(-1, 4, 0);
    glVertex3f(-3, 2, 0);
    glVertex3f(-3, -2, 0);
    glVertex3f(-1, -4, 0);
    glVertex3f(1, -4, 0);
    glVertex3f(3, -2, 0);
    glVertex3f(3, 2, 0);
    glVertex3f(1, 4, 0);
    glEnd();

    // agujas
    glColor3d(1, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    glColor3d(1, 0, 0);
    glVertex3f(0.2, 0, 0);
    glVertex3f(0, 4, 0);
    glVertex3f(-0.2, 0, 0);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.2, 0, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(-0.2, 0, 0);
    glEnd();

    glLineWidth(1.0f);
}
float giroManillar = 0.0f;
float giroRueda = 0.0f;

void actualizarGiroRueda()
{
    giroRueda += 0.3f * 360 / 2 / PI;
    if (giroRueda > 360)
        giroRueda -= 360;
    glutPostRedisplay();
    glutTimerFunc(1000, actualizarGiroRueda, 0);
}

void dibujarMarco()
{
    glPushMatrix();
    glScalef(1.0f, 0.5f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujarRueda()
{
    glutWireSphere(0.5, 20, 20); // Draw a sphere as the wheel
}

void bicicleta()
{
    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glScalef(1.2, 1.2, 1.2);
    dibujarMarco();
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0.6, 0, 0);
    glRotatef(giroManillar, 0, 1, 0);
    glRotatef(giroRueda, 0, 0, 1);
    glScalef(0.6, 0.6, 0.6);
    dibujarRueda();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.6, 0, 0);
    glRotatef(giroRueda, 0, 0, 1);
    glScalef(0.6, 0.6, 0.6);
    dibujarRueda();
    glPopMatrix();

    glutSwapBuffers();
}

void coga_recu()
{
    listaCogaRecu = glGenLists(1);
    glNewList(listaCogaRecu, GL_COMPILE);
        glPolygonMode(GL_FRONT, GL_FILL);

    glBegin(GL_TRIANGLES);
    // cara d-b-a (os colores non importan)
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(0, 0, 1);

    // cara d-c-b
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(1, 0, 0);

    // cara a-c-d
    glColor3f(1, 0, 1);
    glVertex3f(0, 0, 1);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);

    // cara frontal c-a-b (en esta proyectamos la textura)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaCogaRecu);
    glColor3f(0, 0, 1);

    // las coordenadas u,v de la textura se calculan desde la esq. inf. izq.
    // por ejemplo, este valor está justo encima del 10
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(0, 1, 0);

    // 0.5 de v - mitad de la imagen (justo debajo del 10)
    glTexCoord2f(0.0f, 0.5f);
    glVertex3f(0, 0, 1);

    glTexCoord2f(1.0f, 0.5f);
    glVertex3f(1, 0, 0);
    glDisable(GL_TEXTURE_2D);

    glEnd();

    glEndList();
}

void triangulos()
{
    GLfloat a[] = {-2, -2, 0};
    GLfloat b[] = {1, -2, 0};
    GLfloat c[] = {0, 0, 0};
    GLfloat d[] = {3, 0, 0};
    GLfloat e[] = {2, 4, 0};
    glColor3f(0.0, 0.0, 1.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glLineWidth(2.0f);
    glPolygonMode(GL_FRONT, GL_LINE);
    glBegin(GL_TRIANGLES);
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
    glVertex3fv(d);
    glVertex3fv(e);
    glEnd();
    glLineWidth(1.0f);
}

void triangulos_strip()
{
    GLfloat a[] = {-2, -2, 0};
    GLfloat b[] = {1, -2, 0};
    GLfloat c[] = {0, 0, 0};
    GLfloat d[] = {3, 0, 0};
    GLfloat e[] = {2, 4, 0};
    glColor3f(0.0, 0.0, 1.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glLineWidth(2.0f);
    glPolygonMode(GL_FRONT, GL_LINE);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
    glVertex3fv(d);
    glVertex3fv(e);
    glEnd();
    glLineWidth(1.0f);
}
void camera()
{

    if (angulo > 89.0f)
        angulo = 89.0f;
    if (angulo < -89.0f)
        angulo = -89.0f;
    float radY = beta * PI / 180.0f;
    float radX = angulo * PI / 180.0f;

    camX = r * cos(beta * PI / 180.0f);
    camZ = r * sin(beta * PI / 180.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);
}
void cuadricula()
{
    float tam = 7.0f;

    for (int i = -7; i <= 7; i++)
    {
        if (i == 0)
        {
            glColor3f(0.3, 0.3f, 0.3f);
            glLineWidth(2.0f);
        }
        else
        {
            glLineWidth(1.0f);

            glColor3f(0.5f, 0.5f, 0.5f);
        }
        glBegin(GL_LINES);
        glVertex3f(-tam, (float)i, 0.0f);
        glVertex3f(tam, (float)i, 0.0f);
        glEnd();
    }

    for (int i = -7; i <= 7; i++)
    {
        if (i == 0)
        {
            glLineWidth(2.0f);
            glColor3f(0.3, 0.3f, 0.3f);
        }
        else
        {
            glLineWidth(1.0f);

            glColor3f(0.5f, 0.5f, 0.5f);
        }
        glBegin(GL_LINES);
        glVertex3f((float)i, -tam, 0.0f);
        glVertex3f((float)i, tam, 0.0f);
        glEnd();
    }

    glLineWidth(1.0f);
}
void dibujarCubos()
{
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(5, 5, 0); // 3: define o centro do prisma en 5,5
    glPushMatrix();
    glRotatef(45.0f, 0, 1, 0); // 2: inclina 45º respecto a eixo Y
    glScalef(2, 1, 1);         // 1: rectángulo de 2 de largo
    glColor3f(1.0, 0, 0);      // rojo
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glRotatef(45.0f, 0, 0, 1); // 4: rotan ambas 45º no seu eixo z
    glTranslatef(0, -5, 0);    // 3: móvense ambas -5 no seu eixo Y
    glPushMatrix();
    glRotatef(90.0f, 0, 0, 1); // 2a: gira 90º respecto a eixo Z:
                               // queda 'tumbado' (o aumento ahora é no largo)
    glScalef(2, 1, 1);         // 1a: prisma de 2 de largo
    glColor3f(0, 1, 0);        // verde
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0f, 0, 0, 1); // 2.2b: xira 90º en eixo z, imitando o xiro do verde
    glTranslatef(3, 0, 0);     // 2.1b: móvese 3 cara o seu eixo X, apartandose do verde
    glPushMatrix();
    glRotatef(90.0f, 0, 0, 1); // 2b: gira 90º respecto a eixo Z:
                               // queda 'tumbado' (visualmente ahora é como o verde)
    glScalef(1, 2, 1);         // 1b: prisma de 2 de alto
    glColor3f(0, 0, 1);        // azul
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0f, 0, 0, 1); // 3:rotacion de 90º respecto do eixo z, queda abaixo
    glTranslatef(-8, 0, 0);    // 2:móvese a fora da cuadricula (esquerda) no eixo x
    glPushMatrix();
    glRotatef(45.0f, 0, 0, 1); // 1:rota 45º no seu eixo z: queda inclinado (rombo)
    glScalef(1, 1, 1);         // cubo
    glColor3f(1, 1, 0);        // amarillo
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();
    glLineWidth(1.0f);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    camera();
    cuadricula();
    switch (modoDibujo)
    {
    case 0:
        reloj_triangle();
        // printf("dibujando reloj triangle\n");
        break;
    case 1:
        reloj_strip();
        // printf("dibujando reloj strip\n");
        break;

    case 2:
        reloj_fan();
        // printf("dibujando reloj fan\n");
        break;
    case 3:
        // printf("ejercicio segmentos (enunciado)\n");
        segmento(2, 1, 4, 1, 'b');
        segmento(2, 1, 2, 5, 'g');
        break;

    case 4:
        // printf("ejercicio segmentos (resolto)\n");
        //(probar aqui a meter transformacions para ver se funcionan)
        glPushMatrix();
        glTranslatef(2, 3, 0);
        glScalef(1, 2, 1);
        glRotatef(90.0, 0, 0, 1);
        glTranslatef(-3, -1, 0);
        segmento(2, 1, 4, 1, 'b');
        glPopMatrix();

        glPushMatrix();
        glTranslatef(3, -3, 0);
        glScalef(1, 2, 1);
        glRotatef(90.0, 0, 0, 1);
        segmento(2, 1, 4, 1, 'r');
        glPopMatrix();
        segmento(2, 1, 2, 5, 'g');
        break;

    case 5:
        // printf(" ejercicio cubos\n");
        dibujarCubos();
        break;
    case 6:
        // printf("aviso: a bicicleta tira a un fps e eso pode joder as funciones de mover a camara\n");
        bicicleta();
        break;
    case 7:
        // printf("ejercicio triangulos (triangles)\n");
        triangulos();
        break;
    case 8:
        // printf("ejercicio triangulos (triangles strip)\n");
        triangulos_strip();
        break;
    case 9:
        // printf("figura do ej 1 de coga recu");
        glCallList(listaCogaRecu);
        break;
    }
    glutSwapBuffers();
    if (modoDibujo != 6)
        glutPostRedisplay(); // a bicicleta tira a 1fps
}

void perspectiva(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (proyeccion == 0)
    {
        if (w <= h)
            glOrtho(-10.0, 10.0, -10.0 * (float)h / (float)w, 10.0 * (float)h / (float)w, 0.01, 100.0);
        else
            glOrtho(-10.0 * (float)w / (float)h, 10.0 * (float)w / (float)h, -10.0, 10.0, 0.01, 100.0);
    }
    else
    {
        if (w <= h)
            gluPerspective(fov, (float)w / (float)h, 0.01, 100.0);
        else
            gluPerspective(fov * (float)h / (float)w, (float)w / (float)h, 0.01, 100.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

void resetCamera(int pos)
{
    angulo = 0.0f;
    beta = 90.0f;
    vel = 0.1f;
    if (pos == 0) // posición inicial
    {
        camX = 0;
        camY = 0;
        camZ = 20;

        angulo = -45;
    }
    else if (pos == 1) // vista desde arrba
    {
        camX = 0.0f;
        camY = 8.0f;
        camZ = 0.0f;
        angulo = -90;
    }
    else if (pos == 2) // vista plana
    {
        camX = -9.0f;
        camY = 1.0f;
        camZ = 0.0f;
        angulo = 0;
    }
    fov = 45.0f;
    camera();
}

// ? --- ENTRADA --- ? //
void ratonMov(int x, int y)
{
    if (ratonPulsado)
    {
        int dx = x - ratonOrigenX;
        int dy = y - ratonOrigenY;

        beta -= dx * 0.2f;
        angulo += dy * 0.2f;
        ratonOrigenX = x;
        ratonOrigenY = y;

        camera();
        glutPostRedisplay();
    }
}

void ratonClick(int tecla, int raton, int x, int y)
{
    if (tecla == GLUT_LEFT_BUTTON)
    { //? permite arrastrar co rato (clic esquerdo)
        if (raton == GLUT_DOWN)
        {
            ratonPulsado = 1;
            ratonOrigenX = x;
            ratonOrigenY = y;
        }
        else
        {
            ratonPulsado = 0;
        }
    }
}

void teclado(unsigned char tecla, int x, int y)
{
    switch (tecla)
    {
    case 'w': // Avanzar
        r += vel;
        break;
    case 's': // Retroceder
        r -= vel;
        break;

    case 'a':         // rotar á esquerda
        beta -= 2.0f; // reduce ángulo (gira a la izquierda)
        if (beta < 0)
            beta += 360;
        break;
    case 'd':         // rotar á dereita
        beta += 2.0f; // aumenta ángulo (gira a la derecha)
        if (beta >= 360)
            beta -= 360;
        break;

    case 'q': // Subir
        camY += vel;
        break;
    case 'e': // Baixar
        camY -= vel;
        break;

    // Posiciones cámara:
    case 'c':
        resetCamera(0);
        break;
    case 'x':
        resetCamera(1);
        break;
    case 'z':
        resetCamera(2);
        break;
    case 'p':
        proyeccion = (proyeccion + 1) % 2;
        if (proyeccion == 0)
            printf("Proyección ortográfica\n");
        else
            printf("Proyección perspectiva\n");
        perspectiva(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;

    // Cambiar modo de dibuxo:
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
        modoDibujo = (tecla - '0');
        break;

    // Zoom:
    case '+':
        if (fov > 10.0f)
            fov -= 2.0f;
        perspectiva(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case '-':
        if (fov < 90.0f)
            fov += 2.0f;
        perspectiva(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case 'm': // Imprimir variables da cámara
        printf("Posición de la cámara: X=%.2f, Y=%.2f, Z=%.2f\n", camX, camY, camZ);
        printf("Ángulo de la cámara: %.2f\n", angulo);
        printf("Beta de la cámara: %.2f\n", beta);
        printf("Velocidad de la cámara: %.2f\n", vel);
        printf("FOV de la cámara: %.2ff\n", fov);
        break;
    case 27: // ESC para saír
        exit(0);
    }
    camera();
    glutPostRedisplay();
}
void openGLInit()
{
    glClearDepth(1.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    resetCamera(0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Test");
    openGLInit();
    printf("cámara con raton/wasdqe, 1-9 para cambiar ejercicio, p para ortografica/perspectiva, +/- para zoom\n");
    actualizarGiroRueda();
    glutReshapeFunc(perspectiva);
    glutKeyboardFunc(teclado);
    coga_recu();

    glEnable(GL_TEXTURE_2D);
    texturaCogaRecu = cargarTexturaPNG("textura.png");
    glutDisplayFunc(display);
    glutMotionFunc(ratonMov);
    glutMouseFunc(ratonClick);

    glutMainLoop();
    return 0;
}

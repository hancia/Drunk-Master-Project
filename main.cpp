#include <GL/glut.h>
#include <stdlib.h>

enum{
    WIRE_SPHERE,
};

int object = WIRE_SPHERE;

const GLdouble left = - 10.0;
const GLdouble right = 10.0;
const GLdouble bottom = - 10.0;
const GLdouble top = 10.0;
const GLdouble near = 50.0;
const GLdouble far = 70.0;

GLfloat scale = 1.0;

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

GLfloat translatex = 0.0;
GLfloat translatey = 0.0;

void Display(){
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glTranslatef( 0, 0, -( near + far ) / 2 );
    glTranslatef( translatex, translatey, 0.0 );
    glScalef( scale, scale, scale );

    glRotatef( rotatex, 1.0, 0, 0 );
    glRotatef( rotatey, 0, 1.0, 0 );

    glColor3f( 0.0, 0.0, 0.0 );

    switch( object ){

        case WIRE_SPHERE:
            glutWireSphere( 1.0, 20, 10 );
            break;

    }

    glFlush();
    glutSwapBuffers();
}

void Reshape( int width, int height ){
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( left, right, bottom, top, near, far );
    Display();
}

int main( int argc, char * argv[] ){
    glutInit( & argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize( 1024, 768 );

    glutCreateWindow( "DRUNKMASTER" );

    glutDisplayFunc( Display );
    glutReshapeFunc( Reshape );

    glutMainLoop();
    return 0;
}
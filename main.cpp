#include<GL/gl.h>
#include<GL/glut.h>
#include<stdio.h>


GLuint bottle;

void loadObj(char *fname)
{
    FILE *fp;
    int read;
    GLfloat x, y, z;
    char ch;
    bottle=glGenLists(1);
    fp=fopen(fname,"r");
    if (!fp)
    {
        printf("can't open file %s\n", fname);
        exit(1);
    }
    glPointSize(2.0);
    glNewList(bottle, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_POINTS);
        while(!(feof(fp)))
        {
            read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
            if(read==4&&ch=='v')
            {
                glVertex3f(x,y,z);
            }
        }
        glEnd();
    }
    glPopMatrix();
    glEndList();
    fclose(fp);
}


void reshape(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void drawBottle()
{
    glPushMatrix();
    glTranslatef(0,0,-150);
    glColor3f(0,0,0);
    glScalef(10,10,10);
    glRotatef(0,0,1,0);
    glCallList(bottle);
    glPopMatrix();
}

void display(void)
{
    glClearColor (1,1,1,1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    drawBottle();
    glutSwapBuffers();

}

int main(int argc,char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(1200,700);
    glutInitWindowPosition(20,20);
    glutCreateWindow("DRUNKMASTER");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    loadObj("/home/pluto/Drunk-Master-Project/models/bottle.obj"); //Nie dziala sciezka wzgledna, musisz ustawic wlasna aby uruchomic
    glutMainLoop();
    return 0;
}
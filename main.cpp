/*
(c) Janusz Ganczarski (Power)
http://www.januszg.hg.pl
JanuszG(małpeczka)enter.net.pl
*/

#include <GL/glut.h>
#include <stdlib.h>

// funkcja generująca scenę 3D

void Display()
{
    // kolor tła - zawartość bufora koloru
    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    // czyszczenie bufora koloru
    glClear( GL_COLOR_BUFFER_BIT );

    // kolor kwadratu
    glColor3f( 1.0, 0.0, 0.0 );

    // początek definicji wielokąta
    glBegin( GL_POLYGON );

    // kolejne wierzchołki wielokąta
    glVertex3f( 0.0, 0.0, 0.0 );
    glVertex3f( 0.0, 1.0, 0.0 );
    glVertex3f( 1.0, 1.0, 0.0 );
    glVertex3f( 1.0, 0.0, 0.0 );

    // koniec definicji prymitywu
    glEnd();

    // skierowanie poleceń do wykonania
    glFlush();

    // zamiana buforów koloru
    glutSwapBuffers();
}

// zmiana wielkości okna

void Reshape( int width, int height )
{
    // generowanie sceny 3D
    Display();
}

// stałe do obsługi menu podręcznego

enum
{
    EXIT // wyjście
};

// obsługa menu podręcznego

void Menu( int value )
{
    switch( value )
    {
        // wyjście
        case EXIT:
            exit( 0 );
    }
}

int main( int argc, char * argv[] )
{
    // inicjalizacja biblioteki GLUT
    glutInit( & argc, argv );

    // inicjalizacja bufora ramki
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );

    // rozmiary głównego okna programu
    glutInitWindowSize( 400, 400 );

    // utworzenie głównego okna programu
    glutCreateWindow( "Kwadrat 1" );

    // dołączenie funkcji generującej scenę 3D
    glutDisplayFunc( Display );

    // dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
    glutReshapeFunc( Reshape );

    // utworzenie menu podręcznego
    glutCreateMenu( Menu );

    // dodatnie pozycji do menu podręcznego
#ifdef WIN32

    glutAddMenuEntry( "Wyjście", EXIT );
#else

    glutAddMenuEntry( "Wyjscie", EXIT );
#endif

    // określenie przycisku myszki obsługującej menu podręczne
    glutAttachMenu( GLUT_RIGHT_BUTTON );

    // wprowadzenie programu do obsługi pętli komunikatów
    glutMainLoop();
    return 0;
}
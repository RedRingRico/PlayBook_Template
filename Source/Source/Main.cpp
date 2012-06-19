#include <iostream>
#include <stdlib.h>
#include <stdio.h>

int main( int p_Argc, char **p_ppArgv )
{
	std::cout << "std::cout: Hello, PlayBook!" << std::endl;
	fprintf( stderr, "printf: Hello, PlayBook!\n" );

	fflush( stderr );
	return 0;
}


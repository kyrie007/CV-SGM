#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"

#define ROWS		(int)480
#define COLUMNS		(int)640

void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );

int main( int argc, char **argv )
{

	int				i, j, k, threshold[3], max[3], x, y, s,m,n;
	FILE			*fp;
	unsigned char	image[ROWS][COLUMNS], ximage[ROWS][COLUMNS], yimage[ROWS][COLUMNS], head[32];
	static unsigned char simage[ROWS][COLUMNS], bimage[ROWS][COLUMNS];
	static float      ximageprime[ROWS][COLUMNS], yimageprime[ROWS][COLUMNS];
	static double   simageprime[ROWS][COLUMNS];
	char			filename[6][50], ifilename[50], ch;

	strcpy( filename[0], "image1" );
	strcpy( filename[1], "image2" );
	strcpy( filename[2], "image3" );
	header ( ROWS, COLUMNS, head );

	printf( "Maximum of Gx, Gy, SGM\n" );
	for ( k = 0; k < 3; k ++)
	{
		clear( ximage );
		clear( yimage );

		/* Read in the image */
		strcpy( ifilename, filename[k] );
		if (( fp = fopen( strcat(ifilename, ".raw"), "rb" )) == NULL )
		{
		  fprintf( stderr, "error: couldn't open %s\n", ifilename );
		  exit( 1 );
		}			
		for ( i = 0; i < ROWS ; i++ )
		  if ( fread( image[i], sizeof(char), COLUMNS, fp ) != COLUMNS )
		  {
			fprintf( stderr, "error: couldn't read enough stuff\n" );
			exit( 1 );
		  }
		fclose( fp );

		max[0] = 0; //maximum of Gx
		max[1] = 0; //maximum of Gy
		max[2] = 0; //maximum of SGM

		/* Compute Gx, Gy, SGM, find out the maximum and normalize */


		for (m = 1; m < ROWS-1; m++)
		{
			for (n = 1; n < COLUMNS-1; n++)
			{
				ximageprime[m][n] = abs(image[m-1][n+1]+2*image[m][n+1]+image[m+1][n+1])-(image[m-1][n-1]+2*image[m][n-1]+image[m+1][n-1]);
				yimageprime[m][n] = abs(image[m+1][n-1]+2*image[m+1][n]+image[m+1][n+1])-(image[m-1][n+1]+2*image[m-1][n]+image[m-1][n-1]);
				simageprime[m][n] = pow(ximageprime[m][n],2)+pow(yimageprime[m][n],2);
			}
		}
						
		for (m = 1; m < ROWS-1; m++)
		{
			for (n = 1; n < COLUMNS-1; n++)
			{
				if (ximageprime[m][n] > max[0])
				{
					max[0] = ximageprime[m][n];
				}
				
	        	if (yimageprime[m][n] > max[1])
				{
					max[1] = yimageprime[m][n];
				}
				
				if (simageprime[m][n] > max[2])
				{
					max[2] = simageprime[m][n];
				}

			}
		}

		
		//normalize 
		
		for (m = 1; m < ROWS-1; m++)
		{
			for (n = 1; n < COLUMNS-1; n++)
			{
				ximage[m][n] = ximageprime[m][n]/max[0]*255;
				yimage[m][n] = yimageprime[m][n]/max[1]*255;
				simage[m][n] = simageprime[m][n]/max[2]*255;
			}
		}
		
		

		/* Write Gx to a new image*/
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-x.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for ( i = 0 ; i < ROWS ; i++ ) fwrite( ximage[i], 1, COLUMNS, fp );
	    fclose( fp );
					
		/* Write Gy to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-y.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for ( i = 0 ; i < ROWS ; i++ ) fwrite( yimage[i], 1, COLUMNS, fp );
		fclose( fp );

		

		/* Write SGM to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-s.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
	    for ( i = 0 ; i < ROWS ; i++ ) fwrite( simage[i], 1, COLUMNS, fp );
		fclose( fp );
		
		
		/* Compute the binary image */
		
		threshold[0] = 31;
		threshold[1] = 10;
		threshold[2] = 17;

		for(m = 0; m < ROWS; m++)
		{
			for (n = 0; n < COLUMNS; n++)
			{
				if (simage[m][n] < threshold[k])
				{
					bimage[m][n] = 0;
				}
				if (simage[m][n] >= threshold[k])
				{
					bimage[m][n] = 255;
				}
			}
		}




		/* Write the binary image to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-b.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
        for ( i = 0 ; i < ROWS ; i++ ) fwrite( bimage[i], 1, COLUMNS, fp );
		fclose( fp );

		printf( "%d %d %d\n", max[0], max[1], max[2] );
		
		/* Save the original image as ras */
		strcpy(ifilename, filename[k]);
		if (!(fp = fopen(strcat(ifilename, "-o.ras"), "wb")))
		{
			fprintf(stderr, "error: could not open %s\n", ifilename);
			exit(1);
		}
		fwrite(head, 4, 8, fp);
		for (i = 0; i < ROWS; i++) fwrite(image[i], 1, COLUMNS, fp);
		fclose(fp);

	}
	

	printf( "Press any key to exit: " );
	gets( &ch );
	return 0;
}

void clear( unsigned char image[][COLUMNS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLUMNS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */
	
	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++; 
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++; 
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;
	
	ch = (char*)&num;
	head[19] = *ch;
	ch ++; 
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;
	

	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}

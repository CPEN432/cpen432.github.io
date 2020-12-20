/* fdctref.c, forward discrete cosine transform, double precision           */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */


#define PI 3.14159265358979323846


/* private data */
static double c[8][8]; /* transform coefficients */

int main ()
{

  short *block;
  int i, j, k;
  double s;
  double tmp[64];

  int t;
  for(t = 0; t < 6; t++){
	  for (i=0; i<8; i++)
		for (j=0; j<8; j++)
		{
		  s = 0.0;

		  for (k=0; k<8; k++)
			s += c[j][k] * block[8*i+k];

		  tmp[8*i+j] = s;
		}

	  for (j=0; j<8; j++)
		for (i=0; i<8; i++)
		{
		  s = 0.0;

		  for (k=0; k<8; k++)
			s += c[i][k] * tmp[8*k+j];

		  block[8*i+j] = (int)(s+0.499999);
		  /*
		   * reason for adding 0.499999 instead of 0.5:
		   * s is quite often x.5 (at least for i and/or j = 0 or 4)
		   * and setting the rounding threshold exactly to 0.5 leads to an
		   * extremely high arithmetic implementation dependency of the result;
		   * s being between x.5 and x.500001 (which is now incorrectly rounded
		   * downwards instead of upwards) is assumed to occur less often
		   * (if at all)
		   */
		}
  }
}

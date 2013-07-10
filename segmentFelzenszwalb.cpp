#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mex.h>

#include "./image.h"
#include "./misc.h"
#include "./pnmfile.h"
#include "./image.h"
#include "./segment-image.h"

void mexFunction(int nOutput, mxArray *pOutput[], /* Output variables */
  int nInput, const mxArray *pInput[]) /* Input variables */
{
  mexPrintf("Let's run the segmentation program of Felzenszwalb and Huttenlocher :) !\n"); 
    
   /* -----------------------------------------------------------------
   *                         Check the arguments
   * -------------------------------------------------------------- */

  if (nInput < 1) {
    mexErrMsgTxt ( "To few input arguments.");
    return;
  }
  
  if (nOutput > 1) {
    mexErrMsgTxt ( "Too many output arguments.");
    return;
  }
  
  if ( (nOutput == 0) && (nInput < 5) ) {
    mexErrMsgTxt ( "No destination for segmentation result specified - chose either return value or string for destination!");
    return;
  }  
  
  
   /* -----------------------------------------------------------------
   *                      Get the arguments
   * -------------------------------------------------------------- */  
  
   bool verbose ( false );
   if ( nInput >= 6)
    verbose = mxGetLogicals( pInput[5] );
   
  /* Get string of input image*/  
  image<rgb> * imgInput;
  
  /* was the input image given as a string?*/ 
  if ( mxGetM( pInput[0] ) == 1 )
  {
    char *input;     
    input = (char *) mxCalloc( mxGetN(pInput[0] )+1, sizeof(char) );
    mxGetString(pInput[0], input, mxGetN( pInput[0] )+1);
    if ( verbose ) 
      mexPrintf("The input string is:  %s\n", input);    
    
    imgInput = loadPPM( input );
  }
  /* the image was directly passed as matlab matrix*/ 
  else
  {    
    /*double * imgInputMatlab = mxGetPr( pInput[0] );*/
    signed char* imgInputMatlab = (signed char *)mxGetData( pInput[0] );
    
    size_t K = mxGetNumberOfDimensions(  pInput[0] );
    const mwSize *N = mxGetDimensions(  pInput[0] );
    
    uint width  ( N[0] );
    uint height ( N[1] );
  
    
    imgInput = new image<rgb>( height, width, 0 );
    
     /* start with RED channel*/     
     /* keep in mind that matlab stores images col by width, whereas C does it the other way round*/
     for ( uint y = 0; y < width; y++)
     {
       for ( uint x = 0; x < height; x++)
       {
	  imRef(imgInput, x, y).r = imgInputMatlab[y + width*(x + height*0) ];
	  imRef(imgInput, x, y).g = imgInputMatlab[y + width*(x + height*1) ];
	  imRef(imgInput, x, y).b = imgInputMatlab[y + width*(x + height*2) ];
       }
     }    
  }     
  
  char buf [1024] ;
  
  /* Get sigma */  
  double sigma;
  if ( nInput < 2) 
  {
    sigma = 0.5; 
  }
  else
    sigma = mxGetScalar( pInput[1] ); 
  if ( verbose ) 
    mexPrintf("The sigma value is:  %f\n", sigma);
    
  /* Get k */  
  int k;
  if ( nInput < 3) 
  {
    k = 500;
  }
  else
    k = mxGetScalar( pInput[2] );
  
  if ( verbose ) 
    mexPrintf("The k is:  %i\n", k);
   
  /* Get minSize*/  
  int minSize;  
  if ( nInput < 4)
  {
   minSize = 20;
  }
  else
    minSize = mxGetScalar(  pInput[3] );
  
  if ( verbose ) 
    mexPrintf("The minSize is:  %i\n", minSize);
   

   
  /* Get string of output image if given*/  
  char * output;
  if (nInput >=  5)
  {
    output = (char *) mxCalloc(mxGetN(pInput[4])+1, sizeof(char));
    mxGetString(pInput[4], output, mxGetN(pInput[4])+1);
    
    if ( verbose ) 
      mexPrintf("The output string is:  %s\n", output);
  }
    
  if ( verbose )
    mexPrintf("image loaded, now start segmentation\n");  
   
   /* -----------------------------------------------------------------
   *                       Do the main stuff 
   *                        >segmentation<
   * -------------------------------------------------------------- */   
   
   int num_ccs; 
   image<rgb> * imgResult = segment_image(imgInput, sigma, k, minSize, &num_ccs); 
   if ( verbose )
     mexPrintf("segmentation done\n");
  
   
   /* -----------------------------------------------------------------
   *                      Conversion to Matlab structures
   * -------------------------------------------------------------- */      
   
   if ( nOutput == 0 )
   {
     savePPM( imgResult, output );
     mexPrintf("save results\n"); 
   }
   else
   {
     mexPrintf("convert to matlab structure and hand result back to main program\n"); 
     /* convert segmentation result to matlax matrix*/
     int width ( imgResult->width() );
     int height (imgResult->height() );
        
     int numberOfPixels ( width * height * sizeof(rgb)  );
     
     if ( verbose )
       mexPrintf("The number of Pixel in your resulting image is:  %i -- summing over all color channels\n", numberOfPixels);
     
     /*if ( verbose )*/
       mexPrintf(" width: %i, height: %i\n", width, height);
     
     /* keep in mind that matlab stores images  height × width × color, whereas C does it the other way round*/
     int dims[] = {height, width,3};
     pOutput[0] = mxCreateNumericArray (3, dims, mxUINT8_CLASS, mxREAL);
     unsigned char *out1; /* pointer to output 1 */
     out1 = (unsigned char *)mxGetPr( pOutput[0] ); /* pointer to output 1 */

          
     /* start with RED channel*/     
     /* keep in mind that matlab stores images col by width, whereas C does it the other way round*/
     for ( uint x = 0; x < width; x++)
     {
       uint rowOffset ( x*height );
       for ( uint y = 0; y < height; y++)
       {
	  out1[rowOffset + y ] = (double) (imRef(imgResult, x, y)).r;
       }
     }
     
     /* GREEN channel*/
     uint channelOffsetG ( width * height );
     for ( uint x = 0; x < width; x++)
     {
       uint rowOffset ( x*height );
        for ( uint y = 0; y < height; y++)
	{
	   out1[channelOffsetG + rowOffset + y ] = (double) (imRef(imgResult, x, y)).g;
	}
     }
     
     /* BLUE channel*/
     uint channelOffsetB ( 2 * width * height );
     for ( uint x = 0; x < width; x++)
     {
       uint rowOffset ( x*height );
        for ( uint y = 0; y < height; y++)
	{
	   out1[channelOffsetB + rowOffset + y ] = (double) (imRef(imgResult, x, y)).b;
	}
     }

    /* done */
   }
   
   /* don't waste memory*/
   if ( imgInput != NULL )
     delete imgInput;
  
  return;
}
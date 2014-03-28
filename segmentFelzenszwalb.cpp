#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mex.h>


#include "./image.h"
#include "./misc.h"
#include "./pnmfile.h"
#include "./image.h"
#include "./segment-image.h"
#include "./segment-image-labelOutput.h"

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y)) /* min and max value macros */
#endif
#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

void mexFunction(int nOutput, mxArray *pOutput[], /* Output variables */
  int nInput, const mxArray *pInput[]) /* Input variables */
{ 
   
   /* -----------------------------------------------------------------
   *                         Check the arguments
   * -------------------------------------------------------------- */

  if (nInput < 1) {
    mexErrMsgTxt ( "To few input arguments.");
    return;
  }
  
  if (nOutput > 2) {
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
  
  bool b_verbose ( false );
  if ( nInput >= 7)
  {
    if ( mxIsLogicalScalar( pInput[6] ) && mxIsLogicalScalarTrue( pInput[6] ) )
    {
      b_verbose = true;
    }
    else
      b_verbose = false; 
  }
   
  /* Get string of input image*/  
  image<rgb> * imgInput;
  
  /* was the input image given as a string?*/ 
  if ( mxGetM( pInput[0] ) == 1 )
  {
    char *input;     
    input = (char *) mxCalloc( mxGetN(pInput[0] )+1, sizeof(char) );
    mxGetString(pInput[0], input, mxGetN( pInput[0] )+1);
    if ( b_verbose ) 
      mexPrintf("The input string is:  %s\n", input);    
    
    imgInput = loadPPM( input );
    mxFree ( input );
  }
  /* the image was directly passed as matlab matrix*/ 
  else
  {    
    /*double * imgInputMatlab = mxGetPr( pInput[0] );*/
    signed char* imgInputMatlab = (signed char *)mxGetData( pInput[0] );
    
    size_t K = mxGetNumberOfDimensions(  pInput[0] );
    const mwSize *N = mxGetDimensions(  pInput[0] );
    
    uint height  ( N[0] ); // matlab gives first height
    uint width ( N[1] ); // matlab gives then width
  
    
    imgInput = new image<rgb>( width, height, 0 );
    
    /* start with RED channel*/     
    /* keep in mind that matlab stores images col by width, whereas C does it the other way round*/
    for ( uint y = 0; y < height; y++)
    {
      for ( uint x = 0; x < width; x++)
      {
        imRef(imgInput, x, y).r = imgInputMatlab[y + height*(x + width*0) ];
        imRef(imgInput, x, y).g = imgInputMatlab[y + height*(x + width*1) ];
        imRef(imgInput, x, y).b = imgInputMatlab[y + height*(x + width*2) ];
      }
    }    
  }     
  
  char buf [1024] ;
  
  /* Get sigma */  
  double d_sigma;
  if ( nInput < 2) 
  {
    d_sigma = 0.5; 
  }
  else
    d_sigma = mxGetScalar( pInput[1] ); 
  if ( b_verbose ) 
    mexPrintf("The sigma value is:  %f\n", d_sigma);
    
  /* Get k */  
  int i_k;
  if ( nInput < 3) 
  {
    i_k = 500;
  }
  else
    i_k = mxGetScalar( pInput[2] );
  
  if ( b_verbose ) 
    mexPrintf("The k is:  %i\n", i_k);
   
  /* Get minSize*/  
  int i_minSize;  
  if ( nInput < 4)
  {
    i_minSize = 50;
  }
  else
    i_minSize = mxGetScalar(  pInput[3] );
  
  if ( b_verbose ) 
    mexPrintf("The minSize is:  %i\n", i_minSize);
   
  /* Get bool whether to compute the label img (int) or colored img (rgb)*/  
  bool b_computeColorOutput;
  if (nInput >=  5)
  {
    if ( mxIsLogicalScalar( pInput[4] ) && mxIsLogicalScalarTrue( pInput[4] ) )
    {
      b_computeColorOutput = true;
    }
    else
      b_computeColorOutput = false;   
  }
  else
  {
    b_computeColorOutput = false;
  }
  if ( b_verbose ) 
    mexPrintf("To we compute RGB colored segmentation result?  :  %i\n", b_computeColorOutput );
   
  /* Get string of output image if given*/  
  char * output;
  if (nInput >=  6)
  {
    output = (char *) mxCalloc(mxGetN(pInput[5])+1, sizeof(char));
    mxGetString(pInput[5], output, mxGetN(pInput[5])+1);
    
    if ( b_verbose ) 
      mexPrintf("The output string is:  %s\n", output);
  }
    
  if ( b_verbose )
    mexPrintf("image loaded, now start segmentation\n");  
  
   
  /* -----------------------------------------------------------------
  *                       Do the main stuff 
  *                        >segmentation<
  * -------------------------------------------------------------- */   
   
   
  int num_ccs; 
  image<rgb> * imgResultRGB = NULL;
  image<unsigned short> * imgResult  = NULL;
   
  if ( b_computeColorOutput )
  {
    imgResultRGB = segment_image(imgInput, d_sigma, i_k, i_minSize, &num_ccs); 
  }
  else
  {
    imgResult = segment_image_labelOutput(imgInput, d_sigma, i_k, i_minSize, &num_ccs); 
  }
       
  if ( b_verbose )
    mexPrintf("segmentation done\n");   
   
   
  /* -----------------------------------------------------------------
  *                      Conversion to Matlab structures
  * -------------------------------------------------------------- */      
   
  if ( nOutput == 0 )
  {
    //was a filename given to store the image in?
    if (nInput >=  6) 
    {
      if ( b_computeColorOutput )
        savePPM( imgResultRGB, output );
      else
        save_image( imgResult, output );
      mexPrintf("save results\n"); 
    }
    else
    {
      mexPrintf("ATTENTION -- Neither output variables nor filename to store the result to given!");
    }
  }
  else
  {
    if ( b_verbose )
      mexPrintf("convert to matlab structure and hand result back to main program\n"); 
     
    /* convert segmentation result to matlab matrix*/
        
    if ( b_computeColorOutput )
    {
      int width ( imgResultRGB->width() );
      int height (imgResultRGB->height() );       
      /* keep in mind that matlab stores images  height × width × color, whereas C does it the other way round*/
      int dims[] = {height, width,3};
      pOutput[0] = mxCreateNumericArray (3, dims, mxUINT8_CLASS, mxREAL);
      //unsigned char *out1; /* pointer to output 1 */
      //out1 = (unsigned char *)mxGetPr( pOutput[0] ); /* pointer to output 1 */
      //unsigned char *out1; /* pointer to output 1 */
      //out1 = (unsigned char *)mxGetPr( pOutput[0] ); /* pointer to output 1 */
      unsigned char *out1; /* pointer to output 1 */
      out1 = (unsigned char *)mxGetData( pOutput[0] ); /* pointer to output 1 *	

              
      /* start with RED channel*/     
      /* keep in mind that matlab stores images col by width, whereas C does it the other way round*/
      for ( uint x = 0; x < width; x++)
      {
        uint rowOffset ( x*height );
        for ( uint y = 0; y < height; y++)
        {
          out1[rowOffset + y ] = (unsigned char) (imRef(imgResultRGB, x, y)).r;
        }
      }
        
      /* GREEN channel*/
      uint channelOffsetG ( width * height );
      for ( uint x = 0; x < width; x++)
      {
        uint rowOffset ( x*height );
          for ( uint y = 0; y < height; y++)
          {
            out1[channelOffsetG + rowOffset + y ] = (unsigned char) (imRef(imgResultRGB, x, y)).g;
          }
      }
        
      /* BLUE channel*/
      uint channelOffsetB ( 2 * width * height );
      for ( uint x = 0; x < width; x++)
      {
        uint rowOffset ( x*height );
          for ( uint y = 0; y < height; y++)
          {
            out1[channelOffsetB + rowOffset + y ] = (unsigned char) (imRef(imgResultRGB, x, y)).b;
          }
      }
    }
    else /* do not compute colored rgb segmentation image, but only an unsigned short-int img*/
    {
      int width ( imgResult->width() );
      int height (imgResult->height() ); 
      int dims[] = {height, width};
      
      if (num_ccs < 255)
      {
        pOutput[0] = mxCreateNumericArray (2, dims, mxUINT8_CLASS, mxREAL);
        unsigned char *out1; /* pointer to output 1 */
        out1 = (unsigned char *)mxGetPr( pOutput[0] ); /* pointer to output 1 */
        
        /* keep in mind that matlab stores images col by width, whereas C does it the other way round*/
        for ( uint x = 0; x < width; x++)
        {
          uint rowOffset ( x*height );
          for ( uint y = 0; y < height; y++)
          {
            out1[rowOffset + y ] = (unsigned char) (imRef(imgResult, x, y));
          }
        }
      }
      else
      {
        pOutput[0] = mxCreateNumericArray (2, dims, mxUINT16_CLASS, mxREAL);
        unsigned short *out1; /* pointer to output 1 */
        out1 = (unsigned short *)mxGetData( pOutput[0] ); /* pointer to output 1 *
        
        /* keep in mind that matlab stores images col by width, whereas C does it the other way round*/
        for ( uint x = 0; x < width; x++)
        {
          uint rowOffset ( x*height );
          for ( uint y = 0; y < height; y++)
          {
            out1[rowOffset + y ] = (unsigned short) (imRef(imgResult, x, y));
          }
        }
      }
    }
     
    /* return number of segments*/
    if ( nOutput >= 2 )
    {
      int dims[] = {1};
      if (num_ccs < 255)
      {
        pOutput[1] = mxCreateNumericArray(1,dims,mxUINT8_CLASS,mxREAL);
        unsigned char *out2; /* pointer to output 2 */
        out2 = (unsigned char *)mxGetPr( pOutput[1] ); /* pointer to output 2 */
          
        out2[0] = num_ccs;
      }
      else
      {
        pOutput[1] = mxCreateNumericArray(1,dims,mxINT16_CLASS,mxREAL);
        //unsigned char *out2; /* pointer to output 2 */
        //out2 = (unsigned char *)mxGetPr( pOutput[1] ); /* pointer to output 2 */
        unsigned short *out2; /* pointer to output 2 */
        out2 = (unsigned short *) mxGetData( pOutput[1] ); /* pointer to output 2 */
          
        out2[0] = num_ccs;

      }
       
      if ( b_verbose ) 
        mexPrintf("number of components: %i", num_ccs);
    }

    /* done */
  }
   
  /* don't waste memory*/
  if ( imgInput != NULL )
    delete imgInput;
  if ( imgResultRGB != NULL )
    delete imgResultRGB;
  if ( imgResult != NULL )
    delete imgResult;
  
  return;
}
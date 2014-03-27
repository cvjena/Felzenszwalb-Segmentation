#ifndef SEGMENT_IMAGE_LABELOUTPUT
#define SEGMENT_IMAGE_LABELOUTPUT

#include <cstdlib>
#include <map>
#include <limits>
#include "./image.h"
#include "./misc.h"
#include "./filter.h"
#include "./segment-graph.h"
#include "./segment-image.h"

#include <mex.h>

 /**
 * @brief Segment an image, returning an int-image giving the index of the corresponding segment for every pixel
 * @author Alexander Freytag
 * @date 27-03-2014 ( dd-mm-yyyy, last updated)
 * 
 * @param[in] im: image to segment
 * @param[in] sigma: to smooth the image
 * @param[in] c: constant for treshold function
 * @param[in] min_size: minimum component size (enforced by post-processing stage)
 * @param[in] num_ccs: number of connected components in the segmentation
 * 
 * @param[out] output int-image giving the index of the corresponding segment for every pixel
 */
image<int> *segment_image_labelOutput(   image<rgb> *im,
                                         float sigma,
                                         float c,
                                         int min_size,
                                         int *num_ccs
                                     )
{
  int width = im->width();
  int height = im->height();

  image<float> *r = new image<float>(width, height);
  image<float> *g = new image<float>(width, height);
  image<float> *b = new image<float>(width, height);

  // smooth each color channel  
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      imRef(r, x, y) = imRef(im, x, y).r;
      imRef(g, x, y) = imRef(im, x, y).g;
      imRef(b, x, y) = imRef(im, x, y).b;
    }
  }
  image<float> *smooth_r = smooth(r, sigma);
  image<float> *smooth_g = smooth(g, sigma);
  image<float> *smooth_b = smooth(b, sigma);
  delete r;
  delete g;
  delete b;
 
  // build graph
  edge *edges = new edge[width*height*4];
  int num = 0;
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      if (x < width-1)
      {
        edges[num].a = y * width + x;
        edges[num].b = y * width + (x+1);
        edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y);
        num++;
      }

      if (y < height-1)
      {
        edges[num].a = y * width + x;
        edges[num].b = (y+1) * width + x;
        edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x, y+1);
        num++;
      }

      if ( (x < width-1) && (y < height-1) )
      {
        edges[num].a = y * width + x;
        edges[num].b = (y+1) * width + (x+1);
        edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y+1);
        num++;
      }

      if ( (x < width-1) && (y > 0) )
      {
        edges[num].a = y * width + x;
        edges[num].b = (y-1) * width + (x+1);
        edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y-1);
        num++;
      }
    }
  }
  delete smooth_r;
  delete smooth_g;
  delete smooth_b;

  // segment
  universe *u = segment_graph(width*height, num, edges, c);
  
  // post process small components 
  for (int i = 0; i < num; i++)
  {
    int a = u->find(edges[i].a);
    int b = u->find(edges[i].b);
    if ( (a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size))  )
      u->join(a, b);
  }
  delete [] edges;
  
  //NOTE since u->num_sets() is an int, we are not able to handle more than 
  // max_int segments ( and therefore also not imgs with more pixels)
  *num_ccs = u->num_sets();
  
  image<int> *output = new image<int>(width, height);
 
  //how many different regions do we finally have, and which of them are the corresponding indices?
  std::map<int,int> regionLabels;
  int idx ( 0 );
  
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      int comp = u->find(y * width + x);
      if ( regionLabels.find( comp ) == regionLabels.end() )
      {
        regionLabels.insert( std::pair<int,int>(comp,idx) );
        idx++;
      }
    }
  }  
  
  // insert region indices into img, map indices from current ones to consecutive ones
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      int comp = u->find(y * width + x);
      imRef(output, x, y) = regionLabels.find(comp)->second;
    }
  }  
 
  delete u;

  return output;
}

#endif
# A matlab wrapper for unsupervised segmentation of images based on Felzenszwalb and Huttenlocher

## Matlab aspects

### Compilation
simply run 
```
compileFelzenszwalbSegmentation.m
```

### Setup - safely add repo to search path
To make Matlab familiar with all relevant paths (there are only 2 currently) go to the root folder and execute
```
initWorkspaceSegmentation.m
```


### Demo
1) Run  
```
demoFelzenszwalbSegmentation.m
```  
which requires Matlabs GUI to show images and segmentation results

2) Inspect the demo file to learn how to setup variables, and how to call the underlying mex functions

## C++ aspects

Implementation of the segmentation algorithm described in:

*Efficient Graph-Based Image Segmentation
Pedro F. Felzenszwalb and Daniel P. Huttenlocher
International Journal of Computer Vision, 59(2) September 2004.*

The program [segment.cpp, note by A. Freytag] takes a color image (PPM format) and produces a segmentation
with a random color assigned to each region.

1) Type ```make``` to compile ```segment.cpp```

2) Run ```segment sigma k min input output```.

The parameters are: (see the paper for details)

- sigma: Used to smooth the input image before segmenting it.
- k: Value for the threshold function.
- min: Minimum component size enforced by post-processing.
- input: Input image.
- output: Output image.

Typical parameters are sigma = 0.5, k = 500, min = 20.
Larger values for k result in larger components in the result.


NOTE ( by Alexander Freytag )  
  - only images with less then std::numeric_limits<int>::max() pixels are supported properly!



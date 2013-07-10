% function [ segImg, noRegions ]= segmentFelzenszwalb(imgInput, sigma, k, minSize, computeColorOutput, destination, verbose)
%
% BRIEF: segmentFelzenszwalb Run the region segmentation algorithm of Felzenszwalb and Huttonlocher
%
%   The function accepts the following options:
%   INPUT: 
%        imgInput           --   (string or 3d-matrix over uint8 )
%        sigma              --   bandwidth of Gaussian kernel to smooth the image (optional, scalar, default: 0.5) 
%        k                  --   influences the threshold accept a boundary between regions, larger k -> larger regions,(optional, scalar, default: 500)
%        minSize            --   minimum component size enforced by post-processing stage (optional, scalar, default: 50)
%        computeColorOutput --   compute colored segm output (rgb) or region indicees (gray)? (optional, logical, default: false) 
%        destination        --   filename where the image shell be stored (optional, string) 
%        verbose            --   print additional information (optional, logical, default: false) 
%
%   OUTPUT: 
%        segImg             --   segmentation result, either colored or with region indicees (optional, 3d-matrix or 2d-matrix over uint8)
%        noRegions          --   total number of regions (optional, uint8)
%
%
%   Example:
%        segResult = segmentFelzenszwalb(imgOrig);
%        segResult = segmentFelzenszwalb(imgOrig,0.5, 200, 20, true, 0 , true);
%


% Authors: Alexander Freytag
% Copyright (C) 2013 Alexander Freytag
% All rights reserved.
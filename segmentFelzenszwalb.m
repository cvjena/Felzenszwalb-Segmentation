% function segImg = segmentFelzenszwalb(imgInput, sigma, k, minSize, destination, verbose)
%
% BRIEF: segmentFelzenszwalb Run the region segmentation algorithm of Felzenszwalb
% and Huttonlocher
%   V = segmentFelzenszwalb(X, N) 
%
%   The function accepts the following options:
%   INPUT: 
%        imgInput         --   (string or 3d-matrix)
%        sigma            --   (optional, scalar) bandwidth of Gaussian kernel to
%        smooth the image, default: 0.5
%        k                --   (optional, scalar) influences the threshold accept a
%        boundary between regions (larger k -> larger regions), default:
%        500
%        minSize          --   (optional, scalar), minimum component size (enforced by post-processing stage), default: 20
%        destination      --   (optional, string) 
%        verbose          --   (optional, bool) 
%   OUTPUT: 
%
%
%   Example::
%
%   Note::


% Authors: Alexander Freytag
% Copyright (C) 2013 Alexander Freytag
% All rights reserved.
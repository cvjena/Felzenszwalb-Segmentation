% demoFelzenszwalbSegmentation
% 
% author: Alexander Freytag
% date:   28-03-2014 (dd-mm-yyyy)
% 
% brief:  small demo showing how to use the felzenszwalb segmentation code
%         via Matlab and how to adjust parameter specifications.


%% ( 1 ) GET INPUT

%read image ... 
img = imread( 'data/test.png');


% show input image in figure
figOrig = figure;
set ( figOrig, 'name', 'Input image');
imshow ( img );


%% ( 2 ) PERFORM SEGMENTATION WITH DEFAULT SETTINGS


% call felzenszwalb segmentation using our mex-interface
segResult = segmentFelzenszwalb( img );

% show segmentation result in separate figure
figSegResult = figure;
set ( figSegResult, 'name', 'Segmentation result');

imshow ( segResult );
% make region colors visually distinguishable
colormap ( 'lines' );

%% ( 3 ) PERFORM SEGMENTATION WITH SPECIFIED SETTINGS

d_sigma              = 0.75;   % default: 0.5
i_k                  = 700;   % default: 500
i_minSize            = 50;    % default: 50
b_computeColorOutput = true;  % default: false
s_destination        = '';    % default: ''
b_verbose            = false; % default: false

srManSpec = segmentFelzenszwalb( img, d_sigma, i_k, i_minSize, ...
                                 b_computeColorOutput, ...
                                 s_destination, ...
                                 b_verbose...
                               );

% show segmentation result in separate figure
figSRManSpec = figure;
set ( figSRManSpec, 'name', 'Segmentation result, man. specified');

imshow ( srManSpec );

% actually, another linespec is not needed, since the output is already a
% nice RGB image. However, for easier visual comparison, we again apply the
% matlab color mapping here.
colormap ( 'lines' );


% wait for user input
pause

% close images
close ( figOrig );
close ( figSegResult );
close ( figSRManSpec );
% Prototyping for FftBuffer JUCE project.
%
% Author: Max Henry
% May 2022.

clc; clear; close all;

addpath(genpath('/Users/maxhenry/Documents/Matlab/eers-audio-toolbox'));
applyPlotSettings();

doPlots = false;

% Simulation parameters.
fpath = "/Users/maxhenry/Documents/Matlab/eers-audio-toolbox/audio/speech_REF.wav";
blockSize = 1024;


[x, sr] = audioread(fpath, [5465, inf]);
x = [x; zeros(mod(length(x), blockSize), 1)];
numBlocks = length(x) / blockSize;

time = (0:(length(x) - 1)) / sr;

figure;
ax = gca;
hold(ax, "on");
plot(ax, time, x, "DisplayName", "in");

pIn = 1;
pOut = pIn + blockSize - 1;

% Plugin parameters.
frameSize = 256;
numOverlap = 4;

myWindow = hamming(frameSize);
myWindow = normalizeForUnityAtOverlap(myWindow, numOverlap);

frameProcesser = @(x) x .* myWindow;

olaBuffer = OlaBuffer(frameSize, numOverlap, frameProcesser);

for b = 1:numBlocks
    block = x(pIn:pOut);

    block = olaBuffer.processBlock(block);

    x(pIn:pOut) = block;

    pIn = pIn + blockSize;
    pOut = pOut + blockSize;
end

plot(ax, time, x, "DisplayName", "out");
xlabel("Time (s)");
ylabel("Amplitude");
legend(ax);

function someWindow = normalizeForUnityAtOverlap(someWindow, numOverlap)
    
    frameSize = length(someWindow);
    tmp = 0; 
    for p = 0:(numOverlap - 1)
        tmp = tmp + someWindow(frameSize / numOverlap * p + 1);
    end

    someWindow = someWindow / tmp;

end
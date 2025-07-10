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
y = x * 0;
numBlocks = length(x) / blockSize;

time = (0:(length(x) - 1)) / sr;

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

    y(pIn:pOut) = block;

    pIn = pIn + blockSize;
    pOut = pOut + blockSize;
end

figure;
ax = gca;
hold(ax, "on");
plot(ax, x(1:end - frameSize), "DisplayName", "in");
plot(ax, y(frameSize + 1:end), "DisplayName", "out");
xlabel("Time (samples)");
ylabel("Amplitude");
legend(ax);

error = x(1:end - frameSize).^2 - y(frameSize + 1:end).^2;
disp(sqrt(mean(error .^2)));

function someWindow = normalizeForUnityAtOverlap(someWindow, numOverlap)
    
    frameSize = length(someWindow);
    tmp = 0; 
    for p = 0:(numOverlap - 1)
        tmp = tmp + someWindow(frameSize / numOverlap * p + 1);
    end

    someWindow = someWindow / tmp;

end
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
hopSize = ceil(frameSize / numOverlap);

buffer = zeros(frameSize, 1);
synthBuffer = zeros(hopSize, 1);

frameBuffers = zeros(frameSize, numOverlap);

myWindow = hamming(frameSize);
myWindow = normalizeForUnityAtOverlap(myWindow, numOverlap);

pBufferWrite = 1;
pSynthRead = 1;
pNewestFrame = 1;

for b = 1:numBlocks
    block = x(pIn:pOut);

    % `processBlock()`
    for n = 1:blockSize

        buffer(pBufferWrite) = block(n);

        if mod(pBufferWrite, hopSize) == 1

            frameBuffers(:, pNewestFrame) = circshift(buffer, -pBufferWrite);
            frameBuffers(:, pNewestFrame) = frameBuffers(:, pNewestFrame) .* myWindow;

            % Do frame processing here before updating newest frame number.

            if doPlots
                figure(2);
                for f = 1:numOverlap
                    subplot(numOverlap, 1, f);
    
    
                    plotColor = 'b';
                    if f == pNewestFrame
                        plotColor = 'r';
                    end
    
                    plot(frameBuffers(:, f), plotColor);
                    ylim([-1, 1]);
                end
            end

            synthBuffer = fillSynthBuffer(frameBuffers, pNewestFrame, numOverlap, hopSize);
            pSynthRead = 1;

            if doPlots
                figure(3);
                plot(synthBuffer);
                ylim([-1, 1]);
                pause();
            end

            % Then advance frame pointer.
            pNewestFrame = mod(pNewestFrame, numOverlap) + 1;
        end

        block(n) = synthBuffer(pSynthRead);

        pBufferWrite = mod(pBufferWrite, frameSize) + 1;
        pSynthRead = pSynthRead + 1;
        
    end

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

function synthBuffer = fillSynthBuffer(frameBuffers, pNewestFrame, numOverlap, hopSize)

    synthBuffer = zeros(hopSize, 1);

    frameOrder = pNewestFrame:-1:(pNewestFrame - numOverlap + 1);
    frameOrder = mod(frameOrder - 1, numOverlap) + 1;

    pIn = 1;
    pOut = hopSize;
    for f = frameOrder
        synthBuffer = synthBuffer + frameBuffers(pIn:pOut, f);

        pIn = pIn + hopSize;
        pOut = pOut + hopSize;
    end

end
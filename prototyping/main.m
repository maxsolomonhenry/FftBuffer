% Prototyping for FftBuffer JUCE project.
%
% Author: Max Henry
% May 2022.

clc; clear; close all;

addpath(genpath('/Users/maxhenry/Documents/Matlab/eers-audio-toolbox'));
applyPlotSettings();

% Simulation parameters.
fpath = "/Users/maxhenry/Documents/Matlab/eers-audio-toolbox/audio/speech_REF.wav";
blockSize = 1024;


[x, sr] = audioread(fpath, [5465, inf]);
x = [x; zeros(mod(length(x), blockSize), 1)];
numBlocks = length(x) / blockSize;

time = (0:(length(x) - 1)) / sr;

% figure;
% ax = gca;
% hold(ax, "on");
% plot(ax, time, x, "DisplayName", "in");

pIn = 1;
pOut = pIn + blockSize - 1;

% Plugin parameters.
frameSize = 2048;
numOverlap = 4;

frames = zeros(frameSize, numOverlap);

hopSize = ceil(frameSize / numOverlap);
myWindow = hamming(frameSize);

pRead = mod((0:-1:-(numOverlap - 1))' * hopSize + 1, frameSize);
pFrame = 1;

figure();
for b = 1:numBlocks
    block = x(pIn:pOut);

    % `processBlock()`
    for n = 1:blockSize

        frames(pRead(1), 1) = block(n);

        for f = 2:numOverlap
            % Read into each frame, and window at the same time.
            frames(pFrame, f) = frames(pRead(f), 1);%   * myWindow(pFrame);
        end

        if pFrame == frameSize

            % Grab overlap part.

            % Synthesis buffer has to factor in somehow.
            for f = 1:numOverlap
                subplot(numOverlap, 1, f);
                plot(frames(:, f));
                hold on;
                plot(pFrame, 0, "*");
                hold off;
                xlabel("Time (samples)");
                ylabel("Amplitude");
                ylim([-.25, .25]);
            end

            pause();
        end

        pRead = mod(pRead, frameSize) + 1;
        pFrame = mod(pFrame, frameSize) + 1;
    end

    x(pIn:pOut) = block;
    pIn = pIn + blockSize;
    pOut = pOut + blockSize;
end

plot(ax, time, x, "DisplayName", "out");
xlabel("Time (s)");
ylabel("Amplitude");
legend(ax);
% Prototyping for FftBuffer JUCE project.
%
% Author: Max Henry
% Mayb 2022.

clc; clear; close all;

% Simulation parameters.
fpath = "/Users/maxhenry/Documents/Matlab/eers-audio-toolbox/audio/speech_REF.wav";
blockSize = 1024;


[x, sr] = audioread(fpath);
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
frameSize = 2048;
numOverlap = 4;

hopSize = ceil(frameSize / numOverlap);
bufferSize = (numOverlap * 2 - 1) * hopSize;
buffer = dsp.AsyncBuffer(bufferSize);

for b = 1:numBlocks
    block = x(pIn:pOut);

    buffer.write(block);

    block = buffer.peek(blockSize);

    x(pIn:pOut) = block;
    pIn = pIn + blockSize;
    pOut = pOut + blockSize;
end

plot(ax, time, x, "DisplayName", "out");
xlabel("Time (s)");
ylabel("Amplitude");
legend(ax);
classdef OlaBuffer < handle

    properties
        frameSize
        numOverlap

        hopSize
        delayBuffer
        addBuffer
        frameBuffers

        frameProcesser

        pDelayBuffer
        pAddBuffer
        pNewestFrame
    end

    methods
        function obj = OlaBuffer(frameSize, numOverlap, frameProcesser)
            obj.frameSize = frameSize;
            obj.numOverlap = numOverlap;
            obj.hopSize = ceil(frameSize / numOverlap);

            obj.frameProcesser = frameProcesser;

            obj.delayBuffer = zeros(frameSize, 1);
            obj.addBuffer = zeros(obj.hopSize, 1);
            obj.frameBuffers = zeros(frameSize, numOverlap);

            obj.pDelayBuffer = 1;
            obj.pAddBuffer = 1;
            obj.pNewestFrame = 1;
        end

        function block = processBlock(obj, block)

            numSamples = length(block);

            for n = 1:numSamples
        
                block(n) = obj.process(block(n));
                
            end
        end

        function x = process(obj, x)
            obj.delayBuffer(obj.pDelayBuffer) = x;
    
            if mod(obj.pDelayBuffer, obj.hopSize) == 1
    
                obj.frameBuffers(:, obj.pNewestFrame) = obj.fillNewestFrameFromDelayBuffer();

                obj.frameBuffers(:, obj.pNewestFrame) = obj.frameProcesser(obj.frameBuffers(:, obj.pNewestFrame));

                obj.fillAddBuffer();
                obj.pNewestFrame = mod(obj.pNewestFrame, obj.numOverlap) + 1;
            end
    
            x = obj.addBuffer(obj.pAddBuffer);
    
            obj.pDelayBuffer = mod(obj.pDelayBuffer, obj.frameSize) + 1;
            obj.pAddBuffer = mod(obj.pAddBuffer, obj.hopSize) + 1;
        end

        function obj = fillAddBuffer(obj)

            obj.addBuffer = obj.addBuffer * 0;
        
            frameOrder = obj.pNewestFrame:-1:(obj.pNewestFrame - obj.numOverlap + 1);
            frameOrder = mod(frameOrder - 1, obj.numOverlap) + 1;
        
            pIn = 1;
            pOut = obj.hopSize;

            for f = frameOrder
                obj.addBuffer = obj.addBuffer + obj.frameBuffers(pIn:pOut, f);
        
                pIn = pIn + obj.hopSize;
                pOut = pOut + obj.hopSize;
            end
        end

        function newestFrame = fillNewestFrameFromDelayBuffer(obj)
            
            newestFrame = zeros(obj.frameSize, 1);

            pRead = obj.pDelayBuffer;

            for n = 1:obj.frameSize
                newestFrame(n) = obj.delayBuffer(pRead);
                pRead = mod(pRead, obj.frameSize) + 1;
            end
        end
    end
    
end
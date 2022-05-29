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

        function block = process(obj, block)

            numSamples = length(block);

            for n = 1:numSamples
        
                obj.delayBuffer(obj.pDelayBuffer) = block(n);
        
                if mod(obj.pDelayBuffer, obj.hopSize) == 1
        
                    newestFrame = circshift(obj.delayBuffer, - obj.pDelayBuffer);
                    obj.frameBuffers(:, obj.pNewestFrame) = obj.frameProcesser(newestFrame);

                    obj.fillAddBuffer();
                    obj.pNewestFrame = mod(obj.pNewestFrame, obj.numOverlap) + 1;
                end
        
                block(n) = obj.addBuffer(obj.pAddBuffer);
        
                obj.pDelayBuffer = mod(obj.pDelayBuffer, obj.frameSize) + 1;
                obj.pAddBuffer = mod(obj.pAddBuffer, obj.hopSize) + 1;
                
            end
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
    end
    
end
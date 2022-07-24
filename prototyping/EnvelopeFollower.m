classdef EnvelopeFollower < audioPlugin

    properties
        lpFilter;
        curCutoffHz;
        curQ;
        curGainDb;

        linearGain;
    end

    properties
        cutoffHz = 100;
        q = 1;
        gainDb = 0;
        depth = 0;
    end

    properties (Constant)
        PluginInterface = audioPluginInterface( ...
            'PluginName', 'EnvelopeFollower', ...
            'VendorName', 'Pass by Reference', ...
            'VendorVersion', '0.0.0', ...
            'UniqueId', '2109', ...
            'InputChannels', 1, ...
            'OutputChannels', 1 ,...
            audioPluginParameter('cutoffHz', 'DisplayName', 'Cutoff (Hz)', "Mapping", {'lin', 5, 200}),...
            audioPluginParameter('depth', 'DisplayName', 'Depth', "Mapping", {'lin', 0, 1}),...
            audioPluginParameter('q', 'DisplayName', 'Q', "Mapping", {'log', 0.0001, 10}),...
            audioPluginParameter('gainDb', 'DisplayName', 'Gain (dB)', "Mapping", {'lin', 0, 50})...
        );
    end

    properties (Constant)
        ENV_OFFSET = 0.9;
    end

    methods
        function obj = EnvelopeFollower

            [b, a] = obj.getLpFilterCoefficients();
            obj.lpFilter = dsp.IIRFilter('Numerator', b, 'Denominator', a);

            obj.curCutoffHz = nan;
            obj.curQ = nan;
            obj.curGainDb = nan;

            obj.linearGain = 1;

        end

        function [b, a] = getLpFilterCoefficients(obj)

            w0 = 2 * pi * obj.cutoffHz / obj.getSampleRate();
            alpha = sin(w0) / (2 * obj.q);

            b0 = (1 - cos(w0)) / 2;
            b1 = 1 - cos(w0);
            b2 = (1 - cos(w0)) / 2;

            a0 = 1 + alpha;
            a1 = -2 * cos(w0);
            a2 = 1 - alpha;

            b = [b0 b1 b2];
            a = [a0 a1 a2];

            a = a ./ a(1);

        end

        function x = process(obj, x)

            obj.updateFilter();
            obj.updateGain();

            x = abs(x);
            x = obj.lpFilter(x);
            x = x * obj.linearGain;
            x = obj.depth * (x - obj.ENV_OFFSET) + obj.ENV_OFFSET;
            x = min(x, 1);

        end

        function obj = updateFilter(obj)
 
            isNewCutoff = (obj.cutoffHz ~= obj.curCutoffHz);
            isNewQ = (obj.q ~= obj.curQ);

            if isNewCutoff
                obj.curCutoffHz = obj.cutoffHz;
            end

            if isNewQ
                obj.curQ = obj.q;
            end

            if (isNewCutoff || isNewQ)
                [b, a] = obj.getLpFilterCoefficients();
                obj.lpFilter.Numerator = b;
                obj.lpFilter.Denominator = a;

                tmp = phasedelay(obj.lpFilter);
                val = max(tmp);

                fprintf("\nMax phase delay: %d samples\n", val);
            end

        end

        function obj = updateGain(obj)

            isNewGainDb = (obj.gainDb ~= obj.curGainDb);

            if isNewGainDb
                obj.curGainDb = obj.gainDb;
                obj.linearGain = db2mag(obj.gainDb);
            end

        end

    end

end
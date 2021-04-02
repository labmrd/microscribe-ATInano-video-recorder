%% 
ATI_CalibSerialNumber = 'FT28389';
ATI_CalibPartNumber = 'SI-80-4';
ATI_CalibFamilyId = 'Net ';
ATI_CalibTime = '2019-07-18 04:00:00Z';
ATI_BasicMatrix = [
	   28.0250	   -3.3050	 -132.0016	 3668.2085	  144.7000	-3826.2173	
	  165.1710	-4380.0811	  -48.9427	 2111.3879	 -148.6830	 2217.8435	
	 6371.4810	  102.2830	 6321.1084	   62.0294	 6331.8882	   42.0717	
	    1.0174	  -24.1759	   90.3032	   12.4314	  -92.8633	   11.6713	
	 -102.7653	   -1.4344	   52.2929	  -19.8079	   50.8704	   21.4821	
	    1.2302	  -53.2961	    2.1848	  -50.9759	    3.1111	  -53.2863	
                  ];             
ATI_ForceUnitsCode = 2;
ATI_TorqueUnitsCode = 3;
ATI_ForceUnits = 'Newtons';
ATI_TorqueUnits = 'Newton-meters';
ATI_MaxRating = [    80.0	80.0	240.0	 4.0	 4.0	 4.0	 ];
ATI_CountsPerForce  = 1000000
ATI_CountsPerTorque = 1000000
ATI_GageGain = [    505	523	527	535	529	533	 ] 
ATI_GageOffset = [    31122	29141	33345	33078	28831	31609	 ] 

%%


GC6x6 = repmat(ATI_GageGain, 6, 1); % only need this if you are using your own amplifier?
runtimeMatrix = ATI_BasicMatrix .* (GC6x6.^-1) % element-wise computation
% see 9610-05-1030.pdf

c6x1 = [ ones(3,1)*ATI_CountsPerForce ; ones(3,1)*ATI_CountsPerTorque];

runtimeMatrix = ATI_BasicMatrix;  % Typical use, but NETcanOEM needs it's gage corrected


((runtimeMatrix * ATI_GageOffset.').* c6x1.^-1).'

[(diag(c6x1.^-1)*(runtimeMatrix ))* ATI_GageOffset.'].'




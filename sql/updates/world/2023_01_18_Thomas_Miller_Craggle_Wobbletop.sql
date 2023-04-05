-- Thomas Miller
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 3518;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 3518;
INSERT INTO `smart_scripts` (entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,event_param1,event_param2,event_param3,event_param4,action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,target_type,target_param1,target_param2,target_param3,target_x,target_y,target_z,target_o,comment) VALUES
	 (3518,0,0,0,1,0,100,0,5000,15000,30000,60000,1,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'OOC - Random Speech'),
	 (3518,0,100,0,22,0,100,0,101,2000,2000,0,5,3,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Wave Emote - Wave'),
	 (3518,0,101,0,22,0,100,0,41,2000,2000,0,5,11,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Flex Emote - Laugh'),
	 (3518,0,102,0,22,0,100,0,39,2000,2000,0,5,11,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Fart Emote - Laugh'),
	 (3518,0,103,0,22,0,100,0,5,2000,2000,0,5,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Applaud Emote - Bow'),
	 (3518,0,104,0,22,0,100,0,24,2000,2000,0,5,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Clap Emote - Bow'),
	 (3518,0,105,0,22,0,100,0,21,2000,2000,0,5,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Cheer Emote - Bow'),
	 (3518,0,106,0,22,0,100,0,33,2000,2000,0,5,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Curtsey Emote - Bow'),
	 (3518,0,107,0,22,0,100,0,26,2000,2000,0,5,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Congrat Emote - Bow'),
	 (3518,0,108,0,22,0,100,0,8,2000,2000,0,5,274,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Beg Emote - ''No'''),
	 (3518,0,109,0,22,0,100,0,31,2000,2000,0,5,274,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Cry Emote - ''No'''),
	 (3518,0,110,0,22,0,100,0,35,2000,2000,0,5,273,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Drink Emote - ''Yes'''),
	 (3518,0,111,0,22,0,100,0,28,2000,2000,0,5,23,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Cower Emote - Flex'),
	 (3518,0,112,0,22,0,100,0,60,2000,2000,0,5,6,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Laugh Emote - Question'),
	 (3518,0,113,0,22,0,100,0,75,2000,2000,0,5,15,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Roar Emote - Roar'),
	 (3518,0,114,0,22,0,100,0,98,2000,2000,0,5,274,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Threaten Emote - ''No'''),
	 (3518,0,115,0,22,0,100,0,100,2000,2000,0,5,23,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Victory Emote - Flex'),
	 (3518,0,116,0,22,0,100,0,80,2000,2000,0,5,94,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Sexy Emote - Dance'),
	 (3518,0,117,0,22,0,100,0,104,2000,2000,0,5,23,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Receive Whistle Emote - Flex'),
	 (3518,0,122,123,22,0,30,0,89,2000,2000,0,11,58519,0,0,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,'Receive Spit Emote - Chance to Cast Spit on invoker'),
	 (3518,0,123,0,61,0,100,0,0,0,0,0,5,274,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Link - ''No'' Emote'),
	 (3518,0,124,125,22,0,30,0,77,2000,2000,0,11,58511,0,0,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,'Receive Rude Emote - Chance to Cast Rotten Apple on invoker'),
	 (3518,0,125,0,61,0,100,0,0,0,0,0,5,274,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Link - ''No'' Emote'),
	 (3518,0,126,127,22,0,30,0,68,2000,2000,0,11,58514,0,0,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,'Receive Nosepick Emote - Chance to Cast Rotten Banana on invoker'),
	 (3518,0,127,0,61,0,100,0,0,0,0,0,5,274,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,'Link - ''No'' Emote');



DELETE FROM `creature_text` WHERE  `Entry`=3518;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `MinTimer`, `MaxTimer`, `SpellID`, `comment`) VALUES
	(3518, 0, 1, 'Warm, wholesome bread!', 12, 7, 100, 1, 0, 0, 1102, 0, 0, 0, 'Thomas Miller'),
	(3518, 0, 2, 'Rolls, buns and bread. Baked fresh!', 12, 7, 100, 1, 0, 0, 1103, 0, 0, 0, 'Thomas Miller'),
	(3518, 0, 0, 'Fresh bread for sale!', 12, 7, 100, 1, 0, 0, 1101, 0, 0, 0, 'Thomas Miller'),
	(3518, 0, 3, 'Freshly baked bread for sale!', 12, 7, 100, 1, 0, 0, 1104, 0, 0, 0, 'Thomas Miller');

DELETE FROM `creature_template_addon` WHERE `entry` = 3518;
DELETE FROM `creature_addon` WHERE `guid` = 265278;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`) VALUES
(265278, 265278, 257);

UPDATE `creature` SET `position_x` = -8831.28, `position_y` = 543.109, `position_z` = 96.8344 WHERE `guid` = 265278 AND `id` = 3518;

DELETE FROM `waypoint_data` WHERE  `id`=265278;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `delay_chance`, `move_flag`, `speed`, `action`, `action_chance`, `entry`, `wpguid`) VALUES
	(265278, 1, -8842.27, 606.085, 93.3415, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 2, -8851.58, 594.931, 92.8328, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 3, -8864.37, 582.372, 93.6229, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 4, -8873.49, 577.604, 93.3801, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 5, -8879.78, 577.347, 93.2289, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 6, -8882.37, 578.591, 93.218, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 7, -8884.06, 580.406, 93.2953, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 8, -8885.39, 583.307, 93.3714, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 9, -8884.77, 585.403, 93.4686, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 10, -8883.13, 587.764, 93.4279, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 11, -8879.62, 591.656, 93.3581, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 12, -8869.2, 599.175, 92.8157, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 13, -8852.05, 608.478, 92.5813, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 14, -8841.98, 614.679, 92.9274, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 15, -8833.34, 618.443, 93.7431, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 16, -8826.95, 617.802, 94.4797, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 17, -8824.15, 615.517, 94.9018, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 18, -8813.75, 605.036, 96.2085, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 19, -8809.69, 597.997, 97.0273, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 20, -8810.16, 594.284, 97.2561, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 21, -8812.03, 590.153, 96.8642, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 22, -8820.47, 575.263, 94.4812, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 23, -8836.27, 552.262, 96.0309, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 24, -8837.88, 548.383, 96.5818, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 25, -8837.35, 546.198, 96.7868, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 26, -8835.74, 543.846, 96.9846, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 27, -8834.08, 542.988, 96.9912, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 28, -8832.2, 542.848, 96.9385, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 29, -8830.36, 543.621, 96.8256, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 30, -8827.41, 545.508, 96.6034, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 31, -8825.46, 548.397, 96.0672, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 32, -8814.24, 566.621, 94.2511, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 33, -8803.42, 581.315, 96.6169, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 34, -8774.57, 603.167, 97.3016, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 35, -8763.01, 616.13, 98.8001, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 36, -8758.9, 623.576, 100.98, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 37, -8757.63, 629.511, 102.453, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 38, -8758.09, 640.462, 103.63, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 39, -8762.19, 650.334, 103.841, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 40, -8772.63, 666.086, 103.354, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 41, -8783.3, 678.377, 102.914, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 42, -8789.13, 682.305, 102.094, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 43, -8797.36, 685.5, 101.59, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 44, -8803.89, 685.765, 101.253, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 45, -8818.28, 682.102, 98.0272, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 46, -8832.85, 675.343, 98.459, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 47, -8845.6, 666.639, 97.7289, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 48, -8857.13, 658.219, 96.6525, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 49, -8865.33, 652.728, 96.1823, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 50, -8868.19, 648.39, 96.1479, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 51, -8868.15, 644.925, 96.0672, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 52, -8867.03, 641.344, 96.1197, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 53, -8863.27, 640.802, 96.1972, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 54, -8858.57, 641.597, 96.4074, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 55, -8852.44, 642.624, 96.5362, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 56, -8848.19, 640.617, 96.2506, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 57, -8844.33, 636.377, 95.4882, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 58, -8840.76, 634.894, 95.1817, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 59, -8834.39, 634.577, 94.7181, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 60, -8823.97, 637.852, 94.6053, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 61, -8816.05, 640.071, 94.6107, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 62, -8804.73, 641.086, 94.5386, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 63, -8798.78, 640.894, 94.441, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 64, -8795.51, 639.669, 94.3688, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 65, -8793.47, 637.61, 94.3779, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 66, -8793.23, 635.057, 94.4069, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 67, -8794.84, 631.64, 94.5442, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 68, -8797.51, 630.124, 94.6795, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 69, -8800.78, 629.598, 94.4297, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 70, -8806.17, 628.144, 94.4331, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 71, -8813.87, 624.728, 94.3272, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 73, -8822.25, 620.652, 94.4861, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 74, -8829.77, 617.517, 94.1007, 0, 0, 10, 0, 0, 0, 100, 0, 0),
	(265278, 75, -8834.51, 615.057, 93.6896, 0, 0, 10, 0, 0, 0, 100, 0, 0);



-- Craggle Wobbletop
UPDATE `creature_template` SET `VehicleId` = 1597 WHERE `entry` = 52358;
UPDATE `creature_template` SET `VehicleId` = 1595 WHERE `entry` = 52359;
UPDATE `creature` SET `position_x` = -8742.11, `position_y` = 565.326, `position_z` = 97.4465, `orientation` = 3.93523, `spawndist` = 0, `MovementType` = 2 WHERE `guid` = 265219 AND `id` = 52358;

DELETE FROM `creature_template_addon` WHERE `entry` = 52358;
DELETE FROM `creature_addon` WHERE `guid` = 265219;
INSERT INTO `creature_addon` (`guid`, `path_id`, `emote`) VALUES
(265219, 265219, 379);

DELETE FROM `waypoint_data` WHERE `id` = 265219;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(265219, 1, -8742.11, 565.326, 97.4465, 0, 0, 0, 0, 100, 0, 52358),
(265219, 2, -8724.28, 581.938, 98.0096, 0, 0, 0, 0, 100, 0, 52358),
(265219, 3, -8706.55, 599.998, 99.2008, 0, 0, 0, 0, 100, 0, 52358),
(265219, 4, -8701.12, 615.582, 99.9045, 0, 0, 0, 0, 100, 0, 52358),
(265219, 5, -8702.06, 636.189, 100.263, 0, 0, 0, 0, 100, 0, 52358),
(265219, 6, -8706.83, 653.502, 99.8812, 0, 0, 0, 0, 100, 0, 52358),
(265219, 7, -8716.93, 673.677, 98.9862, 0, 0, 0, 0, 100, 0, 52358),
(265219, 8, -8731.68, 694.602, 99.026, 0, 0, 0, 0, 100, 0, 52358),
(265219, 9, -8744.11, 710.651, 98.6385, 0, 0, 0, 0, 100, 0, 52358),
(265219, 10, -8756.38, 726.195, 98.4693, 0, 0, 0, 0, 100, 0, 52358),
(265219, 11, -8772.68, 740.035, 99.5238, 0, 0, 0, 0, 100, 0, 52358),
(265219, 12, -8784.97, 744.403, 99.008, 0, 0, 0, 0, 100, 0, 52358),
(265219, 13, -8806.59, 741.773, 97.7635, 0, 0, 0, 0, 100, 0, 52358),
(265219, 14, -8831.72, 728.413, 98.3643, 0, 0, 0, 0, 100, 0, 52358),
(265219, 15, -8850.83, 719.234, 97.5135, 0, 0, 0, 0, 100, 0, 52358),
(265219, 16, -8874.03, 709.74, 97.9275, 0, 0, 0, 0, 100, 0, 52358),
(265219, 17, -8888.76, 701.622, 98.5337, 0, 0, 0, 0, 100, 0, 52358),
(265219, 18, -8897.93, 696.422, 98.9529, 0, 0, 0, 0, 100, 0, 52358),
(265219, 19, -8888.76, 701.622, 98.5337, 0, 0, 0, 0, 100, 0, 52358),
(265219, 20, -8874.03, 709.74, 97.9275, 0, 0, 0, 0, 100, 0, 52358),
(265219, 21, -8850.83, 719.234, 97.5135, 0, 0, 0, 0, 100, 0, 52358),
(265219, 22, -8831.72, 728.413, 98.3643, 0, 0, 0, 0, 100, 0, 52358),
(265219, 23, -8806.59, 741.773, 97.7635, 0, 0, 0, 0, 100, 0, 52358),
(265219, 24, -8784.97, 744.403, 99.008, 0, 0, 0, 0, 100, 0, 52358),
(265219, 25, -8772.68, 740.035, 99.5238, 0, 0, 0, 0, 100, 0, 52358),
(265219, 26, -8756.38, 726.195, 98.4693, 0, 0, 0, 0, 100, 0, 52358),
(265219, 27, -8744.11, 710.651, 98.6385, 0, 0, 0, 0, 100, 0, 52358),
(265219, 28, -8731.68, 694.602, 99.026, 0, 0, 0, 0, 100, 0, 52358),
(265219, 29, -8716.93, 673.677, 98.9862, 0, 0, 0, 0, 100, 0, 52358),
(265219, 30, -8706.83, 653.502, 99.8812, 0, 0, 0, 0, 100, 0, 52358),
(265219, 31, -8702.06, 636.189, 100.263, 0, 0, 0, 0, 100, 0, 52358),
(265219, 32, -8701.12, 615.582, 99.9045, 0, 0, 0, 0, 100, 0, 52358),
(265219, 33, -8706.55, 599.998, 99.2008, 0, 0, 0, 0, 100, 0, 52358),
(265219, 34, -8724.08, 582.176, 97.8885, 0, 0, 0, 0, 100, 0, 52358);

DELETE FROM `vehicle_template_accessory` WHERE `EntryOrAura` = 52358 AND `accessory_entry` = 52359;
DELETE FROM `vehicle_template_accessory` WHERE `EntryOrAura` = 52359 AND `accessory_entry` = 52361;
INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(52358, 52359, 0, 1, "Craggle Wobbletop - Toy Cart", 8, 0),
(52359, 52361, 0, 1, "Toy Cart - Toy Cart Bunny", 8, 0);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (52358, 52359);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(52358, 46598, 0, 0),
(52359, 46598, 0, 0);
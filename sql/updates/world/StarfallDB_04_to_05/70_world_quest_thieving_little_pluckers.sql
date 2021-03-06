﻿DELETE FROM `smart_scripts` WHERE (`entryorguid`IN (48040, 48041, 48043) AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(48041, 0, 0, 0, 1, 0, 100, 0, 0, 0, 0, 0, 11, 46598, 0, 0, 0, 0, 0, 11, 48040, 2, 0, 0, 0, 0, 0, 'enter - vehicle on spawn'),
(48043, 0, 0, 0, 1, 0, 100, 0, 0, 0, 0, 0, 11, 46598, 0, 0, 0, 0, 0, 11, 48041, 2, 0, 0, 0, 0, 0, 'enter - vehicle on spawn');

UPDATE creature_template SET AIName = 'SmartAI' WHERE entry IN (48040, 48041, 48043);
DELETE FROM creature_template_addon WHERE entry IN (48040, 48041, 48043);

REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES

(89445, 'spell_uldum_hammer');

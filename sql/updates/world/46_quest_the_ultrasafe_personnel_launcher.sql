UPDATE `npc_spellclick_spells` SET `spell_id`=77393 WHERE `npc_entry`=41327 AND `spell_id`=59099;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = '41398';
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`) VALUES
(41398, 77393, 1);
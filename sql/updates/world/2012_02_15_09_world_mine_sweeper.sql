DELETE FROM disables WHERE sourceType=4 AND entry=5258;
DELETE FROM achievement_criteria_data WHERE criteria_id=5258;
INSERT INTO achievement_criteria_data VALUES (5258,11,0,0,'achievement_mine_sweeper');

UPDATE creature_template SET flags_extra=128, ScriptName='npc_land_mine_bunny' WHERE entry=29397;

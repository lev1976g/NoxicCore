ALTER TABLE `playercreateinfo`
	ADD COLUMN `orientation` FLOAT NOT NULL DEFAULT '0' AFTER `positionZ`;

UPDATE `world_db_version` SET `LastUpdate` = '2014-07-18_playercreateinfo';
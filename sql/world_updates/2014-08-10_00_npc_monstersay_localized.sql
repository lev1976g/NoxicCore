-- Thanks to Matthias for implement

-- ----------------------------
-- Table structure for npc_monstersay_localized
-- ----------------------------
DROP TABLE IF EXISTS `npc_monstersay_localized`;
CREATE TABLE `npc_monstersay_localized` (
  `entry` int(10) NOT NULL DEFAULT '0',
  `language_code` varchar(5) NOT NULL DEFAULT '0',
  `monstername` longtext NOT NULL COLLATE 'utf8_unicode_ci',
  `text0` longtext NOT NULL COLLATE 'utf8_unicode_ci',
  `text1` longtext NOT NULL COLLATE 'utf8_unicode_ci',
  `text2` longtext NOT NULL COLLATE 'utf8_unicode_ci',
  `text3` longtext NOT NULL COLLATE 'utf8_unicode_ci',
  `text4` longtext NOT NULL COLLATE 'utf8_unicode_ci',
  PRIMARY KEY (`entry`,`language_code`)
)
COMMENT='NPC System localized'
COLLATE='utf8_unicode_ci'
ENGINE=MyISAM;

UPDATE `world_db_version` SET `LastUpdate` = '2014-08-10_00_npc_monstersay_localized' WHERE `LastUpdate` = '2014-08-05_01_worldstring';
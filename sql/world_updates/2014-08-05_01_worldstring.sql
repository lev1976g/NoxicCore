DELETE FROM `worldstring_tables` WHERE `entry` between 85 and 89;
INSERT INTO `worldstring_tables` (`text`) VALUES 
('Trick or Treat!'),
('What can I do at an inn?'),
('You must be at least level %u to buy Arena charter');

UPDATE `world_db_version` SET `LastUpdate` = '2014-08-05_01_worldstring' WHERE `LastUpdate` = '2014-08-05_00_worldstring';
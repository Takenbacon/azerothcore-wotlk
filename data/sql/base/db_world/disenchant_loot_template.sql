-- MySQL dump 10.13  Distrib 8.4.3, for Win64 (x86_64)
--
-- Host: localhost    Database: acore_world
-- ------------------------------------------------------
-- Server version	8.4.3

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `disenchant_loot_template`
--

DROP TABLE IF EXISTS `disenchant_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `disenchant_loot_template` (
  `Entry` int unsigned NOT NULL DEFAULT '0',
  `Item` int unsigned NOT NULL DEFAULT '0',
  `Reference` int NOT NULL DEFAULT '0',
  `Chance` float NOT NULL DEFAULT '100',
  `QuestRequired` tinyint NOT NULL DEFAULT '0',
  `LootMode` smallint unsigned NOT NULL DEFAULT '1',
  `GroupId` tinyint unsigned NOT NULL DEFAULT '0',
  `MinCount` tinyint unsigned NOT NULL DEFAULT '1',
  `MaxCount` tinyint unsigned NOT NULL DEFAULT '1',
  `Comment` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`Entry`,`Item`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `disenchant_loot_template`
--

LOCK TABLES `disenchant_loot_template` WRITE;
/*!40000 ALTER TABLE `disenchant_loot_template` DISABLE KEYS */;
INSERT INTO `disenchant_loot_template` VALUES
(1,10938,0,0,0,1,1,1,2,'Lesser Magic Essence'),
(1,10940,0,80,0,1,1,1,2,'Strange Dust'),
(2,10939,0,20,0,1,1,1,2,'Greater Magic Essence'),
(2,10940,0,75,0,1,1,2,3,'Strange Dust'),
(2,10978,0,0,0,1,1,1,1,'Small Glimmering Shard'),
(3,10940,0,75,0,1,1,4,6,'Strange Dust'),
(3,10978,0,0,0,1,1,1,1,'Small Glimmering Shard'),
(3,10998,0,15,0,1,1,1,2,'Lesser Astral Essence'),
(4,11082,0,20,0,1,1,1,2,'Greater Astral Essence'),
(4,11083,0,75,0,1,1,1,2,'Soul Dust'),
(4,11084,0,0,0,1,1,1,1,'Large Glimmering Shard'),
(5,11083,0,75,0,1,1,2,5,'Soul Dust'),
(5,11134,0,20,0,1,1,1,2,'Lesser Mystic Essence'),
(5,11138,0,0,0,1,1,1,1,'Small Glowing Shard'),
(6,11135,0,20,0,1,1,1,2,'Greater Mystic Essence'),
(6,11137,0,75,0,1,1,1,2,'Vision Dust'),
(6,11139,0,0,0,1,1,1,1,'Large Glowing Shard'),
(7,11137,0,75,0,1,1,2,5,'Vision Dust'),
(7,11174,0,20,0,1,1,1,2,'Lesser Nether Essence'),
(7,11177,0,0,0,1,1,1,1,'Small Radiant Shard'),
(8,11175,0,20,0,1,1,1,2,'Greater Nether Essence'),
(8,11176,0,75,0,1,1,1,2,'Dream Dust'),
(8,11178,0,0,0,1,1,1,1,'Large Radiant Shard'),
(9,11176,0,75,0,1,1,2,5,'Dream Dust'),
(9,14343,0,0,0,1,1,1,1,'Small Brilliant Shard'),
(9,16202,0,20,0,1,1,1,2,'Lesser Eternal Essence'),
(10,14344,0,0,0,1,1,1,1,'Large Brilliant Shard'),
(10,16203,0,20,0,1,1,1,2,'Greater Eternal Essence'),
(10,16204,0,75,0,1,1,1,2,'Illusion Dust'),
(11,14344,0,0,0,1,1,1,1,'Large Brilliant Shard'),
(11,16203,0,20,0,1,1,2,3,'Greater Eternal Essence'),
(11,16204,0,75,0,1,1,2,5,'Illusion Dust'),
(12,22445,0,75,0,1,1,1,3,'Arcane Dust'),
(12,22447,0,22,0,1,1,1,3,'Lesser Planar Essence'),
(12,22448,0,0,0,1,1,1,1,'Small Prismatic Shard'),
(13,22445,0,75,0,1,1,2,3,'Arcane Dust'),
(13,22447,0,22,0,1,1,2,3,'Lesser Planar Essence'),
(13,22448,0,0,0,1,1,1,1,'Small Prismatic Shard'),
(14,22445,0,75,0,1,1,2,5,'Arcane Dust'),
(14,22446,0,22,0,1,1,1,2,'Greater Planar Essence'),
(14,22449,0,0,0,1,1,1,1,'Large Prismatic Shard'),
(15,34053,0,0,0,1,1,1,1,'Small Dream Shard'),
(15,34054,0,75,0,1,1,1,2,'Infinite Dust'),
(15,34056,0,22,0,1,1,1,2,'Lesser Cosmic Essence'),
(16,34052,0,0,0,1,1,1,1,'Dream Shard'),
(16,34054,0,75,0,1,1,2,5,'Infinite Dust'),
(16,34055,0,22,0,1,1,1,2,'Greater Cosmic Essence'),
(17,11139,0,100,0,1,0,2,4,'Large Glowing Shard'),
(18,14344,0,75,0,1,0,1,1,'Large Brilliant Shard'),
(21,10938,0,80,0,1,1,1,2,'Lesser Magic Essence'),
(21,10940,0,0,0,1,1,1,2,'Strange Dust'),
(22,10939,0,75,0,1,1,1,2,'Greater Magic Essence'),
(22,10940,0,20,0,1,1,2,3,'Strange Dust'),
(22,10978,0,0,0,1,1,1,1,'Small Glimmering Shard'),
(23,10940,0,15,0,1,1,4,6,'Strange Dust'),
(23,10978,0,0,0,1,1,1,1,'Small Glimmering Shard'),
(23,10998,0,75,0,1,1,1,2,'Lesser Astral Essence'),
(24,11082,0,75,0,1,1,1,2,'Greater Astral Essence'),
(24,11083,0,20,0,1,1,1,2,'Soul Dust'),
(24,11084,0,0,0,1,1,1,1,'Large Glimmering Shard'),
(25,11083,0,20,0,1,1,2,5,'Soul Dust'),
(25,11134,0,75,0,1,1,1,2,'Lesser Mystic Essence'),
(25,11138,0,0,0,1,1,1,1,'Small Glowing Shard'),
(26,11135,0,75,0,1,1,1,2,'Greater Mystic Essence'),
(26,11137,0,20,0,1,1,1,2,'Vision Dust'),
(26,11139,0,0,0,1,1,1,1,'Large Glowing Shard'),
(27,11137,0,20,0,1,1,2,5,'Vision Dust'),
(27,11174,0,75,0,1,1,1,2,'Lesser Nether Essence'),
(27,11177,0,0,0,1,1,1,1,'Small Radiant Shard'),
(28,11175,0,75,0,1,1,1,2,'Greater Nether Essence'),
(28,11176,0,20,0,1,1,1,2,'Dream Dust'),
(28,11178,0,0,0,1,1,1,1,'Large Radiant Shard'),
(29,11176,0,22,0,1,1,2,5,'Dream Dust'),
(29,14343,0,0,0,1,1,1,1,'Small Brilliant Shard'),
(29,16202,0,75,0,1,1,1,2,'Lesser Eternal Essence'),
(30,14344,0,0,0,1,1,1,1,'Large Brilliant Shard'),
(30,16203,0,75,0,1,1,1,2,'Greater Eternal Essence'),
(30,16204,0,22,0,1,1,1,2,'Illusion Dust'),
(31,14344,0,0,0,1,1,1,1,'Large Brilliant Shard'),
(31,16203,0,75,0,1,1,2,3,'Greater Eternal Essence'),
(31,16204,0,22,0,1,1,2,5,'Illusion Dust'),
(32,22445,0,22,0,1,1,2,3,'Arcane Dust'),
(32,22447,0,75,0,1,1,2,3,'Lesser Planar Essence'),
(32,22448,0,0,0,1,1,1,1,'Small Prismatic Shard'),
(33,22445,0,22,0,1,1,2,5,'Arcane Dust'),
(33,22446,0,75,0,1,1,1,2,'Greater Planar Essence'),
(33,22449,0,0,0,1,1,1,1,'Large Prismatic Shard'),
(34,34053,0,0,0,1,1,1,1,'Small Dream Shard'),
(34,34054,0,22,0,1,1,1,2,'Infinite Dust'),
(34,34056,0,75,0,1,1,1,2,'Lesser Cosmic Essence'),
(35,34052,0,0,0,1,1,1,1,'Dream Shard'),
(35,34054,0,22,0,1,1,2,5,'Infinite Dust'),
(35,34055,0,75,0,1,1,1,2,'Greater Cosmic Essence'),
(41,10978,0,100,0,1,0,1,1,'Small Glimmering Shard'),
(42,11084,0,100,0,1,0,1,1,'Large Glimmering Shard'),
(43,11138,0,100,0,1,0,1,1,'Small Glowing Shard'),
(44,11139,0,100,0,1,0,1,1,'Large Glowing Shard'),
(45,11177,0,100,0,1,0,1,1,'Small Radiant Shard'),
(46,11178,0,100,0,1,0,1,1,'Large Radiant Shard'),
(47,14343,0,100,0,1,0,1,1,'Small Brilliant Shard'),
(48,14344,0,99.5,0,1,1,1,1,'Large Brilliant Shard'),
(48,20725,0,0,0,1,1,1,1,'Nexus Crystal'),
(49,14344,0,99.5,0,1,1,1,1,'Large Brilliant Shard'),
(49,20725,0,0,0,1,1,1,1,'Nexus Crystal'),
(50,20725,0,0,0,1,1,1,1,'Nexus Crystal'),
(50,22448,0,99.5,0,1,1,1,1,'Small Prismatic Shard'),
(51,20725,0,0,0,1,1,1,1,'Nexus Crystal'),
(51,22448,0,99.5,0,1,1,1,1,'Small Prismatic Shard'),
(52,22449,0,99.5,0,1,1,1,1,'Large Prismatic Shard'),
(52,22450,0,0,0,1,1,1,1,'Void Crystal'),
(53,34053,0,99.5,0,1,1,1,1,'Small Dream Shard'),
(53,34057,0,0,0,1,1,1,1,'Abyss Crystal'),
(54,34052,0,99.5,0,1,1,1,1,'Dream Shard'),
(54,34057,0,0,0,1,1,1,1,'Abyss Crystal'),
(61,11177,0,100,0,1,0,2,4,'Small Radiant Shard'),
(62,11178,0,100,0,1,0,2,4,'Large Radiant Shard'),
(63,14343,0,100,0,1,0,2,4,'Small Brilliant Shard'),
(64,20725,0,100,0,1,0,1,1,'Nexus Crystal'),
(65,20725,0,100,0,1,0,1,2,'Nexus Crystal'),
(66,22450,0,100,0,1,0,1,2,'Void Crystal'),
(67,22450,0,100,0,1,0,1,1,'Void Crystal'),
(67,44012,44012,67,0,1,1,1,1,'(ReferenceTable)'),
(68,34057,0,100,0,1,0,1,1,'Abyss Crystal'),
(69,34057,0,100,0,1,0,1,2,'Abyss Crystal');
/*!40000 ALTER TABLE `disenchant_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-07-19 12:08:53

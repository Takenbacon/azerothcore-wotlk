-- MySQL dump 10.13  Distrib 8.4.3, for Win64 (x86_64)
--
-- Host: localhost    Database: acore_characters
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
-- Table structure for table `updates`
--

DROP TABLE IF EXISTS `updates`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `updates` (
  `name` varchar(200) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT 'filename with extension of the update.',
  `hash` char(40) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT '' COMMENT 'sha1 hash of the sql file.',
  `state` enum('RELEASED','CUSTOM','MODULE','ARCHIVED','PENDING') CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'RELEASED' COMMENT 'defines if an update is released or archived.',
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'timestamp when the query was applied.',
  `speed` int unsigned NOT NULL DEFAULT '0' COMMENT 'time the query takes to apply in ms.',
  PRIMARY KEY (`name`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='List of all applied updates in this database.';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `updates`
--

LOCK TABLES `updates` WRITE;
/*!40000 ALTER TABLE `updates` DISABLE KEYS */;
INSERT INTO `updates` VALUES
('2023_04_24_00.sql','D164A70B22B2462464484614018C3218B3259AE4','ARCHIVED','2024-01-20 14:25:01',30),
('2023_05_23_00.sql','A1A442D3F5049CDA2C067761F768C08BEFFFD26A','ARCHIVED','2024-01-20 14:25:01',35),
('2023_09_16_00.sql','5760BA953E3F0C73492B979A33A86771B82CE464','ARCHIVED','2024-01-20 14:25:01',68),
('2024_01_20_00.sql','FB9F840C7601C4F0939D23E87377D7DD9D145094','RELEASED','2024-01-20 14:25:01',26),
('2024_07_05_00.sql','1C9590EBB81D192A2DF101D6B0B2178E45306500','RELEASED','2024-07-17 00:46:08',88),
('2024_09_03_00.sql','6D7992803C7747E9CDE15A6EF0A0319DEE93DA51','RELEASED','2024-12-17 21:51:20',146),
('2024_09_22_00.sql','CC603A632BB6E01737A3D2DF7A85D1BEFF16C102','RELEASED','2024-12-17 21:51:20',96),
('2024_11_15_00.sql','AC109BE8DC3ABD09435A700F8ADE0050B9CB5170','RELEASED','2024-12-17 21:51:20',72),
('2025_01_31_00.sql','49B70E7107D57C75198BA707B849A6243A32863F','RELEASED','2025-07-19 10:16:15',44),
('2025_02_12_00.sql','BF2260040A6B47500D6C24BE74DB61198E5DA966','RELEASED','2025-07-19 10:16:15',80),
('2025_02_16_00.sql','BF15638A8F522A4D9DE51D42A71B860B95FB8031','RELEASED','2025-07-19 10:16:15',60),
('2025_03_09_00.sql','9BC72C8A080EDC1B3ECCB81C8146C8A5EA7E4266','RELEASED','2025-07-19 10:16:16',540),
('2025_07_11_00.sql','EA89D9F35FC3237E8B443902D6A7AD94DC7C8645','RELEASED','2025-07-19 10:16:16',127);
/*!40000 ALTER TABLE `updates` ENABLE KEYS */;
UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-07-19 12:10:17

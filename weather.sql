CREATE TABLE `weather` (
  `ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `temp` float DEFAULT NULL,
  `humidity` float DEFAULT NULL,
  `pressure` float DEFAULT NULL,
  `light` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1


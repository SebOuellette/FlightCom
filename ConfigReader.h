#pragma once
#include "Config.h"
#include <vector>
#include <fstream>

class ConfigReader
{
private:
	std::string configPath;
public:
	ConfigReader(std::string pathToConfig);
	std::vector<Config> readConfig();
	bool saveConfig(std::vector<Config> Details);
};


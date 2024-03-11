#include "ConfigReader.h"

ConfigReader::ConfigReader(std::string pathToCameraConfig)
{
	this->configPath = pathToCameraConfig;
}

std::vector<Config> ConfigReader::readConfig()
{
	std::vector<Config> details = std::vector<Config>();

	std::ifstream inFStream(configPath, std::fstream::in);
	int length = inFStream.tellg();
	std::string line;

	try
	{
		while (inFStream.is_open() && !inFStream.eof())
		{
			Config serverConfig;
			std::getline(inFStream, serverConfig.name, '\n');
			std::getline(inFStream, serverConfig.address, '\n');
			details.push_back(serverConfig);
		}
	}
	catch (...)
	{
		throw std::exception("Failed to read from file");
	}

	return details;
}

bool ConfigReader::saveConfig(std::vector<Config> details)
{
	std::ofstream outFStream(configPath, std::fstream::out);

	try
	{
		for (Config config : details)
		{
			if (outFStream.is_open())
			{
				outFStream.write(std::string(config.name + "\n").c_str(), config.name.size() + 1);
				outFStream.write(std::string(config.address + "\n").c_str(), config.address.size() + 1);
			}
		}
	}
	catch (...)
	{
		throw std::exception("Failed to write to file");
	}


	outFStream.close();
	return true;
}

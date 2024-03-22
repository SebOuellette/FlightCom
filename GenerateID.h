#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
class GenerateID
{
private:

	int length;
	std::string generated = "";
	std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

public:
	std::string GetRandomSequence(int length);
};


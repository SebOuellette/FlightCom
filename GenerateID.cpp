#include "GenerateID.h"

std::string GenerateID::GetRandomSequence(int length)
{
	this->generated = "";

	for (int i = 0; i < length; i++)
	{
		int num = rand() % (charset.size() - 1);
		this->generated += charset.at(num);
	}
	return generated;
}

#include "GenerateID.h"

std::string GenerateID::GetRandomSequence(int length)
{
	for (int i = 0; i < length; i++)
	{
		
		srand(time(0));
		generated += charset.at(rand() % charset.size() - 1);
	}
	return generated;
}

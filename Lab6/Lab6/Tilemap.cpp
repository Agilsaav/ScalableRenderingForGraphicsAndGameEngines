#include "Tilemap.h"

#include <iostream>
#include <fstream>
#include <sstream>

Tilemap::Tilemap(const std::string &filepath)
{
	std::ifstream stream(filepath);
	std::string line;

	while (std::getline(stream, line))
	{
		std::vector<int> tempv;
		std::string dummy;
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] != ' ')
			{
				dummy = line[i];
				tempv.push_back(std::atoi(dummy.c_str()));
			}
			else
				tempv.push_back(9);
		}

		tilemap.push_back(tempv);
	}



	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int count = 0;
	std::vector<int> vec_temp;

	for (const std::vector<int> &v : tilemap)
	{
		i = 0;
		vec_temp = {};
		for (int x : v)
		{
			vec_temp.push_back(count);
			i++;
			count++;
		}
		
		cellCounter.push_back(vec_temp);
		j++;
	}
}

Tilemap::~Tilemap()
{
}


std::vector<std::vector<int>> Tilemap::getTilemap()
{
	return tilemap;
}

std::vector<std::vector<int>>& Tilemap::getCellCounter()
{
	return cellCounter;
}

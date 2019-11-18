/* Lab6.cpp : This file contains the 'main' function. Program execution begins and ends there.

This program is based on the article: http://www.cse.yorku.ca/~amana/research/grid.pdf

The program needs an input file in order to work correctly.
The output of the computation is a visibility file generated in the Output Folder.

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <unordered_set>

#include "Tilemap.h"


/* Code to remove repeated elements of a vector
Extracted from: https://www.techiedelight.com/remove-duplicates-vector-cpp/
*/
void remove(std::vector<int>& v)
{
	std::unordered_set<int> s;
	auto end = std::remove_copy_if(v.begin(), v.end(), v.begin(),
		[&s](int const& i) {
			return !s.insert(i).second;
		});
	v.erase(end, v.end());
}

int main()
{

	Tilemap tilemap("Tilemap/TilemapProva2.txt");
	std::vector<std::vector<int>> tilemapvec = tilemap.getTilemap();
	std::vector<std::vector<int>> cellCounter = tilemap.getCellCounter();

	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); //Generates random Floats between 0.0 and 1.0
	std::default_random_engine generator; //Random engine class that generates pseudo-random numbers

	unsigned int i = 0; //Horitzontal -> x
	unsigned int j = 0; //Vertical-> y
	unsigned int numberOfRays = 300000;
	int cell_i, cell_j;
	const double pi = 3.14159265358979323846;
	double x, y, theta;
	double dirX, dirY, tMaxY, tMaxX;
	double distBoundaryX, distBoundaryY, deltaX, deltaY;
	int stepX, stepY;
	bool NotWall, firstIt;

	std::vector<std::vector<int>> visibleCells;
	std::vector<int> vec_temp;
	std::vector<std::unordered_set<int>> visibleCellsSets;
	std::unordered_set<int> set_temp;

	std::cout << "Calculating Visibility..." << std::endl;

	for (const std::vector<int> &v : tilemapvec)
	{
		i = 0;

		for (unsigned int num : v)
		{

			std::cout << "Computing Cell: " << cellCounter[j][i] << std::endl;

			//vec_temp = {};
			set_temp = {};

			//For each ray:
			for (unsigned int k = 0; k < numberOfRays; k++)
			{
				//Copy of the positions in order to mantain constant the real positions:
				cell_i = i;
				cell_j = j;

				x = (double)(cell_i + randomFloats(generator) ); //Initial x
				y = (double)(cell_j + randomFloats(generator) ); //Initial y
				theta = randomFloats(generator) * (2 * pi); //Angle

				dirX = cos(theta); //Vector x direction
				dirY = sin(theta); //Vector y direction

				//Step of X and Distance to the horitzontal/vertical Boundary
				if (dirX >= 0)
				{
					stepX = 1;
					distBoundaryX = (cell_i + 1) - x;

				}
				else
				{
					stepX = -1;
					distBoundaryX = x - cell_i;
				}

				//Step of Y
				if (dirY >= 0)
				{
					stepY = 1;
					distBoundaryY = (cell_j + 1) - y;
				}
				else
				{
					stepY = -1;
					distBoundaryY = y - cell_j;
				}

				//First Boundary Cross X and Y
				//We have to keep in mind that if dirX or dirY is equal to 0 the definition of tMax will give us an error
				//So we assign a large number to tMax
				if(dirX == 0) tMaxX = std::numeric_limits<double>::max();
				else tMaxX = distBoundaryX / dirX;

				if(dirX == 0) tMaxY = std::numeric_limits<double>::max();
				tMaxY = distBoundaryY / dirY;

				//Compute DeltaX and DeltaY:
				//As before we have to considerate the case where dirX or dirY is 0
				if (dirX == 0) deltaX = std::numeric_limits<double>::max();
				else deltaX = 1 / dirX;

				if (dirY == 0) tMaxX = std::numeric_limits<double>::max();
				else deltaY = 1 / dirY;

				NotWall = true;
				while (NotWall)
				{
					if (abs(tMaxX) < abs(tMaxY))
					{
						tMaxX += deltaX;
						cell_i += stepX;
					}
					else
					{
						tMaxY += deltaY;
						cell_j += stepY;

					}

					if (cell_i < 0 || cell_j < 0)
					{
						NotWall = false; //Cell number negative
	
					}
					else if (tilemapvec.size() <= cell_j)
					{
						NotWall = false; //Size of the map vectors greater
					}
					else if (tilemapvec[cell_j].size() <= cell_i)
					{
						NotWall = false; //Size of the vector greater
					}
					else if (tilemapvec[cell_j][cell_i] == 1)
					{
						set_temp.insert(cellCounter[cell_j][cell_i]);
						NotWall = false; //It's a wall

					}
					else
					{
						set_temp.insert(cellCounter[cell_j][cell_i]);
					}
						
				}

			}

			set_temp.insert(cellCounter[j][i]);
			visibleCellsSets.push_back(set_temp);
			i++;
		}
		j++;
	}


	std::cout << "Generating file..." << std::endl;

	std::ofstream myfile("Output/visibility_output.txt");
	unsigned int element = 0;

	if (myfile.is_open())
	{
		for (std::unordered_set<int>& v : visibleCellsSets)
		{
			element = 0;
			for (int x : v)
			{
				myfile << x;
				if (element < v.size() - 1) myfile << ",";
				element++;
			}

			myfile << "\n";

		}
		myfile.close();
		std::cout << "File Generated Correctly" << std::endl;
	}
	else
		std::cout << "Unable to open file!" << std::endl;


	//if (myfile.is_open())
//{
//	for (std::vector<int>& v : visibleCells)
//	{
//		remove(v);
//		element = 0;
//		for (int x : v)
//		{
//			myfile << x;
//			if (element < v.size() - 1) myfile << ",";
//			element++;
//		}

//		myfile << "\n";

//	}
//	myfile.close();
//}

}


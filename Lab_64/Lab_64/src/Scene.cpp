#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Scene.h"
#include "PLYReader.h"
#include "Tilemap.h"
#include "LOD.h"



Scene::Scene()
{
	mesh = NULL;
}

Scene::~Scene()
{
	if(mesh != NULL)
		delete mesh;
}


void Scene::init()
{
	initShaders("Res/shaders/basic.vert", "Res/shaders/basic.frag");

	currentTime = 0.0f;
	
	bPolygonFill = true;

	setParameters();

	//camera.init(glm::vec3(3.0f, 0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	camera.init(glm::vec3(2.0f, 0.5f,-1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	   	 
	//Load Models
	loadModels();
	modelMode = true;

	// Select which font you want to use
	if(!text.init("Res/fonts/OpenSans-Regular.ttf"))
		cout << "Could not load font!!!" << endl;

	//Load Tilemap
	std::cout << "Loading Tilemap..." << std::endl;
	Tilemap tilemap("Res/Tilemap/Tilemap.txt");
	tilemapvec = tilemap.getTilemap();

	numModels = tilemap.getNumModels();
	cellsPositions = tilemap.getPositions();
	cellCounter = tilemap.getCellCounter();

	//Load Visibility
	std::cout << "Loading Visibility File..." << std::endl;
	cellsVisibility = readVisibilityFile("Res/Visibility/visibility.txt");

	floortile = new FloorTile();

	std::cout << std::endl;
	std::cout << "Application ready!" << std::endl;

}

bool Scene::loadMesh(const char *filename)
{
	PLYReader reader;

	mesh->free();
	bool bSuccess = reader.readMesh(filename, *mesh);
	if(bSuccess)
	  mesh->sendToOpenGL(basicProgram);
	
	return bSuccess;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
}

void Scene::render(double fps)
{
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 proj = camera.getProjectionMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	//float distBB = sqrt(3.0)*2.0;
	
	glm::mat4 modelview = view * model;

	unsigned int actualCell = cellCounter[round(camera.Position.x)][round(abs(camera.Position.z))];
	vector<int> actualCellVisibility = cellsVisibility[actualCell];
	int cell_i, cell_j;

	selectLOD(actualCellVisibility, camera.Position);

	int numModel = 0;
	
	for (unsigned int i = 0; i<actualCellVisibility.size(); i++)
	{
		cell_j = actualCellVisibility[i] / tilemapvec[0].size();
		cell_i = actualCellVisibility[i] % tilemapvec[0].size();

		modelview = camera.getViewMatrix()*glm::mat4(1.0f);

		if (tilemapvec[cell_j][cell_i] == 1)
		{
			modelview = glm::translate(modelview, glm::vec3(cell_j, 0.0, -cell_i));
			SendUniforms(proj, modelview, 1);
			wall->render();
		}
		else if (tilemapvec[cell_j][cell_i] == 0)
		{
			modelview = glm::translate(modelview, glm::vec3(cell_j, 0.0, -cell_i));
			floortile->setUniforms(proj, modelview);
			floortile->DrawFloorTile();
		}
		else if (tilemapvec[cell_j][cell_i] == 3)
		{
			modelview = glm::translate(modelview, glm::vec3(cell_j, 0.0, -cell_i));
			floortile->setUniforms(proj, modelview);
			floortile->DrawFloorTile();


			modelview = glm::translate(modelview, glm::vec3(0.0, -0.1, 0.0));
			SendUniforms(proj, modelview, modelsLODFinal[numModel]);
			modelMode ? bunnys[modelsLODFinal[numModel]-1]->render() : bunnysM[modelsLODFinal[numModel]-1]->render();

			numModel += 1;
		}
		else if (tilemapvec[cell_j][cell_i] == 4)
		{
			modelview = glm::translate(modelview, glm::vec3(cell_j, 0.0, -cell_i));
			floortile->setUniforms(proj, modelview);
			floortile->DrawFloorTile();


			modelview = glm::translate(modelview, glm::vec3(0.0, 0.1, 0.0));
			SendUniforms(proj, modelview, modelsLODFinal[numModel]);
			modelMode ? arma[modelsLODFinal[numModel] - 1]->render() : armaM[modelsLODFinal[numModel] - 1]->render();

			numModel += 1;
		}
		else if (tilemapvec[cell_j][cell_i] == 5)
		{
			modelview = glm::translate(modelview, glm::vec3(cell_j, 0.0, -cell_i));
			floortile->setUniforms(proj, modelview);
			floortile->DrawFloorTile();


			modelview = glm::translate(modelview, glm::vec3(0.0, -0.1, 0.0));
			SendUniforms(proj, modelview, modelsLODFinal[numModel]);
			modelMode ? moais[modelsLODFinal[numModel] - 1]->render() : moaisM[modelsLODFinal[numModel] - 1]->render();

			numModel += 1;
		}
		else if (tilemapvec[cell_j][cell_i] == 6)
		{
			modelview = glm::translate(modelview, glm::vec3(cell_j, 0.0, -cell_i));
			floortile->setUniforms(proj, modelview);
			floortile->DrawFloorTile();


			modelview = glm::translate(modelview, glm::vec3(0.0, -0.15, 0.0));
			SendUniforms(proj, modelview, modelsLODFinal[numModel]);
			modelMode ? frogs[modelsLODFinal[numModel] - 1]->render() : frogsM[modelsLODFinal[numModel] - 1]->render();

			numModel += 1;
		}
	}

	text.render(std::to_string(fps), glm::vec2(20, 20), 16, glm::vec4(0, 0, 0, 1));
	if(modelMode)
		text.render("Mode: QEM ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
	else
		text.render("Mode: Mean Vertex ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
}

Camera &Scene::getCamera()
{
  return camera;
}

void Scene::switchPolygonMode()
{
  bPolygonFill = !bPolygonFill;
}

//Set the Parameters for the Time-Critical Rendering:
void Scene::setParameters()
{
	TPS = 25000000.0f; // Triangles/Sec
	FPS_fixed = 60.0f; //FPS wanted
	MaxCost = TPS / FPS_fixed;
}

void Scene::initShaders(const std::string &vpath, const std::string &fpath)
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, vpath);
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, fpath);
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	basicProgram.init();
	basicProgram.addShader(vShader);
	basicProgram.addShader(fShader);
	basicProgram.link();
	if(!basicProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << basicProgram.log() << endl << endl;
	}
	basicProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}


//Function to send Uniforms before draw the mesh
void Scene::SendUniforms(glm::mat4& proj, glm::mat4& modelview, const unsigned int modelLOD)
{
	glm::mat3 normalMatrix;

	basicProgram.use();
	basicProgram.setUniformMatrix4f("projection", proj);
	basicProgram.setUniformMatrix4f("modelview", modelview);
	normalMatrix = glm::inverseTranspose(modelview);
	basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
	basicProgram.setUniform1i("bLighting", 1);

	switch (modelLOD)
	{
	case 1:
		basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		break;
	case 2:
		basicProgram.setUniform4f("color", 0.0f, 0.0f, 1.0f, 1.0f);
		break;
	case 3:
		basicProgram.setUniform4f("color", 1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case 4:
		basicProgram.setUniform4f("color", 0.0f, 1.0f, 0.0f, 1.0f);
		break;
		//default:
		//	basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		//	break;
	};


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//Function to compute the distance between two vectors:
float Scene::computeDistance(const glm::vec3& pos, const glm::vec3& camPos)
{
	float distance;

	float a = powf((pos.x - camPos.x),2);
	float b = powf((pos.y - camPos.y), 2);
	float c = powf((pos.z - camPos.z), 2);

	distance = sqrt(a + b + c);

	return distance;
}

//Function to compute the benefit:
float Scene::computeBenefit(const int LOD, const float& d, const float& dist)
{
	float benefit;
	float LOD_power;

	if (LOD >= minLOD && LOD <= maxLOD)
	{
		switch (LOD)
		{
		case 1:
			LOD_power = pow(2, 8);
			break;
		case 2:
			LOD_power = pow(2, 7);
			break;
		case 3:
			LOD_power = pow(2, 6);
			break;
		case 4:
			LOD_power = pow(2, 5);
			break;

		}

		benefit = (LOD_power * dist) / d;
	}
	else
	{
		cout << "LOD OUT OF RANGE: LOD = " << LOD << endl;
		benefit = 0;
	}

	return benefit;
}

//Function to compute the cost:
float Scene::computeCost(unsigned int trianglesNum)
{
	float cost;

	cost = trianglesNum;

	return cost;
}

//Function which returns the number of triangles given the type and the lod.
float Scene::selectMesh(const int& type, const int& lod)
{
	if (type == 3)
		return modelMode ? bunnys[lod-1]->getTrianglesSize() : bunnysM[lod-1]->getTrianglesSize();
	else if (type == 4)
		return modelMode ? arma[lod-1]->getTrianglesSize() : armaM[lod-1]->getTrianglesSize();
	else if (type == 5)
		return modelMode ? moais[lod-1]->getTrianglesSize() : moaisM[lod-1]->getTrianglesSize();
	else if (type == 6)
		return modelMode ? frogs[lod - 1]->getTrianglesSize() : frogsM[lod - 1]->getTrianglesSize();
	else
	{
		std::cout << type << " " << lod << std::endl;
		cout << "Wrong Model dected!" << endl;
		return 0;
	}
}

//Function to read the visibility file:
vector<vector<int>> Scene::readVisibilityFile(const std::string& filepath)
{
	std::ifstream stream(filepath);

	vector<vector<int>> cellsVisibility_temp;

	for(std::string line, number; std::getline(stream, line);)
	{

		std::vector<int> tempv;
		std::istringstream iss(line);

		while (std::getline(iss, number, ','))
		{
			tempv.push_back(std::atoi(number.c_str()));
		}

		cellsVisibility_temp.push_back(tempv);
	}

	return cellsVisibility_temp;
}

//Function to load all the models and create the LOD:
void Scene::loadModels()
{
	PLYReader reader;

	LOD lod;
	maxLOD = 4;
	minLOD = 1;

	wall = new TriangleMesh();
	wall->buildWall();
	wall->sendToOpenGL(basicProgram);

	//Bunny Model:----------------------------------

	std::cout << "Loading Bunny original Model" << std::endl;
	bunny = new TriangleMesh();
	bool Success = reader.readMesh("Res/Models/bunny.ply", *bunny);
	if (Success) bunny->sendToOpenGL(basicProgram);

	bunnys.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading Bunny LOD with QEM: " << i + 1 << " of " << maxLOD << std::endl;
		bunnys[i] = new TriangleMesh(*bunny);
		lod.init(i + 1, *bunnys[i], true);
		if (Success) bunnys[i]->sendToOpenGL(basicProgram);
	}

	bunnysM.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading Bunny LOD without QEM: " << i + 1 << " of " << maxLOD << std::endl;
		bunnysM[i] = new TriangleMesh(*bunny);
		lod.init(i + 1, *bunnysM[i], false);
		if (Success) bunnysM[i]->sendToOpenGL(basicProgram);
	}

	std::cout << std::endl;

	//Armadillo Model:----------------------------------
	std::cout << "Loading Armadillo original Model" << std::endl;
	armadillo = new TriangleMesh();
	Success = reader.readMesh("Res/Models/Armadillo.ply", *armadillo);
	if (Success) armadillo->sendToOpenGL(basicProgram);

	arma.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading Armadillo LOD with QEM: " << i + 1 << " of " << maxLOD << std::endl;
		arma[i] = new TriangleMesh(*armadillo);
		lod.init(i + 1, *arma[i], true);
		if (Success) arma[i]->sendToOpenGL(basicProgram);
	}

	armaM.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading Armadillo LOD without QEM: " << i + 1 << " of " << maxLOD << std::endl;
		armaM[i] = new TriangleMesh(*armadillo);
		lod.init(i + 1, *armaM[i], false);
		if (Success) armaM[i]->sendToOpenGL(basicProgram);
	}

	std::cout << std::endl;

	//Moai Model:--------------------------------
	std::cout << "Loading moai original Model" << std::endl;
	moai = new TriangleMesh();
	Success = reader.readMesh("Res/Models/moai.ply", *moai);
	if (Success) moai->sendToOpenGL(basicProgram);

	moais.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading moai LOD with QEM: " << i + 1 << " of " << maxLOD << std::endl;
		moais[i] = new TriangleMesh(*moai);
		lod.init(i + 1, *moais[i], true);
		if (Success) moais[i]->sendToOpenGL(basicProgram);
	}

	moaisM.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading moai LOD without QEM: " << i + 1 << " of " << maxLOD << std::endl;
		moaisM[i] = new TriangleMesh(*moai);
		lod.init(i + 1, *moaisM[i], false);
		if (Success) moaisM[i]->sendToOpenGL(basicProgram);
	}

	std::cout << std::endl;

	//Frog Model:--------------------------------
	std::cout << "Loading frog original Model" << std::endl;
	frog = new TriangleMesh();
	Success = reader.readMesh("Res/Models/frog.ply", *frog);
	if (Success) frog->sendToOpenGL(basicProgram);

	frogs.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading frog LOD with QEM: " << i + 1 << " of " << maxLOD << std::endl;
		frogs[i] = new TriangleMesh(*frog);
		lod.init(i + 1, *frogs[i], true);
		if (Success) frogs[i]->sendToOpenGL(basicProgram);
	}

	frogsM.resize(4);
	for (unsigned int i = 0; i < maxLOD; i++)
	{
		std::cout << "Loading frog LOD without QEM: " << i + 1 << " of " << maxLOD << std::endl;
		frogsM[i] = new TriangleMesh(*frog);
		lod.init(i + 1, *frogsM[i], false);
		if (Success) frogsM[i]->sendToOpenGL(basicProgram);
	}

}

//Time-Critical Rendering function:
void Scene::selectLOD(const vector<int>& actualCellVisibility, const glm::vec3& camPos)
{
	int cell_i, cell_j, modelCount;
	unsigned int maxValuePos;
	vector<float> modelsDistances, modelsCost, modelsBenefit, modelsValues;
	vector<float> modelsCostNew, modelsBenefitNew, modelsValuesNew, modelValDif;
	vector<int> cellWithModels;
	vector<bool> modelsOut;
	vector<std::pair<int, int>> positionXY;
	std::pair<int, int> pair_temp;
	glm::vec3 position_temp;
	float distance_temp, cost_temp, benefit_temp, value1_temp, value2_temp;
	float maxValue;
	bool firstMod = true;

	float CostTot = 0;
	float distBB = sqrt(3.0) * 2.0; //Max distance Bounding Box

	modelsLODFinal = {};
	modelCount = 0;

	for (unsigned int i = 0; i < actualCellVisibility.size(); i++)
	{
		cell_j = actualCellVisibility[i] / tilemapvec[0].size();
		cell_i = actualCellVisibility[i] % tilemapvec[0].size();

		//If there is a model on the cell:
		if ((tilemapvec[cell_j][cell_i] != 1) && (tilemapvec[cell_j][cell_i] != 0))
		{
			modelsLODFinal.push_back(maxLOD); //Vector with the models LOD
			cellWithModels.push_back(tilemapvec[cell_j][cell_i]); //Vector with the cells with models
			modelsOut.push_back(false);

			pair_temp.first = cell_j;
			pair_temp.second = cell_i;
			positionXY.push_back(pair_temp);

			//Compute distances
			position_temp = glm::vec3(cell_j, 0.0, -cell_i);
			distance_temp = computeDistance(position_temp, camPos);
			modelsDistances.push_back(distance_temp); //Distance Model Position - Camera Position

			//LOD Max -> Min triangles
			cost_temp = computeCost(selectMesh(tilemapvec[cell_j][cell_i], maxLOD)); //Cost at max LOD -> Min triangles
			modelsCost.push_back(cost_temp);
			CostTot += cost_temp;
			benefit_temp = computeBenefit(maxLOD, distBB, distance_temp); //Benefit at max LOD -> Min triangles
			modelsBenefit.push_back(benefit_temp);
			value1_temp = benefit_temp / cost_temp; //Value max LOD -> Min triangles
			modelsValues.push_back(value2_temp);

			//LOD Max - 1
			cost_temp = computeCost(selectMesh(tilemapvec[cell_j][cell_i], maxLOD - 1)); //Cost at max LOD - 1 
			modelsCostNew.push_back(cost_temp);
			benefit_temp = computeBenefit(maxLOD - 1, distBB, distance_temp); //Benefit at max LOD - 1
			modelsBenefitNew.push_back(benefit_temp);
			value2_temp = benefit_temp / cost_temp; //Value max LOD -1
			modelsValuesNew.push_back(value2_temp);

			modelValDif.push_back(value2_temp - value1_temp); //Calculate the difference

			//Change min value position and min value
			if (firstMod)
			{
				maxValue = (value2_temp - value1_temp);
				maxValuePos = modelCount;
				firstMod = false;
			}
			else
			{
				if ((value2_temp - value1_temp) > maxValue) maxValuePos = modelCount;
				maxValue = max((value2_temp - value1_temp), maxValue);
			}

			modelCount += 1;
		}

	}

	bool boolMaxCost = true;
	int countMin = 0;
	int countToZero = 0;

	int countOut = 0;

	if (modelCount > 0)
	{
		while (boolMaxCost)
		{
			if (countMin == modelsValuesNew.size() || countToZero == modelsValuesNew.size())
			{
				boolMaxCost = false;
			}
			else if (CostTot - modelsCost[maxValuePos] + modelsCostNew[maxValuePos] < MaxCost && modelsLODFinal[maxValuePos] != minLOD)
			{
				modelsLODFinal[maxValuePos] -= 1; //New LOD
				CostTot = CostTot - modelsCost[maxValuePos] + modelsCostNew[maxValuePos]; //New Cost Tot

				//If the model is in the minimum LOD:
				if (modelsLODFinal[maxValuePos] == minLOD)
				{
					countMin += 1; //Model at min LOD;
					modelsOut[maxValuePos] = true;
					countToZero += 1;

					maxValue = -1000000000;
					maxValuePos = 0;
					countOut = 0;
					//Calculate the new max:
					for (unsigned int i = 0; i < modelsValues.size(); i++)
					{
						if (modelsOut[i] == false)
						{

							if (modelValDif[i] > maxValue) maxValuePos = i;
							maxValue = max(modelValDif[i], maxValue);

						}
						else
							countOut += 1;

					}
					if (countOut == modelsOut.size()) boolMaxCost = false;
				}
				else
				{
					//Assing old values:
					modelsCost[maxValuePos] = modelsCostNew[maxValuePos];
					modelsValues[maxValuePos] = modelsValuesNew[maxValuePos];

					//Calculate new values
					modelsCostNew[maxValuePos] = computeCost(selectMesh(tilemapvec[positionXY[maxValuePos].first][positionXY[maxValuePos].second], modelsLODFinal[maxValuePos]-1));
					modelsBenefitNew[maxValuePos] = computeBenefit(modelsLODFinal[maxValuePos]-1, distBB, distance_temp); //Benefit at max LOD - 1
					modelsValuesNew[maxValuePos] = modelsBenefitNew[maxValuePos] / modelsCostNew[maxValuePos];
					modelValDif[maxValuePos] =(modelsValuesNew[maxValuePos] - modelsValues[maxValuePos]);

					maxValue = -1000000000;
					maxValuePos = 0;
					countOut = 0;
					//Calculate the new max: ->introudir els if x busca el max
					for (unsigned int i = 0; i < modelsValues.size(); i++)
					{
						if (modelsOut[i] == false)
						{
							if (modelValDif[i] > maxValue) maxValuePos = i;
							maxValue = max(modelValDif[i], maxValue);

						}
						else
							countOut += 1;

					}
				}
			}
			else if ((CostTot - modelsCost[maxValuePos] + modelsCostNew[maxValuePos]) > MaxCost && modelsLODFinal[maxValuePos] != minLOD)
			{
				modelsOut[maxValuePos] = true;
				countToZero += 1;


				maxValue = -1000000000;
				maxValuePos = 0;
				countOut = 0;
				//Calculate the new max: ->introudir els if x busca el max
				for (unsigned int i = 0; i < modelsValues.size(); i++)
				{
					if (modelsOut[i] == false)
					{
						if (modelValDif[i] > maxValue) maxValuePos = i;
						maxValue = max(modelValDif[i], maxValue);

					}
					else
						countOut += 1;

				}
				if (countOut == modelsOut.size()) boolMaxCost = false;

			}
		}
	}
	
}

//Function to change the mode:
void Scene::changeModelMode()
{
	if (modelMode)
		modelMode = false;
	else
		modelMode = true;
}








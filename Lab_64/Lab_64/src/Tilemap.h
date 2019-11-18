#include <string>
#include <vector>

#include "TriangleMesh.h"
#include "FloorTile.h"

#include <glm/glm.hpp>

class Tilemap
{
public:
	Tilemap(const std::string &filepath);
	~Tilemap();

	std::vector<std::vector<int>> getTilemap();
	unsigned int& getNumModels();
	std::vector<glm::vec3>& getPositions();
	std::vector<std::vector<int>>& getCellCounter();

private:
	std::vector<std::vector<int>> tilemap;

	unsigned int numModels;
	std::vector<glm::vec3> positions;
	std::vector<std::vector<int>> cellCounter;
};
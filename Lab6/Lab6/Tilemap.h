#include <string>
#include <vector>


class Tilemap
{
public:
	Tilemap(const std::string &filepath);
	~Tilemap();

	std::vector<std::vector<int>> getTilemap();
	std::vector<std::vector<int>>& getCellCounter();

private:
	std::vector<std::vector<int>> tilemap;
	std::vector<std::vector<int>> cellCounter;
};
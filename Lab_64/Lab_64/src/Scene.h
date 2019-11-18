#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "Camera.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"
#include "Text.h"
#include "FloorTile.h"



// Scene contains all the entities of our game.
// It is responsible for updating and render them.

struct MeshProp
{
	unsigned int ID; 
	float distance;
	unsigned int LOD;
	float Cost;
	float Benefit;
	float Value;

};


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	bool loadMesh(const char *filename);
	void update(int deltaTime);
	void render(double fps);

	void changeModelMode();

  Camera &getCamera();
  
  void switchPolygonMode();

private:

	void setParameters();
	void initShaders(const std::string &vpath, const std::string &fpath);
	void SendUniforms(glm::mat4& proj, glm::mat4& modelview, const unsigned int modelLOD);
	float computeDistance(const glm::vec3& pos, const glm::vec3& camPos);
	float computeBenefit(const int LOD, const float& d, const float& dist);
	float computeCost(unsigned int trianglesNum);
	float selectMesh(const int& type, const int& lod);
	vector<vector<int>> readVisibilityFile(const std::string& filepath);
	void loadModels();
	void selectLOD(const vector<int>& actualCellVisibility, const glm::vec3& camPos);
	//bool sortinrev(const pair<float, int>& a, const pair<float, int>& b);

private:
	Camera camera;
	TriangleMesh *mesh;
	ShaderProgram basicProgram;
	float currentTime;
	
	bool bPolygonFill;
	
	Text text;

	TriangleMesh *bunny;
	vector<TriangleMesh*> bunnys;
	vector<TriangleMesh*> bunnysM;

	TriangleMesh *armadillo;
	vector<TriangleMesh*> arma;
	vector<TriangleMesh*> armaM;

	TriangleMesh* moai;
	vector<TriangleMesh*> moais;
	vector<TriangleMesh*> moaisM;

	TriangleMesh* frog;
	vector<TriangleMesh*> frogs;
	vector<TriangleMesh*> frogsM;

	bool modelMode;
	
	//vector<int> modelsLOD;
	int maxLOD, minLOD;
	vector<int> modelsLODFinal;

	vector<MeshProp> meshesProp;

	TriangleMesh *dragon;
	TriangleMesh *cube;
	TriangleMesh *wall;

	std::vector<TriangleMesh> meshes;

	std::vector<std::vector<int>> tilemapvec;
	vector<vector<int>> cellsVisibility;
	unsigned int numModels;
	std::vector<glm::vec3> cellsPositions;
	std::vector<std::vector<int>> cellCounter;

	FloorTile *floortile;

	float TPS; // Triangles/Sec
	float FPS_fixed; //FPS wanted
	float MaxCost;
};


#endif // _SCENE_INCLUDE


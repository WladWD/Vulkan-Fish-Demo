#include "AssimpTest.h"


Test::AssimpTest::AssimpTest() {
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile("Resources\\Models\\bb8\\bb8.obj", 
		aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);// | aiProcess_GenSmoothNormals);// | aiProcess_OptimizeMeshes);

	Importer.FreeScene();
}

Test::AssimpTest::~AssimpTest()
{
}

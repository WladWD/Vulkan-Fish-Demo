#include "LoadModelAssimp.h"

LoadManager::LoadModelAssimp::LoadModelAssimp(
	const Asset::AssetLoader * assetLoader,
	const VulkanEngineApplication::VulkanData * vulkanData,
	const std::shared_ptr<Draw::Model> &scene): scene(scene) {

	imageLoader = std::shared_ptr<ImageManager::ImageLoader>
		(new ImageManager::ImageLoader(assetLoader, vulkanData));

	loadMesh.resize(static_cast<int32_t>(Draw::DrawDataTypes::DrawDataTypeCount));
	loadMesh[static_cast<int32_t>(Draw::DrawDataTypes::DrawDataDiffuse)] =
		std::unique_ptr<LoadMesh>(new LoadDiffuseMesh(imageLoader, scene));
}

void LoadManager::LoadModelAssimp::addModel(std::string model) {
	Assimp::Importer Importer;

	const aiScene *pScene = Importer.ReadFile(model.c_str(),
		aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);// | aiProcess_OptimizeMeshes);

	for (const auto &loader : loadMesh) {
		loader->addModel(pScene);
	}

	Importer.FreeScene();
}

void LoadManager::LoadModelAssimp::packScene(void) {
	//TODO pack output data
}

const std::shared_ptr<Draw::Model>& LoadManager::LoadModelAssimp::getLoadedScene(void) {
	return scene;
}

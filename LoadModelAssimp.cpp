#include "LoadModelAssimp.h"

#ifdef USE_ASSIMP
LoadManager::LoadModelAssimp::LoadModelAssimp(
	const Asset::AssetLoader * assetLoader,
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
	const std::shared_ptr<Draw::Model> &scene): 
	scene(scene), vulkanData(vulkanData),
	vertexBuffer(VK_NULL_HANDLE), indexBuffer(VK_NULL_HANDLE),
	vertexBufferMemory(VK_NULL_HANDLE) , indexBufferMemory(VK_NULL_HANDLE) {

	imageLoader = std::shared_ptr<ImageManager::ImageLoader>
		(new ImageManager::ImageLoader(assetLoader, vulkanData));

	materialLoader = std::unique_ptr<LoadManager::MaterialLoader>(
		new LoadManager::MaterialLoader(vulkanEngineData->imageContainer));

	loadMesh.resize(static_cast<int32_t>(Draw::DrawDataTypes::DrawDataTypeCount));
	loadMesh[static_cast<int32_t>(Draw::DrawDataTypes::DrawDataDiffuse)] =
		std::unique_ptr<LoadMesh>(new LoadDiffuseMesh(imageLoader, scene));
}

void LoadManager::LoadModelAssimp::addModel(std::string model) {
	Assimp::Importer Importer;

	const aiScene *pScene = Importer.ReadFile(model.c_str(),
		aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);// | aiProcess_OptimizeMeshes);

	auto idx = model.find_last_of('\\');
	if (idx == std::string::npos) model = "";
	else model = model.substr(0, idx + 1);

	materialLoader->addMaterials(pScene, model);
	scene->materials = materialLoader->getMaterials();
	scene->mWorld = glm::mat4(1.0);

	for (const auto &loader : loadMesh) {
		if (loader != nullptr) {
			loader->addModel(pScene);
		}
	}

	Importer.FreeScene();
}

VkDeviceSize LoadManager::LoadModelAssimp::packVertexBuffer(void) {
	VkDeviceSize bufferSize = 0;
	uint32_t bufferOffset = 0;

	if (vertexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(vulkanData->device, vertexBuffer, nullptr);
		vertexBuffer = VK_NULL_HANDLE;
	}

	if (vertexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(vulkanData->device, vertexBufferMemory, nullptr);
		vertexBufferMemory = VK_NULL_HANDLE;
	}

	for (const auto &loader : loadMesh) {
		if (loader != nullptr) {
			bufferSize += loader->getRawVertexDataSize();
		}
	}

	std::vector<int8_t> vertex(bufferSize);
	for (const auto &loader : loadMesh) {
		if (loader != nullptr) {
			memcpy(vertex.data() + bufferOffset, loader->getRawVertexData(), loader->getRawVertexDataSize());
			bufferOffset += loader->getRawVertexDataSize();
		}
	}

	VulkanInitialize::createInitializeDeviceLocalBuffer(
		vulkanData,
		vulkanData->commandPool,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		vertex.data(),
		vertexBuffer,
		vertexBufferMemory
	);

	return bufferSize;
}

VkDeviceSize LoadManager::LoadModelAssimp::packIndexBuffer(void) {
	VkDeviceSize bufferSize = 0;
	uint32_t bufferOffset = 0;

	if (indexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(vulkanData->device, indexBuffer, nullptr);
		indexBuffer = VK_NULL_HANDLE;
	}

	if (indexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(vulkanData->device, indexBufferMemory, nullptr);
		indexBufferMemory = VK_NULL_HANDLE;
	}

	for (const auto &loader : loadMesh) {
		if (loader != nullptr) {
			bufferSize += loader->getRawIndexDataSize();
		}
	}

	std::vector<int8_t> index(bufferSize);
	for (const auto &loader : loadMesh) {
		if (loader != nullptr) {
			memcpy(index.data() + bufferOffset, loader->getRawIndexData(), loader->getRawIndexDataSize());
			bufferOffset += loader->getRawIndexDataSize();
		}
	}

	VulkanInitialize::createInitializeDeviceLocalBuffer(
		vulkanData,
		vulkanData->commandPool,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		index.data(),
		indexBuffer,
		indexBufferMemory
	);

	return bufferSize;
}

void LoadManager::LoadModelAssimp::packScene(void) {
	VkDeviceSize vertexBufferSize = packVertexBuffer();
	VkDeviceSize indexBufferSize = packIndexBuffer();

	scene->buffer = std::unique_ptr<Draw::ModelBuffer>(
		new Draw::ModelBuffer(
			vulkanData,
			vertexBuffer,
			indexBuffer,
			vertexBufferMemory,
			indexBufferMemory,
			vertexBufferSize,
			indexBufferSize
		));
}

const std::shared_ptr<Draw::Model>& LoadManager::LoadModelAssimp::getLoadedScene(void) {
	return scene;
}
#endif

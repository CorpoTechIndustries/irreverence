#include <engine/renderer/model.h>

#include <engine/log.h>

#include <platform/memory.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_LOADER_FLAG 	aiProcess_Triangulate | \
aiProcess_OptimizeMeshes | \
aiProcess_OptimizeGraph | \
aiProcess_JoinIdenticalVertices | \
aiProcess_FlipUVs | \
aiProcess_GenSmoothNormals | \
aiProcess_GenUVCoords | \
aiProcess_GenBoundingBoxes

static model_t* s_pModel = NULL;
static const char* s_pModelPath = NULL;
static array_t s_pMeshArray = NULL;
static array_t s_pMeshMaterialsArray = NULL;

typedef struct
{
	bool success;
	
	const char* materialPath[MAX_MODEL_SKINS];
	uint8_t materialCount;

	mesh_vertexmodel_t* vertices;
	uint32_t* indices;
} mesh_data_t;

static mesh_data_t ProcessMesh(const struct aiMesh* mesh)
{
	if (mesh->mNumVertices == 0 || mesh->mNumFaces == 0)  {
		return (mesh_data_t){ false };
	}

	mesh_data_t meshdata = {
		.success = true,
		.vertices = Array_CreateSize(mesh_vertexmodel_t, mesh->mNumVertices),
		.indices = Array_CreateSize(uint32_t, mesh->mNumFaces * 3),
		.materialCount = 0
	};

	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
		mesh_vertexmodel_t vertex;

		const struct aiVector3D* aiVertex = &mesh->mVertices[i];
		vertex.x = aiVertex->x;
		vertex.y = aiVertex->y;
		vertex.z = aiVertex->z;

		if (mesh->mNormals) {
			const struct aiVector3D* aiNormal = &mesh->mNormals[i];
			vertex.nx = aiNormal->x;
			vertex.ny = aiNormal->y;
			vertex.nz = aiNormal->z;
		}

		if (mesh->mTextureCoords[0]) {
			const struct aiVector3D* aiUV = &mesh->mTextureCoords[0][i];
			vertex.tx = aiUV->x;
			vertex.tx = aiUV->y;
		}
		else {
			vertex.tx = 0.0f;
			vertex.tx = 0.0f;
		}

		meshdata.vertices[i] = vertex;
	}

	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		const struct aiFace* face = &mesh->mFaces[i];
		for (uint32_t j = 0; j < face->mNumIndices; j++) {
			meshdata.indices[i * face->mNumIndices + j] = face->mIndices[j];
		}
	}

	return meshdata;
}

static void RecursiveProcessNode(const struct aiScene* scene, const struct aiNode* node)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		mesh_data_t meshdata = ProcessMesh(scene->mMeshes[node->mMeshes[i]]);

		if (meshdata.success) {
			uint32_t vertexCount = (uint32_t)Array_Size(meshdata.vertices);
			uint32_t indexCount = (uint32_t)Array_Size(meshdata.indices);

			mesh_t mesh;
			Mesh_InitModel(&mesh, meshdata.vertices, vertexCount, meshdata.indices, indexCount);

			Array_Push(s_pMeshArray, mesh);
			
			Array_Destroy(meshdata.vertices);
			Array_Destroy(meshdata.indices);
		} else {
			LOG_ERROR("An error occurred while loading Model \"%s\" while processing node named %s", s_pModelPath, node->mName.data);
			Model_Destroy(s_pModel);

			return;
		}
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		RecursiveProcessNode(scene, node->mChildren[i]);
	}
}

void Model_Init(model_t* model, const char* path)
{
	if (!path) return;

	model->meshes = NULL;
	model->meshMaterials = NULL;

	const char* fileExtension = strrchr(path, '.');
	if (!fileExtension || aiIsExtensionSupported(fileExtension) == AI_FALSE) {
		LOG_ERROR("Model path does not have a valid or supported extension");
		return;
	}
	const struct aiScene* scene = NULL;
	scene = aiImportFile(path, ASSIMP_LOADER_FLAG);

	if (!scene) {
		LOG_ERROR("Model could not be loaded");
		return;
	}

	if (scene->mNumMeshes == 0) {
		LOG_ERROR("Model has no meshes");
		return;
	}
	
	model->meshes = Array_Create(mesh_t);

	s_pModelPath = path;
	s_pMeshArray = model->meshes;
	s_pMeshMaterialsArray = model->meshMaterials;

	RecursiveProcessNode(scene, scene->mRootNode);

	aiReleaseImport(scene);

	s_pModelPath = NULL;
	s_pMeshArray = NULL;
	s_pMeshMaterialsArray = NULL;
}

void Model_InitFromMeshes(model_t* model, mesh_t* meshes, uint32_t mesh_count)
{
	model->meshes = Array_CreateSize(mesh_t, mesh_count);
	for (uint32_t i = 0; i < mesh_count; i++) {
		Array_Set(model->meshes, i, meshes[i]);
	}

	model->meshMaterials = NULL;
}

void Model_Destroy(model_t* model)
{
	uint32_t meshCount = (uint32_t)Array_Size(model->meshes);
	for (uint32_t i = 0; i < meshCount; i++) {
		mesh_t mesh;
		Array_Get(model->meshes, i, mesh);

		Mesh_Destroy(&mesh);
	}

	/*
	uint32_t meshMaterialsCount = (uint32_t)Array_Size(model->meshMaterials);
	for (uint32_t i = 0; i < meshMaterialsCount; i++) {
		array_t meshMaterials = NULL;
		Array_Get(model->meshMaterials, i, meshMaterials);
		
		if (meshMaterials) Array_Destroy(meshMaterials);
	}
	*/

	if (model->meshes) { 
		Array_Destroy(model->meshes);
		model->meshes = NULL;
	}

	if (model->meshMaterials) {
		Array_Destroy(model->meshMaterials);
		model->meshMaterials = NULL;
	}
}

void Model_Draw(model_t* model, const mesh_instancemodel_t* instance, uint32_t skin)
{
	uint32_t meshCount = (uint32_t)Array_Count(model->meshes);
	for (uint32_t i = 0; i < meshCount; i++) {
		mesh_t mesh;
		Array_Get(model->meshes, i, mesh);

		/*
		array_t meshMaterials;
		Array_Get(model->meshMaterials, i, meshMaterials);
		material_t* material;
		Array_Get(meshMaterials, skin, material);
		*/

		Mesh_DrawModel(&mesh, instance);
	}
}

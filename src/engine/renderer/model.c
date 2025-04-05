#include <engine/renderer/model.h>
#include <engine/renderer/renderer.h>

#include <engine/log.h>

#include <public/aabb.h>

#include <platform/path.h>
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
static bool s_bSuccess = false;
static bool s_bAnimated = false;

typedef struct
{
	bool success;

	const char* materialPath[MAX_MODEL_SKINS];
	uint8_t materialCount;

	void* vertices;
	uint32_t* indices;
} mesh_data_t;

static void ConvertMat4(const struct aiMatrix4x4* aimat, dualquat_t* dest)
{
    mat4_t mat = {
		aimat->a1, aimat->b1, aimat->c1, aimat->d1,
		aimat->a2, aimat->b2, aimat->c2, aimat->d2,
		aimat->a3, aimat->b3, aimat->c3, aimat->d3,
		aimat->a4, aimat->b4, aimat->c4, aimat->d4
    };

	Mat4_ToDualQuat(&mat, dest);
}

static mesh_data_t ProcessMeshModel(const struct aiMesh* mesh)
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

	mesh_vertexmodel_t* mdlVertices = meshdata.vertices;
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

		mdlVertices[i] = vertex;
	}

	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		const struct aiFace* face = &mesh->mFaces[i];
		for (uint32_t j = 0; j < face->mNumIndices; j++) {
			meshdata.indices[i * face->mNumIndices + j] = face->mIndices[j];
		}
	}

	return meshdata;
}

static mesh_data_t ProcessMeshAnimated(const struct aiMesh* mesh)
{
	if (mesh->mNumVertices == 0 || mesh->mNumFaces == 0)  {
		return (mesh_data_t){ false };
	}

	mesh_data_t meshdata = {
		.success = true,
		.vertices = Array_CreateSize(mesh_vertexanimated_t, mesh->mNumVertices),
		.indices = Array_CreateSize(uint32_t, mesh->mNumFaces * 3),
		.materialCount = 0
	};

	mesh_vertexanimated_t* animVertices = meshdata.vertices;
	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
		mesh_vertexanimated_t vertex;

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

		for (uint32_t i = 0; i < MAX_MODEL_WEIGHTS; i++) {
			vertex.bones[i] = MAX_MODEL_BONES;
			vertex.weights[i] = 0.0f;
		}

		animVertices[i] = vertex;
	}

	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		const struct aiFace* face = &mesh->mFaces[i];
		for (uint32_t j = 0; j < face->mNumIndices; j++) {
			meshdata.indices[i * face->mNumIndices + j] = face->mIndices[j];
		}
	}

	for (uint32_t i = 0; i < mesh->mNumBones; i++) {
		const struct aiBone* aibone = mesh->mBones[i];
		bone_info_t* boneinfo = Model_FindBoneInfo(s_pModel, aibone->mName.data);
		
		uint8_t boneId = MAX_MODEL_BONES;
		if (!boneinfo) {
			bone_info_t newBoneinfo;
			newBoneinfo.id = s_pModel->bones.count;
			
			Sys_MemZero(newBoneinfo.name, MAX_BONEINFO_NAME_LENGTH);
			if (aibone->mName.length > 0) {
				strncpy(newBoneinfo.name, aibone->mName.data, MATH_MIN(aibone->mName.length, MAX_BONEINFO_NAME_LENGTH));
			}

			ConvertMat4(&aibone->mOffsetMatrix, &newBoneinfo.offset);

			boneId = newBoneinfo.id;

			Array_Push(s_pModel->bones.list, newBoneinfo);

			s_pModel->bones.count++;
		} else {
			boneId = boneinfo->id;
		}

		if (boneId == MAX_MODEL_BONES) {
			LOG_ERROR("Bone could not be created nor could find it, Model \"%s\"", s_pModelPath);
			return (mesh_data_t){ false };
		}

		const struct aiVertexWeight* aiweights = aibone->mWeights;
		for (uint32_t j = 0; j < aibone->mNumWeights; j++) {
			uint32_t vertexId = aiweights[j].mVertexId;
			float weight = aiweights[j].mWeight;

			mesh_vertexanimated_t* vertex = (mesh_vertexanimated_t*)&animVertices[vertexId];
			for (uint32_t k = 0; k < MAX_MODEL_WEIGHTS; k++) {
				if (vertex->bones[k] == MAX_MODEL_BONES) {
					vertex->weights[k] = weight;
					vertex->bones[k] = boneId;
					break;
				}
			}
			
		}
		
	}
	
	return meshdata;
}

static void RecursiveProcessNode(const struct aiScene* scene, const struct aiNode* node)
{
	if (!s_bSuccess) return;

	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		mesh_data_t meshdata;
		
		if (s_bAnimated) {
			meshdata = ProcessMeshAnimated(scene->mMeshes[node->mMeshes[i]]);
		} else {
			meshdata = ProcessMeshModel(scene->mMeshes[node->mMeshes[i]]);
		}

		if (meshdata.success) {
			uint32_t vertexCount = (uint32_t)Array_Size(meshdata.vertices);
			uint32_t indexCount = (uint32_t)Array_Size(meshdata.indices);

			mesh_t mesh;
			if (s_bAnimated) {
				Mesh_InitAnimated(&mesh, meshdata.vertices, vertexCount, meshdata.indices, indexCount);
			} else {
				Mesh_InitModel(&mesh, meshdata.vertices, vertexCount, meshdata.indices, indexCount);
			}

			Array_Push(s_pMeshArray, mesh);

			Array_Destroy(meshdata.vertices);
			Array_Destroy(meshdata.indices);
		} else {
			LOG_ERROR("An error occurred while loading Model \"%s\" when processing node named \"%s\"", s_pModelPath, node->mName.data);
			Model_Destroy(s_pModel);

			if (meshdata.vertices) {
				Array_Destroy(meshdata.vertices);
			}

			if (meshdata.indices) {
				Array_Destroy(meshdata.indices);
			}

			s_bSuccess = false;

			return;
		}
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		RecursiveProcessNode(scene, node->mChildren[i]);
	}
}

bool Model_Init(model_t* model, const char* path, bool animated)
{
	model->meshes = NULL;
	model->meshMaterials = NULL;
	model->bones.count = 0;
	model->bones.list = NULL;
	
	if (!path) return false;
	
	const char* fileExtension = Sys_PathGetExtension(path);
	if (!fileExtension || aiIsExtensionSupported(fileExtension) == AI_FALSE) {
		LOG_ERROR("Model path does not have a valid or supported extension");
		return false;
	}
	
	const struct aiScene* scene = NULL;
	scene = aiImportFile(path, ASSIMP_LOADER_FLAG);
	
	if (!scene) {
		LOG_ERROR("Model could not be loaded");
		return false;
	}
	
	if (scene->mNumMeshes == 0) {
		LOG_ERROR("Model has no meshes");
		return false;
	}
	
	model->meshes = Array_Create(mesh_t);
	
	if (animated) {
		model->bones.list = Array_Create(bone_info_t);
	}

	s_pModel = model;
	s_pModelPath = path;
	s_pMeshArray = model->meshes;
	s_pMeshMaterialsArray = model->meshMaterials;
	
	s_bSuccess = true;
	s_bAnimated = animated;

	aabb_t boundingbox = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

	for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
		const struct aiAABB* aiaabb = &scene->mMeshes[i]->mAABB;

		boundingbox.min.x = MATH_MIN(aiaabb->mMin.x, boundingbox.min.x);
		boundingbox.min.y = MATH_MIN(aiaabb->mMin.y, boundingbox.min.y);
		boundingbox.min.z = MATH_MIN(aiaabb->mMin.z, boundingbox.min.z);
		
		boundingbox.max.x = MATH_MAX(aiaabb->mMax.x, boundingbox.max.x);
		boundingbox.max.y = MATH_MAX(aiaabb->mMax.y, boundingbox.max.y);
		boundingbox.max.z = MATH_MAX(aiaabb->mMax.z, boundingbox.max.z);
	}

	RecursiveProcessNode(scene, scene->mRootNode);

	aiReleaseImport(scene);

	// TODO: Realloc to free unused mat4s AFTER loading all anims
	//model->bones.offsets = Sys_ReAlloc(model->bones.offsets, sizeof(mat4_t) * model->bones.count); 

	return s_bSuccess;
}

bool Model_InitFromMeshes(model_t* model, mesh_t* meshes, uint32_t mesh_count)
{
	model->meshes = Array_CreateSize(mesh_t, mesh_count);
	for (uint32_t i = 0; i < mesh_count; i++) {
		Array_Set(model->meshes, i, meshes[i]);
	}

	model->meshMaterials = NULL;

	return true;
}

void Model_Destroy(model_t* model)
{
	if (model->meshes) {
		uint32_t meshCount = (uint32_t)Array_Size(model->meshes);
		for (uint32_t i = 0; i < meshCount; i++) {
			mesh_t mesh;
			Array_Get(model->meshes, i, mesh);

			Mesh_Destroy(&mesh);
		}

		Array_Destroy(model->meshes);
		model->meshes = NULL;
	}

	if (model->meshMaterials) {
		Array_Destroy(model->meshMaterials);
		model->meshMaterials = NULL;
	}

	if (model->bones.list) {
		Array_Destroy(model->bones.list);
		model->bones.list = NULL;
	}
}

void Model_AddInstance(model_t* model, const mesh_instancemodel_t* instance)
{
	if (!model->meshes) return;

	uint32_t meshCount = (uint32_t)Array_Count(model->meshes);
	for (uint32_t i = 0; i < meshCount; i++) {
		Mesh_AddInstance(&model->meshes[i], instance, R_GetWhiteMaterial());
	}
}

void Model_ClearInstances(model_t* model)
{
	if (!model->meshes) return;

	uint32_t meshCount = (uint32_t)Array_Count(model->meshes);
	for (uint32_t i = 0; i < meshCount; i++) {
		Mesh_ClearInstances(&model->meshes[i], R_GetWhiteMaterial());
	}
}

void Model_Draw(model_t* model, const mesh_instancemodel_t* instance, uint32_t skin)
{
	if (!model->meshes) return;

	uint32_t meshCount = (uint32_t)Array_Count(model->meshes);
	for (uint32_t i = 0; i < meshCount; i++) {
		Mesh_Draw(&model->meshes[i], instance);
	}
}

void Model_DrawInstances(model_t* model)
{
	if (!model->meshes) return;

	uint32_t meshCount = (uint32_t)Array_Count(model->meshes);
	for (uint32_t i = 0; i < meshCount; i++) {
		Mesh_DrawInstances(&model->meshes[i], R_GetWhiteMaterial());
	}
}

bone_info_t* Model_FindBoneInfo(model_t* model, const char* name)
{
	uint32_t boneCount = (uint32_t)Array_Count(model->bones.list);
	for (uint8_t i = 0; i < boneCount; i++) {
		bone_info_t* boneinfo = &model->bones.list[i];
		if (strncmp(name, boneinfo->name, MAX_BONEINFO_NAME_LENGTH) == 0) {
			return boneinfo;
		}
	}

	return NULL;
}

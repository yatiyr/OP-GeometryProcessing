#pragma once

#include <vector>
#include <limits>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GeoProcess/System/Geometry/Mesh.h>
#include <GeoProcess/System/Geometry/Model.h>
#include <GeoProcess/System/Geometry/Line.h>
#include <GeoProcess/System/Geometry/Icosphere.h>
#include <GeoProcess/System/RenderSystem/Shader.h>
#include <GeoProcess/System/RenderSystem/VertexArray.h>
#include <GeoProcess/System/RenderSystem/EnvironmentMap.h>

namespace GP
{

	enum class RENDERMODE
	{
		AGD = 0,
		FLAT = 1,
		GC = 2,
		QUALITY = 3,
		SMOOTH = 4	
	};

	struct RenderSpecs
	{

		RENDERMODE renderMode = RENDERMODE::SMOOTH;


		bool fill = true;
		bool line = false;
		float lineDisplacement = 0.5f;

		glm::vec4 lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		bool point;
		glm::vec4 pointColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		float roughness = 0.25f;
		float metalness = 0.35f;
		glm::vec3 albedo = glm::vec3(1.0, 1.0, 1.0);

		bool backfaceCulling = true;
		bool showSamples = false;

	};

	struct VertexNode
	{
		uint32_t index;
		uint32_t prevIndex = -1;
		bool seen = false;
		bool visited = false;
		float shortestPathEstimate = std::numeric_limits<float>::max();
	};


	class Compare
	{
	public:
		bool operator() (VertexNode* lhs, VertexNode* rhs)
		{
			return lhs->shortestPathEstimate > rhs->shortestPathEstimate;
		}
	};

	struct Triangle
	{
		uint32_t idx1;
		uint32_t idx2;
		uint32_t idx3;

		float CalculateArea(const std::vector<glm::vec3>& vertexList)
		{
			glm::vec3 v1 = vertexList[idx1];
			glm::vec3 v2 = vertexList[idx2];
			glm::vec3 v3 = vertexList[idx3];

			return 0.5 * std::abs(glm::length(glm::cross(v2-v1, v3-v1)));
		}

		float CalculateCircumRadius(const std::vector<glm::vec3>& vertexList)
		{
			float area = CalculateArea(vertexList);

			glm::vec3 v1 = vertexList[idx1];
			glm::vec3 v2 = vertexList[idx2];
			glm::vec3 v3 = vertexList[idx3];

			float le1 = glm::length(v1 - v2);
			float le2 = glm::length(v1 - v3);
			float le3 = glm::length(v2 - v3);

			return (le1 * le2 * le3) / (4 * area);
		}

		float CalculateQuality(const std::vector<glm::vec3>& vertexList)
		{
			float circumRadius = CalculateCircumRadius(vertexList);

			glm::vec3 v1 = vertexList[idx1];
			glm::vec3 v2 = vertexList[idx2];
			glm::vec3 v3 = vertexList[idx3];

			float le1 = glm::length(v1 - v2);
			float le2 = glm::length(v1 - v3);
			float le3 = glm::length(v2 - v3);

			float minEdge = std::min(le1, std::min(le2, le3));

			return circumRadius / minEdge;
		}

		glm::vec3 GiveNormal(const std::vector<glm::vec3>& vertexList)
		{

			const float EPSILON = 0.000001f;
			glm::vec3 normal(0.0f);

			glm::vec3 v1 = vertexList[idx1];
			glm::vec3 v2 = vertexList[idx2];
			glm::vec3 v3 = vertexList[idx3];
			

			glm::vec3 e1 = v2 - v1;
			glm::vec3 e2 = v3 - v1;

			normal = glm::cross(e1, e2);
			if (glm::length(normal) > EPSILON)
			{
				normal = glm::normalize(normal);
				return normal;
			}

			return glm::vec3(0.0f);
		}
	};


	// This struct will be used for coloring
	struct ColoringVertex
	{
		glm::vec3 Pos;
		glm::vec3 Normal;
		glm::vec3 Color;
	};


	// This mesh is for assignments for Digital Geometry Course
	class EditorMesh : public Mesh
	{
	public:
		EditorMesh::EditorMesh() {}
		EditorMesh(Ref<Model> model);
		EditorMesh(std::string name, const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);
		static Ref<EditorMesh> Create(Ref<Model> model);
		static Ref<EditorMesh> Create(std::string name, const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);


		Ref<Mesh> GetMainMesh();
		Ref<Line> GetLine();
		Ref<Icosphere> GetSphere();

		glm::vec4 m_LineColor = {1.0f, 0.2f, 0.4f, 1.0f};

		// 0 -> min heap
		// 1 -> vector (array)
		int m_GeodesicDistanceCalcMethod = 0;
		std::string GiveCalcMethodName();
		std::string GiveRenderMethodName();

		// These points are for visualizing geodesic distance
		// between two vertices
		int m_StartIndex = -1;
		int m_EndIndex = -1;

		// Make two spheres maybe?
		glm::vec3 m_StartPoint;
		glm::vec3 m_EndPoint;

		bool m_ShowLine = false;

		// geodesicDistanceCalcTime
		float m_CalcTime = 0.0f;

		void SetupLineVertices();

		void ExportNxNGeodesicDistanceMatrix();
		void ComputeNxNGeodesicDistanceMatrix();
		void ComputeGeodesicDistances(uint32_t index);
		void ComputeGeodesicDistancesMinHeap(uint32_t index);
		void ComputeGeodesicDistancesMinHeapExport(uint32_t index);
		void ComputeGeodesicDistancesVector(uint32_t index);


	public:
		Ref<Shader> m_MainShader;
		Ref<Shader> m_ColorShader;
		Ref<Shader> m_SingleColorShader;

	public:
		void Draw(Ref<Shader> mainShader,
			      Ref<Shader> colorShader,
			      Ref<Shader> singleColorShader,
			      Ref<EnvironmentMap> envMap,
				  uint32_t ditheringTex) const;
		void SmoothingFunction();

		std::future<void> ExportGDM();
	public:
		RenderSpecs m_RenderSpecs;
	public:

		std::vector<glm::vec3> m_GaussianCurvatureColors;
		std::vector<glm::vec3> m_AverageGeodesicDistanceColors;

		// This is going to be flat
		std::vector<glm::vec3> m_QualityColors;

		std::vector<uint32_t> m_SamplePoints;

		glm::vec3 GetVertex(uint32_t id);
	private:
		void CalculateAGDColors();
		void CalculateGaussianCurvatureColors();
		void CalculateQualityColors();

		void CalculateColors();

		void SetupColoringBuffers();
	private:
		bool isSmooth = false;

	private:
		std::vector<Triangle> m_Triangles;

	private:


		// VBO's and VBA's for each coloring mode have separated
		// They will only be computed only in the beginning and 
		// when we use the smooth function
		std::vector<ColoringVertex> m_AGDArrayBuffer;
		Ref<VertexArray> m_AGDVertexArray;
		Ref<VertexBuffer> m_AGDVertexBuffer;
		Ref<IndexBuffer> m_AGDIndexBuffer;

		std::vector<ColoringVertex> m_GCArrayBuffer;
		Ref<VertexArray> m_GCVertexArray;
		Ref<VertexBuffer> m_GCVertexBuffer;
		Ref<IndexBuffer> m_GCIndexBuffer;

		std::vector<ColoringVertex> m_QualityArrayBuffer;
		Ref<VertexArray> m_QualityVertexArray;
		Ref<VertexBuffer> m_QualityVertexBuffer;
		Ref<IndexBuffer> m_QualityIndexBuffer;

	private:
		std::vector<glm::vec3> m_FlatShadeVertices;
		std::vector<glm::vec3> m_FlatShadeNormals;
		std::vector<glm::vec3> m_FlatShadeTangents;
		std::vector<glm::vec3> m_FlatShadeBitangents;
		std::vector<glm::vec2> m_FlatShadeTexCoords;

		std::vector<uint32_t> m_FlatShadeIndices;

	private:
		float m_SmootingFactor = 0.1f;
	private:
		void SetupArrayBufferForColoring(std::vector<ColoringVertex>& coloringArrayBuffer,
			                             const std::vector<glm::vec3>& colors);
		void SetupArrayBufferForFlatColoring(std::vector<ColoringVertex>& coloringArrayBuffer,
										  const std::vector<glm::vec3>& colors);


		void SetupMeshForColoring(Ref<VertexArray>& coloringVertexArray,
			                      Ref<VertexBuffer>& coloringVertexBuffer,
			                      std::vector<ColoringVertex>& coloringArrayBuffer,
			                      Ref<IndexBuffer>& coloringIndexBuffer);

		void SetupMeshForFlatColoring(Ref<VertexArray>& coloringVertexArray,
									  Ref<VertexBuffer>& coloringVertexBuffer,
									  std::vector<ColoringVertex>& coloringArrayBuffer,
									  Ref<IndexBuffer>& coloringIndexBuffer);


		void SetupFlatElements();
		void SetupTriangles();

		void CalculateSmoothNormals();

		std::vector<Triangle> GiveTrianglesWithVertex(uint32_t index);

		std::vector<uint32_t> SampleNPoints(uint32_t sampleCount);

		float GiveGeodesicDistanceBetweenVertices(uint32_t idx1, uint32_t idx2);
	private:
		Ref<Icosphere> m_Sphere;
		ModelMesh m_MainMesh;
		Ref<Line> m_Line;
	private:
		std::priority_queue<VertexNode*, std::vector<VertexNode*>, Compare> m_MinHeap;
		std::priority_queue<VertexNode*, std::vector<VertexNode*>, Compare> m_MinHeapExport;

		std::vector<std::vector<float>> m_NxNGeodesicDistanceMatrix;
		std::unordered_map<uint32_t, std::vector<uint32_t>> m_AdjacencyMap;
		std::unordered_map<uint32_t, VertexNode> m_NodeTable;
		std::unordered_map<uint32_t, VertexNode> m_NodeTableExport;
		std::vector<VertexNode*> m_Vector;


	private:
		virtual void BuildVertices() override;
		void BuildVerticesNoMesh();
		void SetupAdjacencyMap();
		void SetupNodeTable();
		void SetupNodeTableExport();
		void ClearNodeTable();
		void ClearNodeTableExport();

	private:
		int m_CoreSize;
		std::atomic<int> m_Count;
		std::vector<std::future<void>> m_FutureVector;
		std::mutex m_ProgressLock;
		std::mutex m_Mutex;
		std::condition_variable m_WaitResults;
	};
}
#pragma once

#include <GeoProcess/System/RenderSystem/VertexArray.h>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <GeoProcess/System/Geometry/Mesh.h>
#include <GeoProcess/System/Geometry/Model.h>
#include <GeoProcess/System/Geometry/Line.h>

#include <unordered_map>

#include <limits>

namespace GP
{

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


	// A general purpose 
	class EditorMesh : public Mesh
	{
	public:
		EditorMesh::EditorMesh() {}
		EditorMesh(Ref<Model> model);
		static Ref<EditorMesh> Create(Ref<Model> model);

		Ref<Mesh> GetMainMesh();
		Ref<Line> GetLine();
		glm::vec4 m_LineColor = {1.0f, 0.2f, 0.4f, 1.0f};

		// 0 -> min heap
		// 1 -> vector (array)
		int m_GeodesicDistanceCalcMethod = 0;
		std::string GiveCalcMethodName();

		// These points are for visualizing geodesic distance
		// between two vertices
		int m_StartIndex = -1;
		int m_EndIndex = -1;

		bool m_ShowLine = false;

		// geodesicDistanceCalcTime
		float m_CalcTime = 0.0f;

		void SetupLineVertices();


		
	private:
		ModelMesh m_MainMesh;
		Ref<Line> m_Line;

		std::unordered_map<uint32_t, std::vector<uint32_t>> m_AdjacencyMap;
		std::unordered_map<uint32_t, VertexNode> m_NodeTable;
		std::vector<std::vector<float>> m_NxNGeodesicDistanceMatrix;
		std::priority_queue<VertexNode*, std::vector<VertexNode*>, Compare> m_MinHeap;
		std::vector<VertexNode*> m_Vector;
		virtual void BuildVertices() override;

		void SetupAdjacencyMap();
		void SetupNodeTable();
		void ClearNodeTable();
		


		void ComputeGeodesicDistances(uint32_t index);
		void ComputeGeodesicDistancesMinHeap(uint32_t index);
		void ComputeGeodesicDistancesVector(uint32_t index);

		void ComputeNxNGeodesicDistanceMatrix();
		void ExportNxNGeodesicDistanceMatrix();
	};
}
#pragma once

#include <GeoProcess/System/Geometry/ModelDatabase.h>
#include <MeshOperations/EditorMesh.h>

#include <vector>

#include <Eigen/Core>
#include <Spectra/SymEigsSolver.h>

namespace GP
{
	class PCADatabase
	{
	public:
		PCADatabase(Ref<ModelDatabase> modelDB);
		~PCADatabase();

		static Ref<PCADatabase> Create(Ref<ModelDatabase> modelDB);

		Ref<EditorMesh> GetEditorMesh();

		void CalculateNewVertices();

		float* GetCoeff(uint32_t index);

		uint32_t GetCoeffSize();
	private:

		Eigen::MatrixXd CalculateMeanVertices();
		Eigen::MatrixXd ConstructYMatrix(Eigen::MatrixXd mean);


		void SetEditorMesh();

		std::vector<glm::vec3> m_MeanVertices;
		std::vector<glm::vec3> m_Vertices;

		std::vector<uint32_t> m_Indices;

		Ref<ModelDatabase> m_ModelDatabase;
		Ref<EditorMesh> m_EditorMesh;

		uint32_t m_VertexSize;

		Eigen::MatrixXd m_Mean;
		std::vector<Eigen::MatrixXd> m_Eigenvectors;
		std::vector<float> m_Coeffs;

	};
}
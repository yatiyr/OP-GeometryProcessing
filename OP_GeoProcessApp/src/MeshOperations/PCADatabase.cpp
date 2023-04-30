#include <Precomp.h>
#include <MeshOperations/PCADatabase.h>

namespace GP
{
	PCADatabase::PCADatabase(Ref<ModelDatabase> modelDB) : m_ModelDatabase(modelDB)
	{
		GP_TRACE("Size of database is: {0}", m_ModelDatabase->GetMeshCount());
		m_Indices = m_ModelDatabase->GetIndices();

		// Get mean vector
		GP_TRACE("Mean Vector Calculation");
		Eigen::MatrixXd mean = CalculateMeanVertices();
		m_Mean = mean;

		// Get Y Matrix
		GP_TRACE("Y matrix Calculation");
		Eigen::MatrixXd yMatrix = ConstructYMatrix(mean);

		// Karhunen-Loeve Transform
		GP_TRACE("Scatter Calculation");
		Eigen::MatrixXd kScatter = yMatrix.transpose() * yMatrix;

		// Construct matrix operation object using the wrapper class DenseSymMatProd
		Spectra::DenseSymMatProd<double> op(kScatter);

		GP_TRACE("EigenValue Calc");
		int meshCount = m_ModelDatabase->GetMeshCount();
		Spectra::SymEigsSolver<Spectra::DenseSymMatProd<double>> eigs(op, meshCount - 2,
			                                                              meshCount);
		eigs.init();
		int nconv = eigs.compute(Spectra::SortRule::LargestAlge);

		// Retrieve results
		Eigen::VectorXd evalues;
		Eigen::MatrixXd eigenVectors;
		if (eigs.info() == Spectra::CompInfo::Successful)
		{
			evalues = eigs.eigenvalues();
			eigenVectors = eigs.eigenvectors();
		}

		float evalueSum = evalues.sum();

		std::cout << "Eigenvalues found:\n" << evalues << std::endl;
		std::cout << "Eigenvectors\n" << eigenVectors.rows() << " " << eigenVectors.cols() << std::endl;


		// Now compute real eigenvectors
		float totalEvalue = 0.0f;
		for (uint32_t i = 0; i < eigenVectors.cols(); i++)
		{
			float currentEvalue = evalues(i);
			totalEvalue += currentEvalue;

			if (totalEvalue / evalueSum >= 0.9)
				break;

			m_Eigenvectors.push_back(yMatrix * eigenVectors.col(i));
			m_Coeffs.push_back(0.0f);
		}

		GP_TRACE("MeanSize is {0},{1}", mean.rows(), mean.cols());
		GP_TRACE("YmatrixSize is {0},{1}", yMatrix.rows(), yMatrix.cols());


		// Calculate new vertices
		Eigen::MatrixXd x = m_Mean;
		for (uint32_t i = 0; i < m_Eigenvectors.size(); i++)
		{
			x += m_Eigenvectors[i] * m_Coeffs[i];
		}

		for (uint32_t i = 0; i < x.rows(); i+=3)
		{
			m_Vertices.push_back(glm::vec3(x[i], x[i + 1], x[i + 2]));
		}

		// Construct mesh
		m_EditorMesh = EditorMesh::Create("Test", m_Vertices, m_Indices);

		GP_TRACE("Mesh Has been constructed");
	}

	PCADatabase::~PCADatabase()
	{

	}

	Ref<PCADatabase> PCADatabase::Create(Ref<ModelDatabase> modelDB)
	{
		return std::make_shared<PCADatabase>(modelDB);
	}

	std::vector<float>& PCADatabase::GetCoeffs()
	{
		return m_Coeffs;
	}

	Eigen::MatrixXd PCADatabase::CalculateMeanVertices()
	{
		m_VertexSize = m_ModelDatabase->GetVertexCount();

		// Declare a column matrix for the mean
		Eigen::MatrixXd m = Eigen::MatrixXd::Zero(m_VertexSize * 3, 1);

		m_MeanVertices.resize(m_VertexSize, glm::vec3(0.0f));
		
		uint32_t sampleCount = m_ModelDatabase->GetMeshCount();

		std::vector<MeshBlueprint> meshes = m_ModelDatabase->GetMeshes();

		

		for (uint32_t i = 0; i < sampleCount; i++)
		{

			for (uint32_t j = 0; j < m_VertexSize; j++)
			{

				m(j*3, 0)     += meshes[i].vertices[j].x;
				m(j*3 + 1, 0) += meshes[i].vertices[j].y;
				m(j*3 + 2, 0) += meshes[i].vertices[j].z;
				m_MeanVertices[j] += meshes[i].vertices[j];
			}
		}

		for (uint32_t i = 0; i < sampleCount; i++)
		{
			m_MeanVertices[i] /= sampleCount;
		}

		m = m / sampleCount;

		GP_TRACE("Mean Vertex Calculation has finished");

		return m;

	}

	Eigen::MatrixXd PCADatabase::ConstructYMatrix(Eigen::MatrixXd mean)
	{
		uint32_t sampleCount = m_ModelDatabase->GetMeshCount();

		Eigen::MatrixXd yMatrix(m_VertexSize * 3, sampleCount);

		std::vector<MeshBlueprint> meshes = m_ModelDatabase->GetMeshes();

		for (uint32_t i = 0; i < sampleCount; i++)
		{
			for (uint32_t j = 0; j < m_VertexSize; j++)
			{
				yMatrix(j * 3, i)     = meshes[i].vertices[j].x;
				yMatrix(j * 3 + 1, i) = meshes[i].vertices[j].y;
				yMatrix(j * 3 + 2, i) = meshes[i].vertices[j].z;
			}

			yMatrix.col(i) -= mean;
		}

		return yMatrix;
	}

	void PCADatabase::SetEditorMesh()
	{

	}


}
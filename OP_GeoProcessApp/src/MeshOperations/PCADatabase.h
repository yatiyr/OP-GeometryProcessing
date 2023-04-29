#pragma once

#include <GeoProcess/System/Geometry/ModelDatabase.h>
#include <MeshOperations/EditorMesh.h>


namespace GP
{
	class PCADatabase
	{
	public:
		PCADatabase(Ref<ModelDatabase> modelDB);
		~PCADatabase();


	private:
		Ref<ModelDatabase> m_ModelDatabase;
		Ref<EditorMesh> m_EditorMesh;
	};
}
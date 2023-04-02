layout(std140, binding = 0) uniform CubemapCaptureView
{
	mat4 u_CaptureViews[6];
};

layout(std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_View;
	mat4 u_Projection;
	vec3 u_ViewPos;
	float u_Near;
	float u_Far;
};

layout(std140, binding = 2)  uniform Transform
{
	mat4 u_Model;
};



layout(std140, binding = 3) uniform ToneMappingData
{
	float u_Exposure;
	bool u_Hdr;
};


struct BoneMat
{
	mat4 mat;
};

layout(std140, binding = 4) uniform BoneMatricesData
{
	BoneMat u_BoneMatrices[MAX_BONES];
};
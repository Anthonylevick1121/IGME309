#include "MyRigidBody.h"
using namespace BTX;
//Accessors
bool MyRigidBody::GetVisible(void) { return m_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColor(void) { return m_v3Color; }
void MyRigidBody::SetColor(vector3 a_v3Color) { m_v3Color = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void) { return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
vector3 MakeGlobal(matrix4 m4ToWorld, vector3 input) { return m4ToWorld * vector4(input, 1.0); }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix) 
{ 

	if (m_m4ToWorld == a_m4ModelMatrix)
		return;

	m_m4ToWorld = a_m4ModelMatrix; 

	std::vector<vector3> cornerList;
	cornerList.push_back(vector3(m_v3MinL.x,m_v3MinL.y,m_v3MinL.z)); //000
	cornerList.push_back(vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z));//001
	cornerList.push_back(vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z));//010
	cornerList.push_back(vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z));//011

	cornerList.push_back(vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z));//100
	cornerList.push_back(vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z));//101
	cornerList.push_back(vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z));//110
	cornerList.push_back(vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MaxL.z));//111

	for (uint i = 0; i < cornerList.size(); i++)
	{
		cornerList[i] = MakeGlobal(m_m4ToWorld, cornerList[i]);
	}
	MyRigidBody oTemp(cornerList);
	m_v3MinG=oTemp.m_v3MinG;
	m_v3MaxG=oTemp.m_v3MaxG;
}
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = ModelManager::GetInstance();
	m_bVisible = true;

	m_fRadius = 0.0f;

	m_v3Color = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bVisible, other.m_bVisible);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3Color, other.m_v3Color);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3MinL, other.m_v3MinL);
	std::swap(m_v3MaxL, other.m_v3MaxL);

	std::swap(m_v3MinG, other.m_v3MinG);
	std::swap(m_v3MaxG, other.m_v3MaxG);

	std::swap(m_v3HalfWidth, other.m_v3HalfWidth);

	std::swap(m_m4ToWorld, other.m_m4ToWorld);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = other.m_pMeshMngr;
	m_bVisible = other.m_bVisible;

	m_fRadius = other.m_fRadius;

	m_v3Color = other.m_v3Color;

	m_v3Center = other.m_v3Center;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;

	m_m4ToWorld = other.m_m4ToWorld;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };

//--- Non Standard Singleton Methods
void MyRigidBody::AddToRenderList(void)
{
	if (!m_bVisible)
		return;
	matrix4 m4Transform = m_m4ToWorld * glm::translate(vector3(m_v3Center));
	m4Transform = m4Transform * glm::scale(vector3(m_fRadius));
	//m_pMeshMngr->AddWireSphereToRenderList(m4Transform, m_v3Color);


	 m4Transform = m_m4ToWorld * glm::translate(vector3(m_v3Center));
	m4Transform = m4Transform * glm::scale(m_v3HalfWidth * 2);
	m_pMeshMngr->AddWireCubeToRenderList(m4Transform, m_v3Color);

	if (m_bVisibleBS)
	{

	}
	else
	{

	}

	if (m_bVisibleAABB)
	{

	}
	else {

	}

	vector3 v3ARBBSize = m_v3MaxG - m_v3MinG;
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(MakeGlobal(m_m4ToWorld,m_v3Center)) * glm::scale(v3ARBBSize), C_YELLOW);
}
vector3 MyRigidBody::GlobalizeVector(vector3 input)
{
	return m_m4ToWorld * vector4(input, 1.0f);
}

bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	
	bool colliding = true;

	vector3 v3thisCenter = this->GlobalizeVector(this->m_v3Center);
	vector3 v3OtherCenter = other->GlobalizeVector(other->m_v3Center);
	float fDistance = glm::distance(v3thisCenter, v3OtherCenter);
	float fRadiiSum = this->m_fRadius + other->m_fRadius;
	colliding = fRadiiSum > fDistance;

	this->m_v3MinG = this->GlobalizeVector(this->m_v3MinL);
	this->m_v3MaxG = this->GlobalizeVector(this->m_v3MaxL);

	other->m_v3MinG = other->GlobalizeVector(other->m_v3MinL);
	other->m_v3MaxG = other->GlobalizeVector(other->m_v3MaxL);

	colliding = true;

	if (m_v3MinG.x>other->m_v3MaxG.x)
	{
		colliding = false;
	}
	if (m_v3MaxG.x < other->m_v3MinG.x)
	{
		colliding = false;
	}

	if (m_v3MinG.y > other->m_v3MaxG.y)
	{
		colliding = false;
	}
	if (m_v3MaxG.y < other->m_v3MinG.y)
	{
		colliding = false;
	}

	if (m_v3MinG.z > other->m_v3MaxG.z)
	{
		colliding = false;
	}
	if (m_v3MaxG.z < other->m_v3MinG.z)
	{
		colliding = false;
	}


	if (colliding)
	{
		this->m_v3Color = vector3(1.0f, 0.0f, 0.0f);
		other->m_v3Color = C_RED;
	}
	return colliding;
}
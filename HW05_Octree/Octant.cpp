#include "Octant.h"
using namespace BTX;
//  Octant
uint Octant::m_uOctantCount = 0;
uint Octant::m_uMaxLevel = 3;
uint Octant::m_uIdealEntityCount = 5;
Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	/*
	* This constructor is meant to be used ONLY on the root node, there is already a working constructor
	* that will take a size and a center to create a new space
	*/
	Init();//Init the default values
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	//create a rigid body that encloses all the objects in this octant, it necessary you will need
	//to subdivide the octant based on how many objects are in it already an how many you IDEALLY
	//want in it, remember each subdivision will create 8 children for this octant but not all children
	//of those children will have children of their own

	std::vector<vector3> lMinMax;

	//vector3 v3Min = a_rTempBody->GetMinGlobal();
	//vector3 v3Max = a_rTempBody->GetMaxGlobal();


	//Loops through every other entity and compares max/min values to default
	//If a greater max/lower min is found, replaces those values
	//for (uint i = 1; i < m_pEntityMngr->GetEntityCount(); i++)
	//{
	//	a_rTempBody = m_pEntityMngr->GetRigidBody(i);
	//	vector3 a_v3TempMin = a_rTempBody->GetMinGlobal();
	//	vector3 a_v3TempMax = a_rTempBody->GetMaxGlobal();
	//	if (a_v3TempMin.x < v3Min.x) { v3Min.x = a_v3TempMin.x; }
	//	if (a_v3TempMin.y < v3Min.y) { v3Min.y = a_v3TempMin.y; }
	//	if (a_v3TempMin.z < v3Min.z) { v3Min.z = a_v3TempMin.z; }
	//	if (a_v3TempMax.x > v3Max.x) { v3Max.x = a_v3TempMax.x; }
	//	if (a_v3TempMax.y > v3Max.y) { v3Max.y = a_v3TempMax.y; }
	//	if (a_v3TempMax.z > v3Max.z) { v3Max.z = a_v3TempMax.z; }
	//}

	//Takes rigidbody of first entity in manager, assigns max/min values of it as a default
	RigidBody* a_rTempBody = m_pEntityMngr->GetRigidBody(0);

	float a_fMin = 0.0f;
	float a_fMax = 0.0f;

	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		a_rTempBody = m_pEntityMngr->GetRigidBody(i);
		vector3 a_v3TempMin = a_rTempBody->GetMinGlobal();
		vector3 a_v3TempMax = a_rTempBody->GetMaxGlobal();
		if (a_v3TempMin.x < a_fMin) { a_fMin = a_v3TempMin.x; }
		if (a_v3TempMin.y < a_fMin) { a_fMin = a_v3TempMin.y; }
		if (a_v3TempMin.z < a_fMin) { a_fMin = a_v3TempMin.z; }
		if (a_v3TempMax.x > a_fMax) { a_fMax = a_v3TempMax.x; }
		if (a_v3TempMax.y > a_fMax) { a_fMax = a_v3TempMax.y; }
		if (a_v3TempMax.z > a_fMax) { a_fMax = a_v3TempMax.z; }
	}

	//Pushes values into lMinMax, then creates new rigidbody of root octant
	lMinMax.push_back(vector3(a_fMin));
	lMinMax.push_back(vector3(a_fMax));
	RigidBody pRigidBody = RigidBody(lMinMax);


	//The following will set up the values of the octant, make sure the are right, the rigid body at start
	//is NOT fine, it has made-up values
	m_fSize = pRigidBody.GetHalfWidth().x * 2.0f;
	m_v3Center = pRigidBody.GetCenterLocal();
	m_v3Min = m_v3Center - pRigidBody.GetHalfWidth();
	m_v3Max = m_v3Center + pRigidBody.GetHalfWidth();

	m_uOctantCount++; //When we add an octant we increment the count
	ConstructTree(m_uMaxLevel); //Construct the children
}

bool Octant::IsColliding(uint a_uRBIndex)
{
	//Get how many objects there are in the world
	//If the index given is larger than the number of elements in the bounding object there is no collision
	//As the Octree will never rotate or scale this collision is as easy as an Axis Alligned Bounding Box
	//Get all vectors in global space (the octant ones are already in Global)

	RigidBody* a_rTempBody = m_pEntityMngr->GetRigidBody(a_uRBIndex);
	vector3 a_v3Min = a_rTempBody->GetMinGlobal();
	vector3 a_v3Max = a_rTempBody->GetMaxGlobal();
	if (a_v3Min.x > m_v3Max.x) return false;
	if (a_v3Min.y > m_v3Max.y) return false;
	if (a_v3Min.z > m_v3Max.z) return false;
	if (a_v3Max.x < m_v3Min.x) return false;
	if (a_v3Max.y < m_v3Min.y) return false;
	if (a_v3Max.z < m_v3Min.z) return false;
	return true; // for the sake of startup code
}
void Octant::Display(uint a_nIndex, vector3 a_v3Color)
{
	// Display the specified octant
	//Get octant first
	//Octant* a_oTemp = m_pEntityMngr->Get
	//m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, a_oTemp->))
	if (a_nIndex == 0)
	{
		m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
			glm::scale(vector3(m_fSize)), a_v3Color);
		return;
	}
	for (uint i = 0; i < 8; i++)
	{
		if (m_pChild[i] != nullptr)
		{
			if (m_pChild[i]->m_uID == a_nIndex)
			{
				m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_pChild[i]->GetCenterGlobal()) *
					glm::scale(vector3(m_pChild[i]->m_fSize)), a_v3Color);
				return;
			}
			else
			{
				m_pChild[i]->Display(a_nIndex, a_v3Color);
			}
		}
	}
}
void Octant::Display(vector3 a_v3Color)
{
	//this is meant to be a recursive method, in starter code will only display the root
	//even if other objects are created

	//Loop through for number of octants
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color);
	for (uint i = 0; i < 8; i++)
	{
		if (m_pChild[i] != nullptr) { m_pChild[i]->Display(a_v3Color); }
	}
}
void Octant::Subdivide(void)
{
	//Ideal count already checked for method
	//If this node has reach the maximum depth return without changes
	if (m_uLevel >= m_uMaxLevel)
		return;

	//If this node has been already subdivided return without changes
	if (m_uChildren != 0)
		return;

	//Subdivide the space and allocate 8 children
	vector3 a_v3NewCenter;
	uint a_uID = m_uID + 1;
	uint a_uLevel = m_uLevel;
	a_uLevel++;

	a_v3NewCenter = vector3((m_v3Center.x + m_fSize / 4), (m_v3Center.y + m_fSize / 4), (m_v3Center.z + m_fSize / 4)); //000
	m_pChild[0] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[0]->m_uLevel = a_uLevel;

	a_v3NewCenter = vector3((m_v3Center.x - m_fSize / 4), (m_v3Center.y + m_fSize / 4), (m_v3Center.z + m_fSize / 4)); //100
	m_pChild[1] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[1]->m_uLevel = a_uLevel;

	a_v3NewCenter = vector3((m_v3Center.x + m_fSize / 4), (m_v3Center.y - m_fSize / 4), (m_v3Center.z + m_fSize / 4)); //010
	m_pChild[2] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[2]->m_uLevel = a_uLevel;

	a_v3NewCenter = vector3((m_v3Center.x + m_fSize / 4), (m_v3Center.y + m_fSize / 4), (m_v3Center.z - m_fSize / 4)); //001
	m_pChild[3] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[3]->m_uLevel = a_uLevel;

	a_v3NewCenter = vector3((m_v3Center.x - m_fSize / 4), (m_v3Center.y - m_fSize / 4), (m_v3Center.z + m_fSize / 4)); //110
	m_pChild[4] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[4]->m_uLevel = a_uLevel;

	a_v3NewCenter = vector3((m_v3Center.x - m_fSize / 4), (m_v3Center.y + m_fSize / 4), (m_v3Center.z - m_fSize / 4)); //101
	m_pChild[5] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[5]->m_uLevel = a_uLevel;

	a_v3NewCenter = vector3((m_v3Center.x + m_fSize / 4), (m_v3Center.y - m_fSize / 4), (m_v3Center.z - m_fSize / 4)); //011
	m_pChild[6] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[6]->m_uLevel = a_uLevel;

	a_v3NewCenter = vector3((m_v3Center.x - m_fSize / 4), (m_v3Center.y - m_fSize / 4), (m_v3Center.z - m_fSize / 4)); //111
	m_pChild[7] = new Octant(a_v3NewCenter, (m_fSize / 2));
	m_pChild[7]->m_uLevel = a_uLevel;

	m_uChildren = 8;

	for (uint i = 0; i < 8; i++)
	{
		if (m_pChild[i]->ContainsAtLeast(m_uIdealEntityCount) && m_pChild[i]->m_uLevel != m_uMaxLevel)
			m_pChild[i]->Subdivide();
	}
}
bool Octant::ContainsAtLeast(uint a_nEntities)
{
	//You need to check how many entity objects live within this octant

	//Loop through each entity in octant, increment number
	//If count ever matches a_nEntities, return true
	uint a_uContains = 0;
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		//Perform check to see if entity is colliding with octant
		if (IsColliding(i))
			a_uContains++;
		if (a_uContains >= a_nEntities)
			return true;
	}
	return false; //return something for the sake of start up code
}
void Octant::AssignIDtoEntity(void)
{
	//Recursive method
	//Have to traverse the tree and make sure to tell the entity manager
	//what octant (space) each object is at
	if (m_pChild[0] != nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			m_pChild[i]->AssignIDtoEntity();
		}
		return;
	}

	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		if (IsColliding(i))
		{
			m_pEntityMngr->AddDimension(i, m_uID);
		}
	}
	for (uint i = 0; i < 8; i++)
	{
		if (m_pChild[i] != nullptr) { m_pChild[i]->AssignIDtoEntity(); }
	}


}
//-------------------------------------------------------------------------------------------------------------------
// You can assume the following is fine and does not need changes, you may add onto it but the code is fine as is
// in the proposed solution.
void Octant::Init(void)
{
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint n = 0; n < 8; n++)
	{
		m_pChild[n] = nullptr;
	}
}
void Octant::Swap(Octant& other)
{
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}
void Octant::Release(void)
{
	//this is a special kind of release, it will only happen for the root
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}
void Octant::ConstructTree(uint a_nMaxLevel)
{
	//If this method is tried to be applied to something else
	//other than the root, don't.
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel; //Make sure you mark which is the maximum level you are willing to reach
	m_uOctantCount = 1;// We will always start with one octant

	//If this was initialized before make sure its empty
	m_EntityList.clear();//Make sure the list of entities inside this octant is empty
	KillBranches();
	m_lChild.clear();

	//If we have more entities than those that we ideally want in this octant we subdivide it
	if (ContainsAtLeast(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDtoEntity();//Add octant ID to Entities
	ConstructList();//construct the list of objects
}
//The big 3
Octant::Octant(vector3 a_v3Center, float a_fSize)
{
	//Will create the octant object based on the center and size but will not construct children
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}
Octant::Octant(Octant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot, other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}
Octant& Octant::operator=(Octant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Octant temp(other);
		Swap(temp);
	}
	return *this;
}
Octant::~Octant() { Release(); };
//Accessors
float Octant::GetSize(void) { return m_fSize; }
vector3 Octant::GetCenterGlobal(void) { return m_v3Center; }
vector3 Octant::GetMinGlobal(void) { return m_v3Min; }
vector3 Octant::GetMaxGlobal(void) { return m_v3Max; }
uint Octant::GetOctantCount(void) { return m_uOctantCount; }
bool Octant::IsLeaf(void) { return m_uChildren == 0; }
Octant* Octant::GetParent(void) { return m_pParent; }
//--- other methods
Octant* Octant::GetChild(uint a_nChild)
{
	//if its asking for more than the 8th children return nullptr, as we don't have one
	if (a_nChild > 7) return nullptr;
	return m_pChild[a_nChild];
}
void Octant::KillBranches(void)
{
	/*This method has recursive behavior that is somewhat hard to explain line by line but
	it will traverse the whole tree until it reaches a node with no children and
	once it returns from it to its parent it will kill all of its children, recursively until
	it reaches back the node that called it.*/
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranches();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}
	m_uChildren = 0;
}
void Octant::DisplayLeaves(vector3 a_v3Color)
{
	/*
	* Recursive method
	* it will traverse the tree until it find leaves and once found will draw them
	*/
	uint nLeafs = m_lChild.size(); //get how many children this will have (either 0 or 8)
	for (uint nChild = 0; nChild < nLeafs; nChild++)
	{
		m_lChild[nChild]->DisplayLeaves(a_v3Color);
	}
	//Draw the cube
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color);
}
void Octant::ClearEntityList(void)
{
	/*
	* Recursive method
	* It will traverse the tree until it find leaves and once found will clear its content
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ClearEntityList();
	}
	m_EntityList.clear();
}
void Octant::ConstructList(void)
{
	/*
	* Recursive method
	* It will traverse the tree adding children
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ConstructList();
	}
	//if we find a non-empty child add it to the tree
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
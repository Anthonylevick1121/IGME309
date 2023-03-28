/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2021/06
----------------------------------------------*/
#ifndef __MYRIGIDBODY_H_
#define __MYRIGIDBODY_H_

#include "BTX\BTX.h"

namespace BTX
{

//System Class
class MyRigidBody
{
	ModelManager* m_pMeshMngr = nullptr; //for displaying the Rigid Body

	bool m_bVisible = true; //Visibility

	float m_fRadius = 0.0f; //Radius

	vector3 m_v3Color = C_WHITE; //Color

	vector3 m_v3Center = vector3(0.0f); //center point in local space
	vector3 m_v3MinL = vector3(0.0f); //minimum coordinate in local space
	vector3 m_v3MaxL = vector3(0.0f); //maximum coordinate in local space

	vector3 m_v3MinG = vector3(0.0f); //minimum coordinate in local space
	vector3 m_v3MaxG = vector3(0.0f); //maximum coordinate in local space

	vector3 m_v3HalfWidth = vector3(0.0f); //half the size of all sides

	matrix4 m_m4ToWorld = IDENTITY_M4; //Matrix that will take us from local to world coordinate

public:
	/*
	Usage: Constructor
	Arguments: std::vector<vector3> a_pointList -> list of points to make the Rigid Body for
	These points are what make up the model, which will be used to calculate points for the
	Rigid body's shape.
	Output: class object instance
	*/
	MyRigidBody(std::vector<vector3> a_pointList);
	/*
	Usage: Copy Constructor
	Arguments: class object to copy
	Output: class object instance
	*/
	MyRigidBody(MyRigidBody const& other);
	/*
	Usage: Copy Assignment Operator
	Arguments: class object to copy
	Output: ---
	*/
	MyRigidBody& operator=(MyRigidBody const& other);
	/*
	Usage: Destructor
	Arguments: ---
	Output: ---
	*/
	~MyRigidBody(void);
	/*
	Usage: Changes object contents for other object's
	Arguments: other -> object to swap content from
	*/
	void Swap(MyRigidBody& other);

	/*
	USAGE: Will render the Rigid Body at the model to world matrix
	*/
	void AddToRenderList(void);

	/*
	USAGE: Tells if the object is colliding with the incoming one
	ARGUMENTS: other -> The Object being tested for collision with current Rigidbody
	OUTPUT: Whether or not the two objects are colliding
	*/
	bool IsColliding(MyRigidBody* const other);
#pragma region Accessors
	/*
	Usage: Gets visibility
	Output: visibility
	*/
	bool GetVisible(void);
	/*
	Usage: Gets radius
	Output: radius
	*/
	float GetRadius(void);
	/*
	Usage: Gets the color
	Output: color
	*/
	vector3 GetColor(void);
	/*
	Usage: Sets the color
	Arguments: vector3 a_v3Color -> color
	*/
	void SetColor(vector3 a_v3Color);
	/*
	Usage: Gets center in local space
	Center will be relative to the points contained in the model,
	and will not include anything from other objects in the world
	Output: center
	*/
	vector3 GetCenterLocal(void);
	/*
	Usage: Gets minimum vector in local space
	Point will be relative to the points contained in the model,
	and will not include anything from other objects in the world
	Output: min vector
	*/
	vector3 GetMinLocal(void);
	/*
	Usage: Gets maximum vector in local space
	Point will be relative to the points contained in the model,
	and will not include anything from other objects in the world
	Output: max vector
	*/
	vector3 GetMaxLocal(void);
	/*
	Usage: Gets center in global space
	Center will be relative to all objects in the world
	Output: center
	*/
	vector3 GetCenterGlobal(void);
	/*
	Usage: Gets minimum vector in local space
	Point will be relative to all objects in the world
	Output: min vector
	*/
	vector3 GetMinGlobal(void);
	/*
	Usage: Gets max vector in global space
	Point will be relative to all objects in the world
	Output: max vector
	*/
	vector3 GetMaxGlobal(void);
	/*
	Usage: Gets the size of the model divided by 2
	Output: halfwidth vector
	This vector will be used to store the half widths in the 
	x-direction, y-direction, and z-direction
	*/
	vector3 GetHalfWidth(void);
	/*
	Usage: Gets Model to World matrix
	Output: The Model Matrix containing information in Global Space
	*/
	matrix4 GetModelMatrix(void);
	/*
	Usage: Sets Model to World matrix
	Arguments: The Model Matrix containing information in Global Space
	*/
	void SetModelMatrix(matrix4 a_m4ModelMatrix);
#pragma endregion
	
private:
	/*
	Usage: Deallocates member fields
	Arguments: ---
	Output: ---
	*/
	void Release(void);
	/*
	Usage: Allocates member fields
	Arguments: ---
	Output: ---
	*/
	void Init(void);

	vector3 GlobalizeVector(vector3 input);
};//class

} //namespace BTX

#endif //__MYRIGIDBODY_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/
#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] = {PxVec3(46.f/255.f,9.f/255.f,39.f/255.f),PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),PxVec3(4.f/255.f,117.f/255.f,111.f/255.f)};

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};

	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box* box;
		Sphere** spheres;
		CompoundObject* compoundObj, *compoundObj2;
		Box** boxTower;

	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			PxReal degree2Rad = PxPi / 180.0f;

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane);

			box = new Box(PxTransform(PxVec3(.0f,10.f,0.0f)), PxVec3(5.0f, .5f, 0.5f));
			box->Color(color_palette[0]);
			Add(box);

			spheres = new Sphere*[2];

			spheres[0] = new Sphere(PxTransform(-1.0f, 0.5f, 0.0f));
			spheres[0]->Color(PxVec3(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f)); // cornflower blue ftw
			Add(spheres[0]);

			spheres[1] = new Sphere(PxTransform(1.0f, 0.5f, 0.0f));
			spheres[1]->Color(PxVec3(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f)); // cornflower blue ftw
			Add(spheres[1]);

			compoundObj = new TwinBoxesCompoundObject(PxTransform(0.0f, 1.0f, 2.0f, PxQuat(45.0f * degree2Rad, PxVec3(0.f,1.f,0.f)) * PxQuat(90.0f * degree2Rad, PxVec3(0.f, 0.f, 1.f))));
			compoundObj->Color(PxVec3(0.0f, 1.0f, 0.0f));
			Add(compoundObj);

			compoundObj2 = new RotatedCapsulesCompoundObject(4, PxVec3(0.f, 1.f, 0.f), PxTransform(PxVec3(0.f, 1.f, 5.f)));
			Add(compoundObj2);

			int towerHeight = 20;
			boxTower = new Box*[towerHeight];
			for (int i = 0; i < towerHeight; i++)
			{
				PxQuat orientation = PxQuat((45.0f + (float)i * 2) * degree2Rad, PxVec3(0.f, 1.f, 0.f))/* * PxQuat(45.0f * degree2Rad, PxVec3(0.f, 0.f, 1.f))*/;
				boxTower[i] = new Box(PxTransform(-10.f, 0.5f + 1.0f * (float)i, 0.0f, orientation));
				boxTower[i]->Color(PxVec3(1.0f, 1.0f, 0.0f));
				Add(boxTower[i]);
			}
		}

		//Custom udpate function
		virtual void CustomUpdate(float dTime) 
		{
			if (pause)
				return;

			PxReal degree2Rad = PxPi / 180.0f;
			PxTransform currentPose = ((PxRigidActor*)compoundObj->Get())->getGlobalPose();
			float yAngle = 0.f;
			if (currentPose.q.w <= -1.0f + 0.000001f)
				currentPose.q.w = 1.0f;
			currentPose.q.toRadiansAndUnitAxis(yAngle, PxVec3(0.f, 1.f, 0.f));
			PxQuat newOrientation = PxQuat((yAngle + 10.f*degree2Rad*dTime), PxVec3(0.f,1.f,0.f));
			((PxRigidActor*)compoundObj->Get())->setGlobalPose(PxTransform(currentPose.p, newOrientation));
		}
	};
}

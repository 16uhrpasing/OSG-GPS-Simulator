#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osgUtil/ShaderGen>
#include <osg/TexGen>

#include <osgViewer/Viewer>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/TrackballManipulator>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osg/Math>
#include <osg/io_utils>
#include <osg/MatrixTransform>

#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/UpdateMatrixTransform>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedQuaternionElement>

#include <Skybox.h>
#include <MyManipulator.h>
#include <PickHandler.h>
#include <UniverseObjects.h>
#include <Animator.h>
#include <SatelliteTracker.h>

#include <tuple>

#include <osg/AnimationPath>

#include <stdexcept>




int main(int argc, char** argv)
{
	//osg::ref_ptr<osg::MatrixTransform> sun_node = createSunGeode();
	osg::ref_ptr<osg::MatrixTransform> earth_node = createEarthGeode();
	

	osg::ref_ptr<osg::AnimationPathCallback> apcb = new
		osg::AnimationPathCallback;
	apcb->setAnimationPath(createAnimationPath(osg::Vec3(0.f,0.f,0.f), 18.0f));

	//satellite_ring_template->setUpdateCallback(apcb.get());


	std::vector<osg::Matrix> x_rotations;
	osg::Matrix xFormationOne = osg::Matrix::rotate((45) * PI / 180, osg::Z_AXIS) * osg::Matrix::rotate((45) * PI / 180, osg::X_AXIS);
	osg::Matrix xFormationTwo = osg::Matrix::rotate((-45) * PI / 180, osg::X_AXIS);

	osg::Matrix moveZ = osg::Matrix::rotate((90) * PI / 180, osg::Y_AXIS);
	osg::Matrix moveY = osg::Matrix::rotate((90) * PI / 180, osg::Z_AXIS);
	//zrot for no collision between satellites
	x_rotations.push_back(xFormationOne); //worldMat0
	x_rotations.push_back(xFormationTwo);
	x_rotations.push_back(xFormationOne*moveZ); //2
	x_rotations.push_back(xFormationTwo*moveZ);
	x_rotations.push_back(xFormationOne*moveY); //4 etc.
	x_rotations.push_back(xFormationTwo*moveY);


	//std::vector<osg::Matrix> satellite_system_rotations;
	//satellite_system_rotations.push_back(osg::Matrix::rotate(0, osg::X_AXIS));
	//satellite_system_rotations.push_back(osg::Matrix::rotate((90) * PI / 180, osg::Y_AXIS));
	//satellite_system_rotations.push_back(osg::Matrix::rotate((90) * PI / 180, osg::Z_AXIS));

	//instanceSatelliteSystem(x_system, satellite_system_rotations);

	

	osg::ref_ptr<osg::Group> root = new osg::Group;
	//root->addChild(sun_node);
	root->addChild(earth_node);
	root->addChild(createSpaceSkyBoxGeode());
	//root->addChild(instanceOne);
	//root->addChild(instanceTwo);
	//root->addChild(instanceOne);
	//root->addChild(instanceOne);
	std::vector<osg::PositionAttitudeTransform*> satRefs;

	for (int i = 0; i < x_rotations.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> satellite_ring = createSatelliteRing(4, "ring" + std::to_string(i), x_rotations.at(i), apcb, satRefs);
		root->addChild(satellite_ring.release());
	}
	std::cout << "satRefCount " << satRefs.size() << std::endl;
	int satelliteCount = satRefs.size();

	osg::ref_ptr<osg::ShapeDrawable> beamGeometry = new osg::ShapeDrawable;
	beamGeometry->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f),
		0.05f, satellite_distance));
	beamGeometry->setNodeMask(~0x5);

	osg::ref_ptr<osg::MatrixTransform> beamRotation = new osg::MatrixTransform;
	beamRotation->addChild(beamGeometry);
	beamRotation->setMatrix(osg::Matrix::rotate(90. * PI/180., osg::Z_AXIS));

	osg::ref_ptr<osg::MatrixTransform> beam = new osg::MatrixTransform;

	beam->addChild(beamRotation);


	osg::ref_ptr<PickHandler> picker = new PickHandler;
	root->addChild(picker->getPickNode());
	root->addChild(picker->getTangentPlane());
	root->addChild(beam);
	//instanceSatelliteSystem(satellite_ring_template, x_rotations, root);

	osg::ref_ptr<MyManipulator> myManipulator = new MyManipulator;
	myManipulator->setHomePosition(osg::Vec3(12.5, 0 , 0),
		osg::Vec3(), osg::Vec3f(0.f,-1.0f,0.f));
	myManipulator->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
	myManipulator->setRotationMode(
		osgGA::NodeTrackerManipulator::TRACKBALL);
	myManipulator->setTrackNode(earth_node);

	myManipulator->setRestrictDistance(false);
	myManipulator->setMinMaxDistance(3, 40.0);


	osgViewer::Viewer viewer;
	viewer.setSceneData(root);
	viewer.setCameraManipulator(myManipulator);
	viewer.addEventHandler(picker.get());
	viewer.setUpViewInWindow(0, 0, 1000, 1000);

	std::cout << viewer.getCamera()->getNearFarRatio() << std::endl;

	//manager->playAnimation(animation.get());

	//SatelliteTracker satelliteTracker;
	//root->accept(satelliteTracker);
	//std::cout << "satellite count: " << satelliteTracker.getSatelliteCount() << std::endl;
	
	//std::vector<osg::MatrixTransform*> satellites = satelliteTracker.getSatellites();
	//satelliteTracker.initializeMatrixHeap();
	//satelliteTracker.calculateMatrixHeap(4);
	//int satelliteCount = satelliteTracker.getSatelliteCount();

	/*std::vector<bool> isSatelliteAbove;
	for (int i = 0; i < satelliteCount; i++) {
		isSatelliteAbove.push_back(false);
	}*/

	osg::ref_ptr<osg::PolygonMode> fillMode = new osg::PolygonMode;
	fillMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
		osg::PolygonMode::FILL);

	osg::ref_ptr<osg::PolygonMode> pointMode = new osg::PolygonMode;
	pointMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
		osg::PolygonMode::POINT);

	osg::Vec3d pos;
	osg::Quat rot;
	osg::Vec3d scale;
	osg::Quat so;

	float frameCount = 0;
	while (!viewer.done())
	{
		//satelliteTracker.calculateMatrixHeap(4);
		//std::cout << satelliteTracker.getDeconstructedMatrixAt(0).trans << std::endl;
		
		if (picker->isPicked())
		{
		
			for (int i = 0; i < satelliteCount; i++)
			{
				satRefs.at(i)->getWorldMatrices().at(0).decompose(pos, rot, scale, so);
				bool isAbove = picker->getPickPlane()->insertPlaneEquation(pos) > 0.0;
				if (isAbove) {
					satRefs.at(i)->getOrCreateStateSet()->setAttribute(fillMode);
				}
				else {
					satRefs.at(i)->getOrCreateStateSet()->setAttribute(pointMode);
				}
			}
			
			/*satellite_ring_template->setMatrix(osg::Matrix::rotate((frameCount)*PI / 180, osg::Y_AXIS));

			for (int i = 0; i < satelliteCount; i++) {
				osg::Vec3 currentSatellitePos = satelliteTracker.getDeconstructedMatrixAt(i).pos;

				//std::cout << i << ": " << picker->getPickPlane()->insertPlaneEquation(currentSatellitePos) << std::endl;
				bool isAbove = picker->getPickPlane()->insertPlaneEquation(currentSatellitePos) > 0.0;
				if (isSatelliteAbove.at(i) != isAbove) {
					isSatelliteAbove.at(i) = isAbove;
					std::cout << "satellite " << i << " is above: " << isAbove << std::endl;

					if (isAbove) {
						satellites.at(i)->getOrCreateStateSet()->setAttribute(fillMode);
					}
					else {
						satellites.at(i)->getOrCreateStateSet()->setAttribute(pointMode);
					}

				}
					
			

			}*/
			//osg::Vec3 satellitePos = satelliteTracker.getDeconstructedMatrixAt(0).pos;
			//osg::Vec3 satellitePosOne = satelliteTracker.getDeconstructedMatrixAt(1).pos;
			//std::cout << "pos1: " << satellitePos << " equation: " << picker->getPickPlane()->insertPlaneEquation(satellitePos) << std::endl;
			//std::cout << "pos2: " << satellitePosOne << " equation: " << picker->getPickPlane()->insertPlaneEquation(satellitePosOne) << std::endl;

			//std::cout << frameCount << std::endl;

			//beam->setMatrix(osg::Matrix::scale(1.0, 1.0, 1.0) *
			//	osg::Matrix::rotate(frameCount / 10.0, osg::Z_AXIS) *
			//	osg::Matrix::translate(satellitePos/2.));

			/*for (int i = 0; i < satelliteCount; i++)
			{
				osg::Vec3 satellitePos = satelliteTracker.getDeconstructedMatrixAt(i).pos;
				bool isSatelliteAbove = picker->getPickPlane()->planeEquation(satellitePos) > 0;
				if (isSatelliteAbove) {
					osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode;
					pm->setMode(osg::PolygonMode::FRONT_AND_BACK,
						osg::PolygonMode::FILL);

					satellites.at(i)->getOrCreateStateSet()->setAttribute(pm.get());
				}
				else {
					osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode;
					pm->setMode(osg::PolygonMode::FRONT_AND_BACK,
						osg::PolygonMode::POINT);

					satellites.at(i)->getOrCreateStateSet()->setAttribute(pm.get());
				}
			}*/
				
		}

		frameCount++;
		viewer.frame();
	}
	picker->getPickPlane()->printPlaneEquation();
	//satelliteTracker.deleteMatrixHeap();

	return 0;
}
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
	osg::ref_ptr<osg::MatrixTransform> earth_node = createEarthGeode();
	

	osg::ref_ptr<osg::AnimationPathCallback> apcb = new
		osg::AnimationPathCallback;
	apcb->setAnimationPath(createAnimationPath(osg::Vec3(0.f,0.f,0.f), 18.0f));

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

	

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(earth_node);
	root->addChild(createSpaceSkyBoxGeode());

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
			
		}

		frameCount++;
		viewer.frame();
	}
	picker->getPickPlane()->printPlaneEquation();

	return 0;
}
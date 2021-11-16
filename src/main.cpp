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

#include <osgText/Font>
#include <osgText/Text>

#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/UpdateMatrixTransform>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedQuaternionElement>

#include <Skybox.h>
#include <MyManipulator.h>
#include <PickHandler.h>
#include <UniverseObjects.h>
#include <Animator.h>
#include <GPSTracker.h>
#include <InfoRenderer.h>

#include <tuple>

#include <osg/AnimationPath>

#include <stdexcept>

#include "wtypes.h"

osg::ref_ptr<PickHandler> pickHandler;
GPSTracker* gpstracker;
InfoRenderer* infoRenderer;

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void onPlaneChanged(std::string input) {;
	infoRenderer->setPlaneEq(input);
}

void onLatLonChanged(std::string input) {
	infoRenderer->setLocation(input);
}

void onVisibleSatellitesChanged(int count)
{
	infoRenderer->setVisibleSatellites(count);
}

int main(int argc, char** argv)
{
	//Earth geometry
	osg::ref_ptr<osg::MatrixTransform> earth_node = createEarthGeode();

	//Rotation animation for a satellite ring
	osg::ref_ptr<osg::AnimationPathCallback> apcb = new
		osg::AnimationPathCallback;
	apcb->setAnimationPath(createAnimationPath(osg::Vec3(0.f,0.f,0.f), 18.0f));

	//Rotations for some satellite rings (6x4sats)
	std::vector<osg::Matrix> x_rotations;
	osg::Matrix xFormationOne = osg::Matrix::rotate((255) * PI / 180, osg::Z_AXIS) * osg::Matrix::rotate((45) * PI / 180, osg::X_AXIS);
	osg::Matrix xFormationTwo = osg::Matrix::rotate((-45) * PI / 180, osg::X_AXIS);
	//z-rotation for no collision between satellites
	osg::Matrix moveZ = osg::Matrix::rotate((90) * PI / 180, osg::Y_AXIS);
	osg::Matrix moveY = osg::Matrix::rotate((90) * PI / 180, osg::Z_AXIS);

	x_rotations.push_back(xFormationOne); //worldMat0
	x_rotations.push_back(xFormationTwo);
	x_rotations.push_back(xFormationOne*moveZ); //2
	x_rotations.push_back(xFormationTwo*moveZ);
	x_rotations.push_back(xFormationOne*moveY); //4 etc.
	x_rotations.push_back(xFormationTwo*moveY);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(earth_node);
	root->addChild(createSpaceSkyBoxGeode());

	//Satellite references
	std::vector<osg::PositionAttitudeTransform*> satRefs;

	//Create the satellite rings
	for (int i = 0; i < x_rotations.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> satellite_ring = createSatelliteRing(4, "ring" + std::to_string(i), x_rotations.at(i), apcb, satRefs);
		root->addChild(satellite_ring.release());
	}
	std::cout << "Number of satellites: " << satRefs.size() << std::endl;
	int satelliteCount = satRefs.size();

	//Add a pick handler
	pickHandler = new PickHandler;
	root->addChild(pickHandler->getPickNode());
	root->addChild(pickHandler->getTangentPlane());

	//Add a gps tracker
	gpstracker = new GPSTracker(satRefs, pickHandler);
	root->addChild(gpstracker->getBeams());

	//min/max manipulator
	osg::ref_ptr<MyManipulator> myManipulator = new MyManipulator;
	myManipulator->setHomePosition(osg::Vec3(12.5, 0 , 0),
		osg::Vec3(), osg::Vec3f(0.f,-1.0f,0.f));
	myManipulator->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
	myManipulator->setRotationMode(
		osgGA::NodeTrackerManipulator::TRACKBALL);
	myManipulator->setTrackNode(earth_node);

	myManipulator->setRestrictDistance(true);
	myManipulator->setMinMaxDistance(3, 40.0);

	//Get screen resolution for the info renderer 
	int screenWidth, screenHeight;
	GetDesktopResolution(screenWidth, screenHeight);

	infoRenderer = new InfoRenderer(0, 1000, 0, 1000);
	infoRenderer->setTotalSatellites(satRefs.size());

	//Set callbacks for the info renderer
	pickHandler->setPlaneCallback(&onPlaneChanged);
	pickHandler->setLocationCallback(&onLatLonChanged);
	gpstracker->setVisibleSatelliteCallback(&onVisibleSatellitesChanged);

	root->addChild(infoRenderer->getHUDCamera());

	osgViewer::Viewer viewer;
	viewer.setSceneData(root);
	viewer.setCameraManipulator(myManipulator);
	viewer.addEventHandler(pickHandler.get());
	//uncomment for windowed mode
	viewer.setUpViewInWindow(0, 0, 1000, 1000);

	std::cout << screenWidth << "x" << screenHeight << std::endl;

	while (!viewer.done())
	{
		if (pickHandler->isPicked())
		{
			gpstracker->trackBeams();
		}
		viewer.frame();
	}
	pickHandler->getPickPlane()->printPlaneEquation();

	return 0;
}
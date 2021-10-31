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
#include <tuple>

//earth distance mapped to 1
const double radius_earth = 6378.137;
const double satellite_distance = 37000 / radius_earth;
const double radius_sun = 695990.0 / radius_earth;
const double AU = 149697900.0 / radius_earth;

void createAnimationPath(float radius, float time,
	//Alle rotationen für einen channel
	osgAnimation::QuatKeyframeContainer* container2)
{
	unsigned int numSamples = 32;
	float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
	float delta_time = time / (float)numSamples;
	for (unsigned int i = 0; i < numSamples; ++i)
	{
		float yaw = delta_yaw * (float)i;
		osg::Vec3 pos(sinf(yaw) * radius, cosf(yaw) * radius, 0.0f);
		osg::Quat rot(-yaw, osg::Z_AXIS);

		container2->push_back(
			osgAnimation::QuatKeyframe(delta_time * (float)i, rot)
		);
	}
}



osg::MatrixTransform* createEarthGeode() {
	osg::Geode* earth_geode = new osg::Geode();
	osg::StateSet* stateset = new osg::StateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	earth_geode->setStateSet(stateset);

	osg::TessellationHints* hints = new osg::TessellationHints;
	hints->setDetailRatio(1.0f);
	auto earth_sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), 1.0), hints);
	auto earthState = earth_sphere->getOrCreateStateSet();
	earthState->setTextureAttributeAndModes(0, new osg::TexGen);
	osg::ref_ptr<osg::TextureCubeMap> tcm = new osg::TextureCubeMap;
	tcm->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("G:/git/earthCube/flipped/posx.jpeg")); //posx
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("G:/git/earthCube/flipped/negx.jpeg")); //links
	tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("G:/git/earthCube/flipped/negy.jpeg")); //hinten
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("G:/git/earthCube/flipped/posyy.jpeg")); //vorne
	tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("G:/git/earthCube/flipped/negz.jpeg")); //oben
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("G:/git/earthCube/flipped/posz.jpeg")); //negy
	earthState->setTextureAttributeAndModes(0, tcm.get());

	earth_geode->addDrawable(earth_sphere);
	osg::ref_ptr<osg::MatrixTransform> earth_node =
		new osg::MatrixTransform;
	earth_node->addChild(earth_geode);
	//earth_node->setMatrix(osg::Matrix::rotate(0.5*-PI, osg::Z_AXIS) * osg::Matrix::rotate(0.5 * PI, osg::Y_AXIS));
	//earth_node->setNodeMask(0x5);

	auto red = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(1, 0, 0), 0.04));
	auto blue = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, 1), 0.04));
	auto green = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 1, 0), 0.04));
	red->setColor(osg::Vec4(1.f, 0.f, 0.f, 0.f));
	blue->setColor(osg::Vec4(0.f, 0.f, 1.f, 0.f));
	green->setColor(osg::Vec4(0.f, 1.f, 0.f, 0.f));

	auto black = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(-1, 0, 0), 0.04));
	black->setColor(osg::Vec4(0.f, 0.f, 0.f, 0.f));
	auto grey = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, -1, 0), 0.04));
	grey->setColor(osg::Vec4(0.5, 0.5f, 0.5f, 0.f));
	auto white = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, -1), 0.04));
	white->setColor(osg::Vec4(1.f, 1.f, 1.f, 0.f));

	earth_node->addChild(red);
	earth_node->addChild(blue);
	earth_node->addChild(green);
	earth_node->addChild(black);
	earth_node->addChild(grey);
	earth_node->addChild(white);

	return earth_node.release();
}

osg::MatrixTransform* createSunGeode() {
	//Create Sun Node
	osg::ref_ptr<osg::ShapeDrawable> sun_sphere = new
		osg::ShapeDrawable;
	sun_sphere->setShape(new osg::Sphere(osg::Vec3(), radius_sun));
	sun_sphere->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	osg::ref_ptr<osg::Geode> sun_geode = new osg::Geode;
	sun_geode->addDrawable(sun_sphere.get());
	osg::ref_ptr<osg::MatrixTransform> sun_node =
		new osg::MatrixTransform;
	sun_node->setMatrix(osg::Matrix::translate(0.0, AU , 0.0));
	sun_node->addChild(sun_geode.get());

	return sun_node.release();
}

osg::Geode* createSpaceSkyBoxGeode() {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), 999.0)
	));


	osg::ref_ptr<SkyBox> skybox = new SkyBox;
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new osg::TexGen);


	osg::ref_ptr<osg::TextureCubeMap> cubemap =
		new osg::TextureCubeMap;
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("G:/git/spaceCube/left.jpeg"));
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("G:/git/spaceCube/right.jpeg"));
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("G:/git/spaceCube/bottom.jpeg"));
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("G:/git/spaceCube/top.jpeg"));
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("G:/git/spaceCube/front.jpeg"));
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("G:/git/spaceCube/back.jpeg"));
	// Please find details in the source code
	cubemap->setResizeNonPowerOfTwoHint(false);
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, cubemap.get());
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	return geode.release();
}

float modulo(int x, int N) {
	return (x % N + N) % N;
}

osg::MatrixTransform* createSatelliteRing(int count)
{
	osg::ref_ptr<osg::Node> satellite_obj = osgDB::readNodeFile("G:/git/satellite/satellite_obj.obj");
	osg::ref_ptr<osg::MatrixTransform> satellite = new osg::MatrixTransform;
	satellite->addChild(satellite_obj);

	osg::ref_ptr<osg::MatrixTransform> satellite_ring = new osg::MatrixTransform;

	for (int i = 0; i < count; i++)
	{
		osg::ref_ptr<osg::MatrixTransform> instanced_satellite = new osg::MatrixTransform;
		instanced_satellite->addChild(satellite);

		float translateX = -sin(2.0 * i * PI / count);
		float translateY = -cos(2.0 * i * PI / count);

		float angleDegrees = modulo((90 - (360/count)*i),360);

		std::cout << "test: " << modulo(90 - 45 * 3,360) << std::endl;

		float angleRads = angleDegrees * PI / 180.;
		std::cout << angleDegrees << std::endl;

		instanced_satellite->setMatrix(osg::Matrix::scale(0.04, 0.04, 0.04) *
			osg::Matrix::rotate(angleRads, osg::Z_AXIS) *
			osg::Matrix::translate(translateX*(1.0 + satellite_distance), translateY*(1.0 + satellite_distance), 0.0));

		satellite_ring->addChild(instanced_satellite);
	}

	

	return satellite_ring.release();
}

int main(int argc, char** argv)
{
	//osg::ref_ptr<osg::MatrixTransform> sun_node = createSunGeode();
	osg::ref_ptr<osg::MatrixTransform> earth_node = createEarthGeode();
	osg::ref_ptr<osg::MatrixTransform> satellite_ring_template = createSatelliteRing(8);

	osg::ref_ptr<osg::MatrixTransform> satellite_ring_one = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> satellite_ring_two = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> satellite_ring_three = new osg::MatrixTransform;

	satellite_ring_one->addChild(satellite_ring_template);
	satellite_ring_two->addChild(satellite_ring_template);
	satellite_ring_three->addChild(satellite_ring_template);

	satellite_ring_two->setMatrix(osg::Matrix::rotate((22.5) * PI / 180, osg::Z_AXIS) * osg::Matrix::rotate((90) * PI / 180, osg::X_AXIS));
	satellite_ring_three->setMatrix(osg::Matrix::rotate((22.5) * PI / 180, osg::Z_AXIS) * osg::Matrix::rotate((90) * PI / 180, osg::Y_AXIS));

	osg::ref_ptr<osgAnimation::QuatSphericalLinearChannel> ch2 =
		new osgAnimation::QuatSphericalLinearChannel;
	ch2->setName("quat");
	ch2->setTargetName("PathCallback");


	createAnimationPath(1.0 + satellite_distance, 18.0f,
		ch2->getOrCreateSampler()->getOrCreateKeyframeContainer());

	osg::ref_ptr<osgAnimation::Animation> animation = new
		osgAnimation::Animation;
	animation->setPlayMode(osgAnimation::Animation::LOOP);
	animation->addChannel(ch2.get());

	osg::ref_ptr<osgAnimation::UpdateMatrixTransform> updater =
		new osgAnimation::UpdateMatrixTransform("PathCallback");
	updater->getStackedTransforms().push_back(
		new osgAnimation::StackedQuaternionElement("quat"));


	satellite_ring_template->setDataVariance(osg::Object::DYNAMIC);
	satellite_ring_template->setUpdateCallback(updater.get());

	//satellite_ring_two->setDataVariance(osg::Object::DYNAMIC);
	//satellite_ring_two->setUpdateCallback(updater.get());

	osg::ref_ptr<osgAnimation::BasicAnimationManager> manager =
		new osgAnimation::BasicAnimationManager;
	manager->registerAnimation(animation.get());

	osg::ref_ptr<osg::Group> root = new osg::Group;
	//root->addChild(sun_node);
	root->addChild(earth_node);
	root->addChild(createSpaceSkyBoxGeode());
	root->addChild(satellite_ring_one);
	root->addChild(satellite_ring_two);
	root->addChild(satellite_ring_three);
	root->setUpdateCallback(manager.get());


	osg::ref_ptr<PickHandler> picker = new PickHandler;
	root->addChild(picker->getOrCreateSelectionBox());


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

	std::cout << viewer.getCamera()->getNearFarRatio() << std::endl;

	manager->playAnimation(animation.get());

	float frameCount = 0;
	while (!viewer.done())
	{
		frameCount++;
		//satellite_ring_container->setMatrix(osg::Matrix::rotate((frameCount / 2) * PI / 180, osg::X_AXIS));
		viewer.frame();
	}

	return 0;
}
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

#include <tuple>

#include <osg/AnimationPath>

class InfoVisitor : public osg::NodeVisitor
{
public:
	InfoVisitor() 
	{
		setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
	}

	virtual void apply(osg::MatrixTransform& node);
	virtual int getSatelliteCount();
	std::vector<osg::MatrixTransform*>& getSatellites();
	virtual void clear();

protected:
	std::vector<osg::MatrixTransform*> satellites;
};

void InfoVisitor::apply(osg::MatrixTransform& node)
{
	if (node.getName().compare("satellite") == 0)
	{
		satellites.push_back(&node);
	}

	traverse(node);
}

int InfoVisitor::getSatelliteCount()
{
	return satellites.size();
}

void InfoVisitor::clear()
{
	return satellites.clear();
}

std::vector<osg::MatrixTransform*>& InfoVisitor::getSatellites()
{
	return satellites;
}

float modulo(int x, int N) {
	return (x % N + N) % N;
}

/*
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
*/

osg::AnimationPath* createAnimationPath(float radius, float time
)
{
	osg::ref_ptr<osg::AnimationPath> path = new
		osg::AnimationPath;
	path->setLoopMode(osg::AnimationPath::LOOP);

	unsigned int numSamples = 32;
	float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
	float delta_time = time / (float)numSamples;
	for (unsigned int i = 0; i < numSamples; ++i)
	{
		float yaw = delta_yaw * (float)i;
		osg::Vec3 pos(sinf(yaw) * radius, cosf(yaw) * radius, 0.0f);
		osg::Quat rot(-yaw, osg::Z_AXIS);
		path->insert(delta_time * (float)i,
			osg::AnimationPath::ControlPoint(pos, rot)
		);
	}
	return path.release();
}

osg::MatrixTransform* createSatelliteRing(int count)
{
	osg::ref_ptr<osg::Node> satellite_obj = osgDB::readNodeFile("G:/git/satellite/satellite_obj.obj");
	osg::ref_ptr<osg::MatrixTransform> satellite_template = new osg::MatrixTransform;
	satellite_template->setName("satellite");
	satellite_template->addChild(satellite_obj);

	osg::ref_ptr<osg::MatrixTransform> satellite_ring = new osg::MatrixTransform;
	 
	for (int i = 0; i < count; i++)
	{
		osg::ref_ptr<osg::MatrixTransform> instanced_satellite = new osg::MatrixTransform;
		instanced_satellite->addChild(satellite_template);

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

osg::MatrixTransform* instanceSatelliteMatrix(osg::MatrixTransform* satellite_ring, const std::vector<osg::Matrix> rotations)
{
	 
	osg::ref_ptr<osg::MatrixTransform> satellite_system = new osg::MatrixTransform;

	for (auto rotation : rotations) {
		osg::ref_ptr<osg::MatrixTransform> satellite_ring_transformation = new osg::MatrixTransform;
		satellite_ring_transformation->addChild(satellite_ring);
		satellite_ring_transformation->setMatrix(rotation);
		satellite_system->addChild(satellite_ring_transformation);
	}

	return satellite_system.release();
}

int main(int argc, char** argv)
{
	//osg::ref_ptr<osg::MatrixTransform> sun_node = createSunGeode();
	osg::ref_ptr<osg::MatrixTransform> earth_node = createEarthGeode();
	osg::ref_ptr<osg::MatrixTransform> satellite_ring_template = createSatelliteRing(4);

	osg::ref_ptr<osg::AnimationPathCallback> apcb = new
		osg::AnimationPathCallback;
	apcb->setAnimationPath(createAnimationPath(1.0 + satellite_distance, 18.0f));
	satellite_ring_template->setUpdateCallback(apcb.get());

	std::vector<osg::Matrix> x_rotations;
	x_rotations.push_back(osg::Matrix::rotate((45) * PI / 180, osg::Z_AXIS) * osg::Matrix::rotate((45) * PI / 180, osg::X_AXIS));
	x_rotations.push_back(osg::Matrix::rotate(-(45) * PI / 180, osg::X_AXIS));

	std::vector<osg::Matrix> satellite_system_rotations;
	satellite_system_rotations.push_back(osg::Matrix::rotate(0, osg::X_AXIS));
	satellite_system_rotations.push_back(osg::Matrix::rotate((90) * PI / 180, osg::Y_AXIS));
	satellite_system_rotations.push_back(osg::Matrix::rotate((90) * PI / 180, osg::Z_AXIS));


	osg::ref_ptr<osg::MatrixTransform> x_system = instanceSatelliteMatrix(satellite_ring_template, x_rotations);
	osg::ref_ptr<osg::MatrixTransform> satellite_system = instanceSatelliteMatrix(x_system, satellite_system_rotations);


	osg::ref_ptr<osgAnimation::QuatSphericalLinearChannel> rotationChannel =
		new osgAnimation::QuatSphericalLinearChannel;
	rotationChannel->setName("quat");
	rotationChannel->setTargetName("PathCallback");


	//createAnimationPath(1.0 + satellite_distance, 18.0f,
	//	rotationChannel->getOrCreateSampler()->getOrCreateKeyframeContainer());

	/*osg::ref_ptr<osgAnimation::Animation> animation = new
		osgAnimation::Animation;
	animation->setPlayMode(osgAnimation::Animation::LOOP);
	animation->addChannel(rotationChannel.get());

	osg::ref_ptr<osgAnimation::UpdateMatrixTransform> updater =
		new osgAnimation::UpdateMatrixTransform("PathCallback");
	updater->getStackedTransforms().push_back(
		new osgAnimation::StackedQuaternionElement("quat"));


	satellite_ring_template->setDataVariance(osg::Object::DYNAMIC);
	satellite_ring_template->setUpdateCallback(updater.get()); 


	osg::ref_ptr<osgAnimation::BasicAnimationManager> manager =
		new osgAnimation::BasicAnimationManager;
	manager->registerAnimation(animation.get());*/

	osg::ref_ptr<osg::ShapeDrawable> shape4 = new osg::ShapeDrawable;
	shape4->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f),
		0.05f, satellite_distance));

	osg::ref_ptr<osg::Group> root = new osg::Group;
	//root->addChild(sun_node);
	root->addChild(earth_node);
	root->addChild(createSpaceSkyBoxGeode());
	root->addChild(satellite_ring_template);
	
	root->addChild(shape4);

	//root->setUpdateCallback(manager.get());


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

	//manager->playAnimation(animation.get());

	/*InfoVisitor infoVisitor;
	root->accept(infoVisitor);
	std::cout << infoVisitor.getSatelliteCount() << std::endl;
	std::vector<osg::MatrixTransform*> satellites = infoVisitor.getSatellites();
	osg::MatrixTransform* refSatellite0 = satellites[0];

	osg::ref_ptr<osg::AnimationPathCallback> apcb = new
		osg::AnimationPathCallback;*/


	float frameCount = 0;
	while (!viewer.done())
	{
		
		frameCount++;
		viewer.frame();
	}

	return 0;
}
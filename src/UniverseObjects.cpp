#include <UniverseObjects.h>



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
	earth_node->setNodeMask(0x5);

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
	sun_node->setMatrix(osg::Matrix::translate(0.0, AU, 0.0));
	sun_node->addChild(sun_geode.get());
	sun_node->setNodeMask(~0x5);
	return sun_node.release();
}

osg::Geode* createSpaceSkyBoxGeode() {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), 999.0)
	));
	geode->setNodeMask(~0x5);

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


osg::MatrixTransform* createSatelliteRing(int count, 
	const std::string& ringPrefix, 
	const osg::Matrix& mat,
	osg::AnimationPathCallback* animation, 
	std::vector<osg::PositionAttitudeTransform*>& refList)
{
	osg::ref_ptr<osg::Node> satellite_obj = osgDB::readNodeFile("G:/git/satellite/satellite_obj.obj");
	//auto test = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), 1.0));
	osg::ref_ptr<osg::PositionAttitudeTransform> satellite_template = new osg::PositionAttitudeTransform;

	satellite_template->setNodeMask(~0x5);
	satellite_template->addChild(satellite_obj);
	
	osg::ref_ptr<osg::MatrixTransform> satellite_ring = new osg::MatrixTransform;

	for (int i = 0; i < count; i++)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> instanced_satellite = new osg::PositionAttitudeTransform;
		instanced_satellite->setName(ringPrefix + "satellite" + std::to_string(i));
		instanced_satellite->addChild(satellite_template);

		float translateX = -sin(2.0 * i * PI / count);
		float translateY = -cos(2.0 * i * PI / count);

		float angleDegrees = modulo((90 - (360 / count) * i), 360);

		std::cout << "test: " << modulo(90 - 45 * 3, 360) << std::endl;

		float angleRads = angleDegrees * PI / 180.;
		std::cout << angleDegrees << std::endl;

		instanced_satellite->setScale(osg::Vec3d(0.04, 0.04, 0.04));
		instanced_satellite->setAttitude(osg::Quat(angleRads, osg::Z_AXIS));
		instanced_satellite->setPosition(osg::Vec3d(translateX * (1.0 + satellite_distance), translateY * (1.0 + satellite_distance), 0.0));

		refList.push_back(instanced_satellite);
		/*instanced_satellite->setMatrix(osg::Matrix::scale(0.04, 0.04, 0.04) *
			osg::Matrix::rotate(angleRads, osg::Z_AXIS) *
			osg::Matrix::translate(translateX * (1.0 + satellite_distance), translateY * (1.0 + satellite_distance), 0.0));*/


		satellite_ring->addChild(instanced_satellite.release());
		satellite_ring->setUpdateCallback(animation);
	}
	osg::ref_ptr<osg::MatrixTransform> rotWrap = new osg::MatrixTransform;
	rotWrap->addChild(satellite_ring.release());
	rotWrap->setMatrix(mat);
	return rotWrap.release();
}

void instanceSatelliteSystem(osg::MatrixTransform* satellite_ring, const std::vector<osg::Matrix>& rotations, osg::Group* root)
{

	//osg::ref_ptr<osg::MatrixTransform> satellite_system = new osg::MatrixTransform;

	for (int i = 0; i < rotations.size(); i++) { //current Ring
		osg::ref_ptr<osg::MatrixTransform> satellite_ring_transformation = new osg::MatrixTransform;

		satellite_ring_transformation->addChild(satellite_ring);
		satellite_ring_transformation->setMatrix(rotations.at(i));
		satellite_ring_transformation->setName("ring" + std::to_string(i));
		root->addChild(satellite_ring_transformation.release());
		//satellite_system->addChild(satellite_ring_transformation);
	}

	//root->addChild(satellite_system.release());
}
#include <PickHandler.h>


struct latlon {
	double lat;
	double lon;

	void print() {
		std::cout << "lat: " << lat << "\nlon: " << lon << std::endl;
	}
};

int sign(double x) {
	if (x > 0.0) return 1;
	if (x < 0.0) return -1;
	return 0;
}

latlon vec3ToLatlon(const osg::Vec3& earthCoord) {
	auto [x, y, z] = std::make_tuple(earthCoord.x(), earthCoord.y(), earthCoord.z());

	//Normalize X, Z longitude
	double magnitude = 1 / sqrt(pow(z, 2) + pow(x, 2));
	x = magnitude * x;
	z = magnitude * z;

	//Transform from 0-360 to (-180  <-  0  ->  180) angle system
	x = -x;
	z = -z;

	double lon = 0.0;

	if (z < 0)
		lon = 180. - (acos(x) * 180./PI);
	else
		lon = (acos(x) * 180. / PI) - 180.;
	 
	std::cout << "previus lat: " << y * -90. << std::endl;

	return latlon{ -asin(y) * 180. / PI, lon };
}

osg::Node* PickHandler::getOrCreateSelectionBox()
{
	if (!_selectionSphere)
	{
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		//auto markPoint = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), 1.0f));
		//markPoint->setColor(osg::Vec4(1.0f, 0.f, 0.f, 0.f));
		//geode->addDrawable(markPoint);

		_selectionSphere = new osg::MatrixTransform;
		//Maske wie in Unity Layer
		_selectionSphere->setNodeMask(0x1);
		_selectionSphere->addChild(geode.get());

		osg::StateSet* ss = _selectionSphere->getOrCreateStateSet();
		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		//ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	}
	return _selectionSphere.get();
}



bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	//Nur Mausklick bitte
	if (ea.getEventType() != osgGA::GUIEventAdapter::RELEASE || ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON || !(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL))
		return false;

	std::cout << "click" << std::endl;

	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (viewer)
	{
		//Maus Ray Casten
		osg::ref_ptr<osgUtil::LineSegmentIntersector>
			intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

		std::cout << "window coords: " << ea.getX() << "/" << ea.getY() << std::endl;

		osgUtil::IntersectionVisitor iv(intersector.get());
		//Maske wie in Unity Layer
		//That makes the selection box itself NOT pickable by the visitor, as shown in the following diagram:
		iv.setTraversalMask(0x5);
		viewer->getCamera()->accept(iv);

		//Getroffen?
		if (intersector->containsIntersections()) 
		{
			osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());
			std::cout << result.localIntersectionPoint.x() << "..." << result.localIntersectionPoint.y() << "..." << result.localIntersectionPoint.z() << std::endl;
			vec3ToLatlon(result.localIntersectionPoint).print();

			osg::ref_ptr<osg::ShapeDrawable> shape2 = new osg::ShapeDrawable;
			shape2->setShape(new osg::Sphere(result.localIntersectionPoint,
				0.005f));

			_selectionSphere->addChild(shape2);

			/*osg::BoundingSphere bb = result.drawable->getBound();
			osg::Vec3 worldCenter = bb.center() * osg::computeLocalToWorld(result.nodePath);



			_selectionSphere->setMatrix(
				osg::Matrix::scale(bb.radius(), bb.radius(), bb.radius()) *
				osg::Matrix::translate(worldCenter));*/
		}
		else
		{
			std::cout << "no intersection" << std::endl;
		}
	}
	
	return false;
}
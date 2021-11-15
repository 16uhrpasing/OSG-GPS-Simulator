#include <PickHandler.h>

struct Latlon {
	double lat;
	double lon;

	void print() {
		std::cout << "lat: " << lat << "\nlon: " << lon << std::endl;
	}

	std::string toHumanReadableString() {
		return parseLatLon(lat, lon);
	}
};



std::string parseLatLon(double latValue, double lonValue)
{
	Direction directionLatitude = latValue < 0 ? South : North;
	std::string latParse = parseLatitudeOrLongitude(latValue, directionLatitude);

	Direction directionLongitude = lonValue < 0 ? West : East;
	std::string lonParse = parseLatitudeOrLongitude(lonValue, directionLongitude);

	return latParse + " " + lonParse;
}

//This must be a private method because it requires the caller to ensure
//that the direction parameter is correct.
std::string parseLatitudeOrLongitude(double value, Direction direction)
{
	value = abs(value);

	double degrees = trunc(value);

	value = (value - degrees) * 60;    

	double minutes = trunc(value);
	double seconds = (value - minutes) * 60; 

	std::string directionStr = "";

	switch (direction)
	{
	case North:
		directionStr = "N";
		break;
	case East:
		directionStr = "E";
		break;
	case South:
		directionStr = "S";
		break;
	case West:
		directionStr = "W";
		break;
	}

	//degrees = roundTwoDecimals(degrees);
	//minutes = roundTwoDecimals(minutes);
	seconds = roundTwoDecimals(seconds);

	return std::to_string(degrees).substr(0, 2) + "°" + std::to_string(minutes).substr(0, 2) +
		"'" + std::to_string(seconds).substr(0, 4) + "''" + directionStr;
}

int sign(double x) {
	if (x > 0.0) return 1;
	if (x < 0.0) return -1;
	return 0;
}

Latlon vec3ToLatlon(const osg::Vec3& earthCoord) {
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

	return Latlon{ -asin(y) * 180. / PI, lon };
}

osg::Node* PickHandler::getPickNode()
{
	return _selectionSphere.get();
}

osg::Vec3d PickHandler::getPickNodePos()
{
	return _selectionSphere->getPosition();
}

bool PickHandler::isPicked()
{
	return _picked;
}

MathPlane* PickHandler::getPickPlane() {
	return _pickPlane;
}

osg::MatrixTransform* PickHandler::getTangentPlane() {
	return _tangentPlane;
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
			if (result.localIntersectionPoint.length() < 0.9) return false;
			osg::Vec3d normalizedIntersection = result.localIntersectionPoint;
			normalizedIntersection.normalize();
			std::cout << normalizedIntersection.x() << "..." << normalizedIntersection.y() << "..." << normalizedIntersection.z() << std::endl;
			
			Latlon latlon = vec3ToLatlon(normalizedIntersection);
			latlon.print();
			std::cout << latlon.toHumanReadableString() << std::endl;
			
			_picked = true;
			_selectionSphere->setPosition(normalizedIntersection);

			osg::Vec3d crossNormal(1.0,0.0,0.0);
			if (abs(normalizedIntersection.x()) > 0.9)
				crossNormal.set(0.0, 1.0, 0.0);

			osg::Vec3d firstDirection = normalizedIntersection ^ crossNormal;
			osg::Vec3d secondDirection = normalizedIntersection ^ firstDirection;

			_pickPlane->setPlaneEquation(normalizedIntersection, firstDirection, secondDirection);
			_pickPlane->printParametricEquation();

			if (isPlaneCallbackSet) planeCallback(_pickPlane->getPlaneEquationString());
			if (isLocationCallbackSet) locationCallback(latlon.toHumanReadableString());

			//_tangentPlane->unref();
			//_tangentPlane = new osg::MatrixTransform;
			_tangentPlane->removeChildren(0, _tangentPlane->getNumChildren());
	
			

			for (double x = -10; x < 10; x += 0.5)
			{
				for (double y = -10; y < 10; y += 0.5)
				{
					osg::Vec3d drawCenter = _pickPlane->parametricEquation(x, y);
					osg::ref_ptr<osg::MatrixTransform> instancePlanePoint = new osg::MatrixTransform;
					instancePlanePoint->addChild(_tangentPointTemplate);

					instancePlanePoint->setMatrix(osg::Matrix::translate(drawCenter));
					_tangentPlane->addChild(instancePlanePoint);
				}
			}

		}
		else
		{
			std::cout << "no intersection" << std::endl;
		}
	}
	
	return false;
}

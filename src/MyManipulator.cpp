#include <MyManipulator.h>


bool MyManipulator::handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (restrictDistance)
	{
		osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
		bool scrollAllowed = true;
		float zoomFactor = 0.0;

		switch (sm)
		{
			// mouse scroll up event
		case osgGA::GUIEventAdapter::SCROLL_UP:
		{
			std::cout << "scroll up" << std::endl;
			float scale = 1.0f + _wheelZoomFactor;
			float totalDistance = _distance * scale;
			if (totalDistance > maxDistance) return false;
			break;
		}

		// mouse scroll down event
		case osgGA::GUIEventAdapter::SCROLL_DOWN:
		{
			std::cout << "scroll down" << std::endl;
			float scale = 1.0f + _wheelZoomFactor;
			float totalDistance = _distance * scale;
			if (totalDistance < minDistance) return false;
			break;
		}
		}


		// unhandled mouse scrolling motion
		//default:{}
	}



	return osgGA::NodeTrackerManipulator::handleMouseWheel(ea, us);
}


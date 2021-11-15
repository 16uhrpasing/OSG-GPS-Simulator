#pragma once

#include <osgGA/NodeTrackerManipulator>
#include <iostream>

/**
 * Extend manipulator class to set a min/max distance
 */
class MyManipulator : public osgGA::NodeTrackerManipulator
{
public:
	MyManipulator(int flags = DEFAULT_SETTINGS) : osgGA::NodeTrackerManipulator(flags) {};

	MyManipulator(const NodeTrackerManipulator& om,
		const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY) :
		osgGA::NodeTrackerManipulator(om, copyOp) {}

	bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) override;

	void setRestrictDistance(bool dist) {
		restrictDistance = dist;
	}

	bool getRestrictDistance() {
		return restrictDistance;
	}

	void setMinMaxDistance(double min, double max) {
		minDistance = min;
		maxDistance = max;
	}

	double getMinDistance() {
		return minDistance;
	}

	double getMaxDistance() {
		return maxDistance;
	}

private:
	double minDistance = 0.0f;
	double maxDistance = 0.0f;
	bool restrictDistance = false;
};

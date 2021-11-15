#pragma once
#include <vector>
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <iostream>
#include <osg/io_utils>
#include <PickHandler.h>

typedef void (*VisibleSatelliteCallback)(int);

struct DeconstructedMatrix {
	osg::Vec3d satPos;
	osg::Quat satRot;
	osg::Vec3d satScale;
	osg::Quat satSo;
};

class GPSTracker 
{
public:
	GPSTracker(const std::vector<osg::PositionAttitudeTransform*>& satRefs, PickHandler* pHandler)
		: 
		satelliteListRef(satRefs), 
		pickHandler(pHandler), 
		beams(new osg::Group),
		fillMode(new osg::PolygonMode),
		pointMode(new osg::PolygonMode),
		beamTemplate(new osg::ShapeDrawable)
	{
		fillMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
			osg::PolygonMode::FILL);

		
		pointMode->setMode(osg::PolygonMode::FRONT_AND_BACK,
			osg::PolygonMode::POINT);

		beamTemplate->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f),
			0.01f, 1.0));
		beamTemplate->setColor(osg::Vec4d(0.897698, 0.112652, 0.425967, 1.));
		beamTemplate->setNodeMask(~0x5);

		for (int i = 0; i < satelliteListRef.size(); i++)
		{
			osg::ref_ptr<osg::MatrixTransform> instancedBeam = new osg::MatrixTransform;
			instancedBeam->addChild(beamTemplate);

			beamRefs.push_back(instancedBeam);
			beams->addChild(instancedBeam.release());
		}
	}

	virtual osg::Group* getBeams();
	virtual void trackBeams();

	inline void setVisibleSatelliteCallback(VisibleSatelliteCallback vsc)
	{
		isVisibleSatelliteCallbackSet = true;
		visibleSatelliteCallback = vsc;
	}

protected:
	const std::vector<osg::PositionAttitudeTransform*>& satelliteListRef;
	PickHandler* pickHandler;
	osg::ref_ptr<osg::Group> beams;
	osg::ref_ptr<osg::PolygonMode> fillMode;
	osg::ref_ptr<osg::PolygonMode> pointMode;
	osg::ref_ptr<osg::ShapeDrawable> beamTemplate;

	std::vector<bool> aboveTracker;
	std::vector<osg::MatrixTransform*> beamRefs;
	bool initialized = false;
	DeconstructedMatrix decomposition;

	int visibleSatelliteCount = 0;
	bool isVisibleSatelliteCallbackSet = false;
	VisibleSatelliteCallback visibleSatelliteCallback;
};
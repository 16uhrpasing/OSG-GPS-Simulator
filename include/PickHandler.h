#pragma once

#include <osg/MatrixTransform>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osg/PolygonMode>
#include <iostream>
#include <osgViewer/Viewer>
#include <osgUtil/LineSegmentIntersector>
#include <MathLib.h>

typedef void (*PlaneCallback)(std::string);
typedef void (*LocationCallback)(std::string);

class PickHandler : public osgGA::GUIEventHandler
{
public:
	//Enthält eine Node zu einer Drawable von einer LineBox
	PickHandler() : GUIEventHandler(), _picked(false) {
		_selectionSphere = new osg::PositionAttitudeTransform;
		//Maske wie in Unity Layer
		_selectionSphere->setNodeMask(~0x5);

		osg::StateSet* ss = _selectionSphere->getOrCreateStateSet();
		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		_selectionSphere = new osg::PositionAttitudeTransform;
		osg::ref_ptr<osg::ShapeDrawable> selectionPoint = new osg::ShapeDrawable;
		selectionPoint->setShape(new osg::Sphere(osg::Vec3(0.f,0.f,0.f),
			0.020f));
		_selectionSphere->addChild(selectionPoint);
		_pickPlane = new MathPlane(osg::Vec3d(0.f, 0.f, 0.f), osg::Vec3d(1.f, 0.f, 0.f), osg::Vec3d(0.f, 1.f, 0.f));
		_tangentPlane = new osg::MatrixTransform;

		_tangentPointTemplate = new osg::ShapeDrawable;
		_tangentPointTemplate->setShape(new osg::Sphere(osg::Vec3(0.0,0.0,0.0),
			0.02f));
		_tangentPointTemplate->setColor(osg::Vec4f(0.7, 0.2, 0.3, 1.0));
		
	}
	~PickHandler() {
		delete _pickPlane;
	}
	virtual osg::Node* getPickNode();
	virtual osg::Vec3d getPickNodePos();
	virtual bool isPicked();
	virtual MathPlane* getPickPlane();
	virtual osg::MatrixTransform* getTangentPlane();



	virtual bool handle(const osgGA::GUIEventAdapter& ea,
		osgGA::GUIActionAdapter& aa);

	inline void setPlaneCallback(PlaneCallback pc) {
		isPlaneCallbackSet = true;
		planeCallback = pc;
	}
	inline void setLocationCallback(LocationCallback lc) {
		isLocationCallbackSet = true;
		locationCallback = lc;
	}

protected:
	osg::ref_ptr<osg::PositionAttitudeTransform> _selectionSphere;
	osg::ref_ptr<osg::MatrixTransform> _tangentPlane;
	bool _picked;
	MathPlane* _pickPlane;
	osg::ref_ptr<osg::ShapeDrawable> _tangentPointTemplate;

	bool isPlaneCallbackSet = false;
	bool isLocationCallbackSet = false;
	PlaneCallback planeCallback;
	LocationCallback locationCallback;
};

enum Direction {
	North, East, South, West
};

std::string parseLatLon(double latValue, double lonValue);
std::string parseLatitudeOrLongitude(double value, Direction direction);
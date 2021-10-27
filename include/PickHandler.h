#pragma once

#include <osg/MatrixTransform>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <iostream>
#include <osgViewer/Viewer>
#include <osgUtil/LineSegmentIntersector>
const double PI = 3.14159265;

class PickHandler : public osgGA::GUIEventHandler
{
public:
	//Enthält eine Node zu einer Drawable von einer LineBox
	osg::Node* getOrCreateSelectionBox();
	virtual bool handle(const osgGA::GUIEventAdapter& ea,
		osgGA::GUIActionAdapter& aa);

protected:
	osg::Vec3d _spherePos;
	osg::ref_ptr<osg::MatrixTransform> _selectionSphere;
};
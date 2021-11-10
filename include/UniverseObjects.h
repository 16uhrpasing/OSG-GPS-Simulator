#pragma once

#include <Skybox.h>

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/TexGen>
#include <osg/TextureCubeMap>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/ShapeDrawable>
#include <MathLib.h>

#include <osgDB/ObjectWrapper>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

//earth distance mapped to 1
const double radius_earth = 6378.137;
const double satellite_distance = 37000 / radius_earth;
const double radius_sun = 695990.0 / radius_earth;
const double AU = 149697900.0 / radius_earth;


osg::MatrixTransform* createEarthGeode();
osg::MatrixTransform* createSunGeode();
osg::Geode* createSpaceSkyBoxGeode();
osg::MatrixTransform* createSatelliteRing(int count,
	const std::string& ringPrefix,
	const osg::Matrix& mat,
	osg::AnimationPathCallback* animation,
	std::vector<osg::PositionAttitudeTransform*>& refList);
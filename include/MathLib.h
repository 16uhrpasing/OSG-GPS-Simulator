#pragma once
#include <osg/MatrixTransform>
#include <iostream>
#include <osg/io_utils>

const double PI = 3.14159265;

class MathPlane {
public:
	MathPlane(osg::Vec3d& offset, osg::Vec3d& directionOne, osg::Vec3d& directionTwo) :
		offset(offset), directionOne(directionOne), directionTwo(directionTwo)
	{
		setPlaneEquation(offset, directionOne, directionTwo);
	}

	virtual double insertPlaneEquation(const osg::Vec3d& p);
	virtual osg::Vec3d parametricEquation(double s, double t);
	virtual void printPlaneEquation();
	virtual void printParametricEquation();
	virtual void setPlaneEquation(const osg::Vec3d& offset, const osg::Vec3d& directionOne, const osg::Vec3d& directionTwo);
	virtual osg::Vec4d& getPlaneEquation();

protected:
	//equation
	osg::Vec4d planeEquation; //x,y,z,d

	//parametric
	osg::Vec3d offset;
	osg::Vec3d directionOne;
	osg::Vec3d directionTwo;
};


float modulo(int x, int N);

double angleBetweenVec3s(osg::Vec3 v1, osg::Vec3 v2);
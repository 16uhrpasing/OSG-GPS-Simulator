#pragma once
#include <osg/MatrixTransform>
#include <iostream>
#include <osg/io_utils>

const double PI = 3.14159265;

/**
 * Class for plane calculations
 *
 * @param offset, directionOne, directionTwo -> parameters for a plane
 */
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
	virtual std::string getPlaneEquationString();
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

/**
 * Some common math functions
 */

float modulo(int x, int N);

double angleBetweenVec3s(const osg::Vec3d& v1, const osg::Vec3d& v2);

inline double roundTwoDecimals(double input) {
	return std::ceil(input * 100.0) / 100.0;
}
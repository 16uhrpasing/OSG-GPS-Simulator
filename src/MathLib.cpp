#include <MathLib.h>

double MathPlane::insertPlaneEquation(const osg::Vec3d& p)
{
	auto [a, b, c, D] = std::make_tuple(planeEquation.x(), planeEquation.y(), planeEquation.z(), planeEquation.w());
	auto [x, y, z] = std::make_tuple(p.x(), p.y(), p.z());


	return a * x + b * y + c * z + D;
}

void MathPlane::printPlaneEquation()
{
	auto [a, b, c, D] = std::make_tuple(planeEquation.x(), planeEquation.y(), planeEquation.z(), planeEquation.w());

	std::cout << a << "x + " << b << "y + " << c << "z + " << D  << " = 0" << std::endl;
}

void MathPlane::printParametricEquation()
{
	std::cout << offset << " + s*" << directionOne << " + t*" << directionTwo << std::endl;
}

void MathPlane::setPlaneEquation(const osg::Vec3d& offset,const osg::Vec3d& directionOne,const osg::Vec3d& directionTwo) {
	this->offset = offset;
	this->directionOne = directionOne;
	this->directionTwo = directionTwo;

	osg::Vec3d normal = directionOne ^ directionTwo;
	osg::Vec3d randomPoint = offset + directionOne * 2 + directionTwo * 2;

	double D = -normal.x() * randomPoint.x() - normal.y() * randomPoint.y() - normal.z() * randomPoint.z();
	planeEquation.set(normal.x(), normal.y(), normal.z(), D);
}

osg::Vec4d& MathPlane::getPlaneEquation()
{
	return planeEquation;
}

osg::Vec3d MathPlane::parametricEquation(double s, double t) {
	return offset + directionOne*s + directionTwo*t;
}

float modulo(int x, int N) {
	return (x % N + N) % N;
}


double angleBetweenVec3s(const osg::Vec3d& v1, const osg::Vec3d& v2) {
	return acos(v1 * v2) * 180. / PI;
}
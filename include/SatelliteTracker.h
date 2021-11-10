#pragma once
#include <vector>
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <iostream>
#include <osg/io_utils>

struct DeconstructedMatrix {
	osg::Vec3d pos;
	osg::Quat rot;
	osg::Vec3d scale;
	osg::Quat so;
};

class SatelliteTracker : public osg::NodeVisitor
{
public:
	SatelliteTracker() : deconstructedMatrices(nullptr)
	{
		setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
	}

	virtual void apply(osg::MatrixTransform& node);
	virtual int getSatelliteCount();
	virtual std::vector<osg::MatrixTransform*>& getSatellites();
	virtual void initializeMatrixHeap();
	virtual void calculateMatrixHeap(int satellitesPerRing);
	virtual void deleteMatrixHeap();
	virtual bool isHeapInitialized();
	virtual DeconstructedMatrix& getDeconstructedMatrixAt(unsigned int index);

	virtual void clear();

protected:
	bool matrixHeapInitialized = false;
	std::string currentRingName = "";
	std::vector<osg::MatrixTransform*> satellites;
	DeconstructedMatrix** deconstructedMatrices;
};

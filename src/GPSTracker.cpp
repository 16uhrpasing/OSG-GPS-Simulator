#include <GPSTracker.h>

osg::Group* GPSTracker::getBeams()
{
	return beams;
}

void GPSTracker::trackBeams()
{
	//fill aboveTracker
	if (!initialized)
	{
		for (int i = 0; i < satelliteListRef.size(); i++)
		{
			satelliteListRef.at(i)->getWorldMatrices().at(0).decompose(
				decomposition.satPos,
				decomposition.satRot,
				decomposition.satScale,
				decomposition.satSo);

			bool isAbove = pickHandler->getPickPlane()->insertPlaneEquation(decomposition.satPos) > 0.0;
			if (isAbove)
			{
				satelliteListRef.at(i)->getOrCreateStateSet()->setAttribute(fillMode);
				visibleSatelliteCount++;
			}
			else {
				satelliteListRef.at(i)->getOrCreateStateSet()->setAttribute(pointMode);
			}

			aboveTracker.push_back(isAbove);
		}
		initialized = true;
	}


	for (int i = 0; i < satelliteListRef.size(); i++)
	{
		satelliteListRef.at(i)->getWorldMatrices().at(0).decompose(
			decomposition.satPos,
			decomposition.satRot,
			decomposition.satScale,
			decomposition.satSo);
		bool isAbove = pickHandler->getPickPlane()->insertPlaneEquation(decomposition.satPos) > 0.0;

		if (isAbove != aboveTracker.at(i)) {
			aboveTracker.at(i) = isAbove;

			if (isAbove)
			{
				//std::cout << "satellite number " << i << " appeared" << std::endl;
				satelliteListRef.at(i)->getOrCreateStateSet()->setAttribute(fillMode);
				beamRefs.at(i)->setNodeMask(1);
				visibleSatelliteCount++;
			}
			else {
				//std::cout << "satellite number " << i << " left" << std::endl;
				satelliteListRef.at(i)->getOrCreateStateSet()->setAttribute(pointMode);
				beamRefs.at(i)->setNodeMask(0);
				visibleSatelliteCount--;
			}

			if (isVisibleSatelliteCallbackSet) visibleSatelliteCallback(visibleSatelliteCount);
		}

		if (aboveTracker.at(i)){
		//Translate between satellite and pickpoint
		osg::Vec3d pickPos = pickHandler->getPickNodePos();
		osg::Vec3d beamPos = (decomposition.satPos + pickPos) / 2.;

		//Scale to distance between satellite and pickpoint
		double distance = (decomposition.satPos - pickPos).length();
		//std::cout << "distance: " << distance << std::endl;


		osg::Vec3d vecBetweenSatAndBeam = decomposition.satPos - beamPos;
		osg::Vec3d mapToXZPlane = osg::Vec3d(vecBetweenSatAndBeam.x(), 0., vecBetweenSatAndBeam.z());
		mapToXZPlane.normalize();



		vecBetweenSatAndBeam.normalize();
		osg::Vec3d secondRotAxis = mapToXZPlane ^ vecBetweenSatAndBeam;
		double secondRotAngle = angleBetweenVec3s(mapToXZPlane, vecBetweenSatAndBeam);

		double sign = vecBetweenSatAndBeam.x() < 0 ? -1. : 1.;
		double XZrotation = angleBetweenVec3s(osg::Z_AXIS * sign, mapToXZPlane);


		beamRefs.at(i)->setMatrix(osg::Matrix::scale(osg::Vec3d(1., 1., distance)) * osg::Matrix::rotate(XZrotation, osg::Y_AXIS) * osg::Matrix::rotate(secondRotAngle, secondRotAxis) * osg::Matrix::translate(beamPos));
		}
	}

}
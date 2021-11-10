#include <SatelliteTracker.h>

void SatelliteTracker::apply(osg::MatrixTransform& node)
{
	std::size_t foundSat = node.getName().find("satellite");
	std::size_t foundRing = node.getName().find("ring");

	if (foundRing != std::string::npos)
	{
		std::cout << "Tracker found ring: " << node.getName() << std::endl;
		//currentRingName = node.getName().substr(4, 1);
		//currentRing = std::stoi(node.getName().substr(4,1));
		//satellites.push_back(&node);
	}

	if (foundSat != std::string::npos)
	{
		//node.setName(currentRingName + node.getName());
		std::cout << "Tracker found satellite: " << node.getName() << std::endl;
		satellites.push_back(&node);
	}

	traverse(node);
}

int SatelliteTracker::getSatelliteCount()
{
	return satellites.size();
}

bool SatelliteTracker::isHeapInitialized()
{
	return matrixHeapInitialized;
}

void SatelliteTracker::clear()
{
	satellites.clear();
}

void SatelliteTracker::initializeMatrixHeap()
{
	if (getSatelliteCount() == 0) throw std::invalid_argument("Cant initialize a matrix heap with 0 satellites!");
	if (matrixHeapInitialized) deleteMatrixHeap();

	deconstructedMatrices = new DeconstructedMatrix * [getSatelliteCount()];

	for (int i = 0; i < getSatelliteCount(); i++)
	{
		std::cout << "initializing " << i << std::endl;
		DeconstructedMatrix* newDM = new DeconstructedMatrix;
		deconstructedMatrices[i] = newDM;
	}

	matrixHeapInitialized = true;
}

void SatelliteTracker::calculateMatrixHeap(int satellitesPerRing)
{
	if (!matrixHeapInitialized) throw std::invalid_argument("Cant calculate a nonexistent matrix heap!");
	for (int i = 0; i < getSatelliteCount(); i++)
	{
		DeconstructedMatrix *currentMatrix = deconstructedMatrices[i];
		auto currentSatellite = satellites.at(i);

		//std::cout << "i am at satellite number " << i << std::endl;
		//std::cout << "and wil use matrix index " << (int)(i / satellitesPerRing) << std::endl;

		int matrixIndex = ((int)(i / satellitesPerRing)+1)%2;
		int size = currentSatellite->getWorldMatrices().size();
		//std::cout << "matsize: " << size << std::endl;
		//std::cout << "used: " << size-1 << std::endl;
		//std::cout << "satellite " << i << ":___" << std::endl;
		for (int x = 0; x < size; x++)
		{
			//currentSatellite->getWorldMatrices().at(x).decompose((*currentMatrix).pos, (*currentMatrix).rot, (*currentMatrix).scale, (*currentMatrix).so);
			//std::cout << (*currentMatrix).pos << std::endl;
		}
		//std::cout << "end______" << i << std::endl;
		currentSatellite->getWorldMatrices().at(matrixIndex).decompose((*currentMatrix).pos, (*currentMatrix).rot, (*currentMatrix).scale, (*currentMatrix).so);
		//std::cout << "calculating " << i << ": " << (*currentMatrix).pos << std::endl;
	}
}

void SatelliteTracker::deleteMatrixHeap()
{
	if (!matrixHeapInitialized) throw std::invalid_argument("Cant delete a nonexistent matrix heap!");

	for (int i = 0; i < getSatelliteCount(); i++)
	{
		delete deconstructedMatrices[i];
	}

	delete[] deconstructedMatrices;
}

DeconstructedMatrix& SatelliteTracker::getDeconstructedMatrixAt(unsigned int index)
{
	if (!matrixHeapInitialized) throw std::invalid_argument("Cant get matrix from an uninitialized matrix heap!");
	if (index > getSatelliteCount() - 1) 
		throw std::invalid_argument("Matrix index " + std::to_string(index) + " out of bounds " + std::to_string(getSatelliteCount()-1));

	return *deconstructedMatrices[index];
}

std::vector<osg::MatrixTransform*>& SatelliteTracker::getSatellites()
{
	return satellites;
}




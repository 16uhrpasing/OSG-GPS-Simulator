#pragma once

#include <osg/MatrixTransform>
#include <osg/io_utils>

#include <osgText/Font>
#include <osgText/Text>

/**
 * Class to display important information about the tracking
 * show the 
 * - total amount of satellites
 * - total amount of visible satellites
 * - lat/lon location
 * - the normal plane equation of the tracked point
 * 
 * @param left, right, bottom, top -> window dimensions
 */
class InfoRenderer
{
public:
	InfoRenderer(double left, double right,
		double bottom, double top) : 
		HUDcamera(new osg::Camera),
		totalSatelliteText(new osgText::Text),
		visibleSatelliteText(new osgText::Text),
		locationText(new osgText::Text),
		planeEquationText(new osgText::Text)
	{
		HUDcamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		HUDcamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		HUDcamera->setRenderOrder(osg::Camera::POST_RENDER);
		HUDcamera->setAllowEventFocus(false);
		HUDcamera->setProjectionMatrix(
			osg::Matrix::ortho2D(left, right, bottom, top));
		HUDcamera->getOrCreateStateSet()->setMode(
			GL_LIGHTING, osg::StateAttribute::OFF);

		osg::ref_ptr<osgText::Text> headerText = createText(
			osg::Vec3(30.0f, top - 35.0f, 0.0f),
			"C++ GPS Simulator\nby 16uhrpasing",
			15.0f);

		totalSatelliteText = createText(osg::Vec3(30.0f, top - 75.0f, 0.0f), "", 15.0f);
		visibleSatelliteText = createText(osg::Vec3(30.0f, top - 95.0f, 0.0f), "", 15.0f);
		locationText = createText(osg::Vec3(30.0f, top - 115.0f, 0.0f), "", 15.0f);
		planeEquationText = createText(osg::Vec3(30.0f, top - 135.0f, 0.0f), "", 15.0f);

		osg::ref_ptr<osgText::Text> infoText = 
			createText(osg::Vec3(30.0f, top - 175.0f, 0.0f), "STRG + click on earth to track a location.", 15.0f);

		osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
		textGeode->addDrawable(headerText.release());
		textGeode->addDrawable(infoText.release());
		textGeode->addDrawable(totalSatelliteText);
		textGeode->addDrawable(visibleSatelliteText);
		textGeode->addDrawable(locationText);
		textGeode->addDrawable(planeEquationText);

		setTotalSatellites(0);
		setVisibleSatellites(0);
		setLocation("no location selected");
		setPlaneEq("no location selected");

		HUDcamera->addChild(textGeode.release());
	}
	
	inline osg::Camera* getHUDCamera() { return HUDcamera; }

	inline osgText::Text* createText(const osg::Vec3& pos,
		const std::string& content,
		float size)
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(g_font.get());
		text->setCharacterSize(size);
		text->setAxisAlignment(osgText::TextBase::XY_PLANE);
		text->setPosition(pos);
		text->setText(content);
		return text.release();
	}

	inline void setTotalSatellites(int count) {
		totalSatelliteText->setText("Total Satellites: " + std::to_string(count));
	}

	inline void setVisibleSatellites(int count) {
		visibleSatelliteText->setText("Visible Satellites: " + std::to_string(count));
	}

	inline void setLocation(std::string latLon) {
		locationText->setText("Estimated Location: " + latLon);
	}

	virtual void setPlaneEq(std::string planeEq) {
		planeEquationText->setText("Normal Plane: " + planeEq);
	}

protected:
	osg::ref_ptr<osg::Camera> HUDcamera;
	osg::ref_ptr<osgText::Text> totalSatelliteText;
	osg::ref_ptr<osgText::Text> visibleSatelliteText;
	osg::ref_ptr<osgText::Text> locationText;
	osg::ref_ptr<osgText::Text> planeEquationText;
	osg::ref_ptr<osgText::Font> g_font =
		osgText::readFontFile("files/fonts/VeraMono.ttf");
};

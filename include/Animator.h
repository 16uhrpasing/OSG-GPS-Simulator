#pragma once
#include <osgAnimation/BasicAnimationManager>
#include <osg/AnimationPath>

/**
 * Library to abstract animation
 *
 * create an animationPath to rotate an object around the Y-axis
 * @param center -> the center of the orbit
 * @param time -> the amount of time to do one round
 * @return --> animationpath which rotates a node
 */
osg::AnimationPath* createAnimationPath(osg::Vec3& center, float time);
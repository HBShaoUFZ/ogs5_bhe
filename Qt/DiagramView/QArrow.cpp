/**
 * \file QArrow.cpp
 * KR Initial implementation
 */

#include <math.h>
#include <QPainter>	
#include "QArrow.h"


/**
 * Creates an arrow as a QGraphicItem.
 * \param l Length of the arrow
 * \param a Orientation of the arrow
 * \param hl Length of the arrow head
 * \param hw Width of the arrow head
 * \param pen The pen for drawing the arrow
 */
QArrow::QArrow(float l, float a, float hl, float hw, QPen &pen, QGraphicsItem* parent) : QGraphicsItem(parent)
{
	_arrowLength = l;
	_arrowAngle  = a;
	_headLength  = hl;	
	_headWidth   = hw;	
	_arrowPen	 = pen;
}

/**
 * Creates an arrow as a QGraphicItem. Length and width of the arrow head are given by default values.
 * \param l Length of the arrow
 * \param a Orientation of the arrow
 * \param pen The pen for drawing the arrow
 */
QArrow::QArrow(float l, float a, QPen &pen, QGraphicsItem* parent) : QGraphicsItem(parent)
{
	_arrowLength = l;
	_arrowAngle  = a;
	_headLength  = 8;		// default headlength
	_headWidth   = 5;		// default headwidth
	_arrowPen	 = pen;
}

QArrow::~QArrow()
{
}

double QArrow::calcCos(double angle)
{
	return cos (angle*PI/180);
}

double QArrow::calcSin(double angle)
{	
	return sin (angle*PI/180);
}

/// The bounding box of the arrow
QRectF QArrow::boundingRect() const
{
	double deltaX = cos(_arrowAngle)*_arrowLength;    
	double deltaY = sin(_arrowAngle)*_arrowLength;

	return QRectF(0, 0, deltaX, deltaY);
}

/// Returns the length of the arrow.
double QArrow::getLength()
{
	return _arrowLength;
}

/// Returns the orientation of the arrow
double QArrow::getAngle()
{
	return _arrowAngle;
}

/**
 * Overloaded paint-method from QGraphicsItem.
 * Basically it draws a line with an arrowhead consisting of two short lines at the end
 */
void QArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	double deltaX    = calcCos(_arrowAngle)*_arrowLength;    
	double deltaY    = calcSin(_arrowAngle)*_arrowLength; 
	double theta     = atan(deltaY/deltaX);
	double theta2    = (deltaX < 0.0) ? (theta+PI) : theta; 
	int lengthdeltaX = -(int)(cos(theta2)*_headLength);   
	int lengthdeltaY = -(int)(sin(theta2)*_headLength);  
	int widthdeltaX  =  (int)(sin(theta2)*_headWidth); 
	int widthdeltaY  =  (int)(cos(theta2)*_headWidth);
	painter->setPen(_arrowPen);
	painter->drawLine(0, 0, deltaX, deltaY);  
	painter->drawLine(deltaX, deltaY, deltaX+lengthdeltaX+widthdeltaX, deltaY+lengthdeltaY-widthdeltaY);  
	painter->drawLine(deltaX, deltaY, deltaX+lengthdeltaX-widthdeltaX, deltaY+lengthdeltaY+widthdeltaY);
}

/// Changes orientation of the arrow.
void QArrow::setAngle(double a)
{
	_arrowAngle = a;
}

/// Changes the length of the arrow.
void QArrow::setLength(double l)
{
	_arrowLength = l;
}

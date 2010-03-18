/**
 * \file StratWindow.h
 * 2010/03/16 - KR Initial implementation
 */

#ifndef STRATWINDOW_H
#define STRATWINDOW_H

#include <QtGui/QWidget>
#include "ui_StratWindow.h"

namespace GEOLIB
{
	class StationBorehole;
}

/**
 * \brief Creates a window to visualise the stratigraphy of a borehole.
 */
class StratWindow : public QWidget, private Ui_StratWindow
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * \param station The borehole object to be visualised.
	 */
	StratWindow(GEOLIB::StationBorehole* station, QWidget* parent = 0);
	~StratWindow(void) {}

private:
	/// Automatically resize window based on the measurements of the borehole.
	void resizeWindow();

private slots:
	void on_closeButton_clicked();
};

#endif //STRATWINDOW_H

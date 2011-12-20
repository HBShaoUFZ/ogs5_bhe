/**
 * \file FEMConditionSetupDialog.h
 * 2011/11/07 KR Initial implementation
 */

#ifndef FEMCONDITIONSETUPDIALOG_H
#define FEMCONDITIONSETUPDIALOG_H

#include "ui_FEMConditionSetup.h"
#include "FEMCondition.h"

#include <QDialog>

class StrictDoubleValidator;

namespace GEOLIB {
	class GeoObject;
}


/**
 * \brief A dialog window for adding FEM Conditions based
 * on geometrica objects.
 */
class FEMConditionSetupDialog : public QDialog, private Ui_FEMConditionSetup
{
	Q_OBJECT

public:
	/// Constructor for creating a new FEM condition.
	FEMConditionSetupDialog(const std::string &associated_geometry, 
							const GEOLIB::GEOTYPE type, 
							const std::string &geo_name, 
							const GEOLIB::GeoObject* const geo_object, 
							QDialog* parent = 0);

	/// Constructor for editing an existing FEM condition.
	FEMConditionSetupDialog(FEMCondition &cond, QDialog* parent = 0);
	~FEMConditionSetupDialog(void);

private:
	void setupDialog();

	FEMCondition _cond;
	QLineEdit* _secondValueEdit;
	StrictDoubleValidator* _first_value_validator;
	StrictDoubleValidator* _second_value_validator;

private slots:
	/// Instructions if the OK-Button has been pressed.
	void accept();

	/// Instructions if the Cancel-Button has been pressed.
	void reject();

	void on_condTypeBox_currentIndexChanged(int index);

	void on_disTypeBox_currentIndexChanged(int index);

signals:
	void addFEMCondition(FEMCondition*);

};

#endif //FEMCONDITIONSETUPDIALOG_H
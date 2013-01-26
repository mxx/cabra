/*
 * HomeUI.h
 *
 *  Created on: Nov 28, 2012
 *      Author: mxx
 */

#ifndef HOMEUI_H_
#define HOMEUI_H_

#include <Wt/WContainerWidget>
#include "Session.h"
using namespace Wt;

namespace DataCenter
{
class DataImportUI;
class HomeUI: public WContainerWidget
{
public:
	HomeUI(WContainerWidget *parent);
	virtual ~HomeUI();

	void handleInternalPath(const std::string &internalPath);

protected:
	Wt::WStackedWidget *mainStack_;

	DataImportUI* importUI;

	Wt::WContainerWidget *links_;
	Wt::WAnchor *importUIAnchor_;

	Session session_;

	void onAuthEvent();
	void showDataImport();
};

} /* namespace DataCenter */
#endif /* HOMEUI_H_ */

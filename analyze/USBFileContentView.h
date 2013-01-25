/*
 * USBFileConetenView.h
 *
 *  Created on: Jan 6, 2013
 *      Author: mxx
 */

#ifndef USBFILECONETENVIEW_H_
#define USBFILECONETENVIEW_H_

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include "USBDataFilev2012.h"
using namespace Wt;
namespace DataCenter
{

class USBFileContentView: public Wt::WContainerWidget
{
public:
	USBFileContentView(WContainerWidget *parent = 0);
	virtual ~USBFileContentView();

	void InitView(const char* szTmpFile,const char* szFileName);

protected:

	WText* pFileName;
	WText* pDriverInfo;

	USBDataFilev2012 file;

};

} /* namespace DataCenter */
#endif /* USBFILECONETENVIEW_H_ */

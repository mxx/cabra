/*
 * USBFileConetenView.cpp
 *
 *  Created on: Jan 6, 2013
 *      Author: mxx
 */

#include "USBFileContentView.h"
#include <string>
#include <Wt/WBreak>
using namespace std;
namespace DataCenter
{

USBFileContentView::USBFileContentView(WContainerWidget *parent)
{
	new WText("FileContent", this);
	addWidget(new WBreak);
	pFileName = new WText(this);
	pDriverInfo = new WText(this);
}

USBFileContentView::~USBFileContentView()
{

}

void USBFileContentView::InitView(const char* szTmpFile, const char* szFileName)
{
	log("info") << "InitView:" << szTmpFile;
	pFileName->setText(szFileName);
	if (file.ParseFileName(pFileName->text().toUTF8().c_str()))
	{
		pFileName->setText(szFileName);
	}
	else
	{
		pFileName->setText(WString::fromUTF8("文件名格式错误："));
		pFileName->setText(pFileName->text() + szFileName);
		return;
	}

	string strErr;
	try
	{
		file.ReadFromFile(szTmpFile);

	} catch (USBDataFileException& e)
	{
		strErr = e.strType;
	} catch (...)
	{

	}
	pFileName->setText(
			pFileName->text() + WString::fromUTF8(":加载文件失败.") + strErr);

	return;

}

} /* namespace DataCenter */

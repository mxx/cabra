/*
 * HomeUI.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: mxx
 */

#include "HomeUI.h"
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WGridLayout>
#include <Wt/WLabel>
#include <Wt/WString>
#include <Wt/WAnchor>

#include <Wt/WStackedWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WApplication>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/AuthModel>
#include "DataImportUI.h"
using namespace Wt;
namespace DataCenter
{

HomeUI::HomeUI(WContainerWidget *parent) :
		WContainerWidget(parent), importUI(0)
{
	setContentAlignment(AlignCenter);

	session_.login().changed().connect(this, &HomeUI::onAuthEvent);

	Auth::AuthModel *authModel = new Auth::AuthModel(Session::auth(),
			session_.users(), this);
	authModel->addPasswordAuth(&Session::passwordAuth());
	authModel->addOAuth(Session::oAuth());

	Auth::AuthWidget *authWidget = new Auth::AuthWidget(session_.login());
	authWidget->setModel(authModel);
	authWidget->setRegistrationEnabled(false);

	WText *title = new WText(WString::fromUTF8("<h1>数据管理平台</h1>"));
	addWidget(title);

	addWidget(authWidget);

	mainStack_ = new WStackedWidget();
	mainStack_->setStyleClass("mainstack");
	addWidget(mainStack_);

	links_ = new WContainerWidget();
	links_->setStyleClass("links");
	links_->hide();
	addWidget(links_);

	importUIAnchor_ = new WAnchor("/dataimport", "Data Import", links_);
	importUIAnchor_->setLink(WLink(WLink::InternalPath, "/dataimport"));

	WApplication::instance()->internalPathChanged().connect(this,
			&HomeUI::handleInternalPath);

	authWidget->processEnvironment();

}

HomeUI::~HomeUI()
{
	// TODO Auto-generated destructor stub
}

void HomeUI::onAuthEvent()
{

	if (session_.login().loggedIn())
	{
		session_.setLastLogin();
		links_->hide();
		handleInternalPath(WApplication::instance()->internalPath());
	}
	else
	{
		mainStack_->clear();
		importUI = NULL;
		links_->hide();
	}
}
void HomeUI::handleInternalPath(const std::string &internalPath)
{
	if (session_.login().loggedIn())
	{
		if (internalPath == "/dataimport")
		{
			log("info") << "data import";
			showDataImport();
		}
		else
			WApplication::instance()->setInternalPath("/dataimport", true);
	}
}
void HomeUI::showDataImport()
{
	if (!importUI)
		importUI = new DataImportUI(&session_, mainStack_);

	mainStack_->setCurrentWidget(importUI);

	importUI->update();
	log("info") << "data import update";
//	importUI->removeStyleClass("selected-link");
}

}

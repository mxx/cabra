//============================================================================
// Name        : analyzer.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WGridLayout>
// c++0x only, for std::bind
// #include <functional>
#include "HomeUI.h"
#include "Session.h"

using namespace Wt;
using namespace DataCenter;
/*
 * A simple hello world application class which demonstrates how to react
 * to events, read input, and give feed-back.
 */
class DataAnalyzer : public WApplication
{
public:
  DataAnalyzer(const WEnvironment& env);

private:


};

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
DataAnalyzer::DataAnalyzer(const WEnvironment& env)
  : WApplication(env)
{
  setTitle(WString::fromUTF8("行车数据管理平台",true));
  WContainerWidget *w = new WContainerWidget(root());
  w->resize(WLength::Auto, WLength::Auto);
  HomeUI* home = new HomeUI(w);
  w->addWidget(home);
  root()->addWidget(w);
  setLocale(WT_LOCALE("cn"));
  WMessageResourceBundle* pTranslate = new WMessageResourceBundle();
  pTranslate->use(appRoot()+"resources/auth");
  setLocalizedStrings(pTranslate);
  useStyleSheet("resources/analyzer.css");
  log("info") << "constructed" ;
}


WApplication *createApplication(const WEnvironment& env)
{

  DataCenter::Session::configureAuth();
  return new DataAnalyzer(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &createApplication);
}


/*
 * Session.cpp
 *
 *  Created on: Dec 1, 2012
 *      Author: mxx
 */

#include "Session.h"
#include "Wt/Auth/AuthService"
#include "Wt/Auth/HashFunction"
#include "Wt/Auth/PasswordService"
#include "Wt/Auth/PasswordStrengthValidator"
#include "Wt/Auth/PasswordVerifier"
#include "Wt/Auth/GoogleService"
#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"

#include <Wt/WApplication>
#include <Wt/WLogger>

using namespace Wt;
namespace dbo = Wt::Dbo;

namespace DataCenter
{

class UnixCryptHashFunction: public Auth::HashFunction
{
public:
	virtual std::string compute(const std::string& msg,
			const std::string& salt) const
	{
		std::string md5Salt = "$1$" + salt;
		return crypt(msg.c_str(), md5Salt.c_str());
	}

	virtual bool verify(const std::string& msg, const std::string& salt,
			const std::string& hash) const
	{
		return crypt(msg.c_str(), hash.c_str()) == hash;
	}

	virtual std::string name() const
	{
		return "crypt";
	}
};

class MyOAuth: public std::vector<const Auth::OAuthService *>
{
public:
	~MyOAuth()
	{
		for (unsigned i = 0; i < size(); ++i)
			delete (*this)[i];
	}
};

Wt::Auth::AuthService myAuthService;
Wt::Auth::PasswordService myPasswordService(myAuthService);
MyOAuth myOAuthServices;


void Session::configureAuth()
{
	myAuthService.setAuthTokensEnabled(false);

	Auth::PasswordVerifier *verifier = new Auth::PasswordVerifier();
	verifier->addHashFunction(new Auth::BCryptHashFunction(7));

	// We want to still support users registered in the pre - Wt::Auth
	// version of the hangman example
	verifier->addHashFunction(new UnixCryptHashFunction());

	myPasswordService.setVerifier(verifier);
	myPasswordService.setStrengthValidator(
			new Auth::PasswordStrengthValidator());
	myPasswordService.setAttemptThrottlingEnabled(true);
}

Session::Session() :
		sqlite3_(WApplication::instance()->appRoot() + "datacenter.db")
{
	session_.setConnection(sqlite3_);
	sqlite3_.setProperty("show-queries", "true");

	session_.mapClass < Worker > ("user");
	session_.mapClass < AuthInfo > ("auth_info");
	session_.mapClass < AuthInfo::AuthIdentityType > ("auth_identity");
	session_.mapClass < AuthInfo::AuthTokenType > ("auth_token");

	users_ = new UserDatabase(session_);

	dbo::Transaction transaction(session_);
	try
	{
		session_.createTables();
		Wt::log("info") << "Database created";
		/*
		 * Add a default guest/guest account
		 */
		Auth::User guestUser = users_->registerNew();
		guestUser.addIdentity(Auth::Identity::LoginName,
				WString::fromUTF8("guest"));
		myPasswordService.updatePassword(guestUser, WString::fromUTF8("guest"));

		Wt::log("info") << "user guest created";
	} catch (...)
	{
		Wt::log("info") << "Using existing database";
	}

	transaction.commit();
}

Session::~Session()
{
	delete users_;
}

dbo::ptr<Worker> Session::user() const
{
	if (login_.loggedIn())
	{
		dbo::ptr < AuthInfo > authInfo = users_->find(login_.user());
		dbo::ptr < Worker > user = authInfo->user();

		if (!user)
		{
			user = session_.add(new Worker());
			authInfo.modify()->setUser(user);
		}

		return user;
	}
	else
		return dbo::ptr<Worker>();
}

std::string Session::userName() const
{
	if (login_.loggedIn())
		return login_.user().identity(Auth::Identity::LoginName).toUTF8();
	else
		return std::string();
}

void Session::setLastLogin(void)
{
	dbo::Transaction transaction(session_);

	dbo::ptr < Worker > u = user();
	if (u)
	{
		u.modify()->last_login = WDateTime::currentDateTime();
		u.modify()->name = userName();
	}

	transaction.commit();
}

Auth::AbstractUserDatabase& Session::users()
{
	return *users_;
}

const Auth::AuthService& Session::auth()
{
	return myAuthService;
}

const Auth::AbstractPasswordService& Session::passwordAuth()
{
	return myPasswordService;
}

const std::vector<const Auth::OAuthService *>& Session::oAuth()
{
	return myOAuthServices;
}

}

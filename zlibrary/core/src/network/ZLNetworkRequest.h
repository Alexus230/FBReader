/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLNETWORKREQUEST_H__
#define __ZLNETWORKREQUEST_H__

#include <string>
#include <vector>

#include <shared_ptr.h>

#include <ZLNetworkSSLCertificate.h>


class ZLNetworkRequest {

public:
	typedef std::vector<shared_ptr<ZLNetworkRequest> > Vector;

public:
	class Listener {

	protected:
		Listener();

	public:
		virtual ~Listener();
		virtual void finished(const std::string &error = std::string()) = 0;
		virtual void showPercent(int ready, int full);

	friend class ZLNetworkRequest;
	};

protected:
	ZLNetworkRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate);

public:
	virtual bool handleHeader(void *ptr, size_t size);
	virtual bool handleContent(void *ptr, size_t size) = 0;

	virtual bool doBefore() = 0;

	// When error is empty return false MUST make request fail; when error is not empty return value MUST be ignored
	virtual bool doAfter(const std::string &error) = 0;

protected:
	void setErrorMessage(const std::string &message);

public:
	const std::string &errorMessage() const;

	const std::string &url() const;
	const ZLNetworkSSLCertificate &sslCertificate() const;

public: // HTTP parameters
	enum AuthenticationMethod {
		NO_AUTH, BASIC
	};
	void setupAuthentication(AuthenticationMethod method, const std::string &userName, const std::string &password);
	const std::string &userName() const;
	const std::string &password() const;
	AuthenticationMethod authenticationMethod() const;

	void setRedirectionSupported(bool supported);
	bool isRedirectionSupported() const;

	const std::vector<std::pair<std::string, std::string> > &postParameters() const;
	void setPostParameters(const std::vector<std::pair<std::string, std::string> > &parameters);


public: //listeners methods
	bool hasListener() const;
	void setListener(shared_ptr<Listener> listener);
	void setPercent(int ready, int full);

protected:
	void finished(const std::string &error);
	shared_ptr<Listener> listener() const;

private:
	const std::string myURL;
	const ZLNetworkSSLCertificate &mySSLCertificate;
	std::string myErrorMessage;

	std::string myUserName;
	std::string myPassword;
	AuthenticationMethod myAuthenticationMethod;

	bool myRedirectionSupported;

	std::vector<std::pair<std::string, std::string> > myPostParameters;

	shared_ptr<Listener> myListener;

private: // disable copying
	ZLNetworkRequest(const ZLNetworkRequest &);
	const ZLNetworkRequest &operator = (const ZLNetworkRequest &);
};

inline void ZLNetworkRequest::setupAuthentication(AuthenticationMethod method, const std::string &userName, const std::string &password) {
	myAuthenticationMethod = method;
	myUserName = userName;
	myPassword = password;
}
inline const std::string &ZLNetworkRequest::userName() const { return myUserName; }
inline const std::string &ZLNetworkRequest::password() const { return myPassword; }
inline ZLNetworkRequest::AuthenticationMethod ZLNetworkRequest::authenticationMethod() const { return myAuthenticationMethod; }
inline void ZLNetworkRequest::setRedirectionSupported(bool supported) { myRedirectionSupported = supported; }
inline bool ZLNetworkRequest::isRedirectionSupported() const { return myRedirectionSupported; }
inline const std::vector<std::pair<std::string, std::string> > &ZLNetworkRequest::postParameters() const { return myPostParameters; }

inline shared_ptr<ZLNetworkRequest::Listener> ZLNetworkRequest::listener() const { return myListener; }

#endif /* __ZLNETWORKREQUEST_H__ */

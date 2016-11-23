#ifndef __VSCALE_H__
#define __VSCALE_H__

#include <json/json.h>
#include <memory>
#include <string>

#define DEFAULT_ACCOUNT_API_URL "https://api.vscale.io/v1/account"
#define DEFAULT_SCALETS_API_URL "https://api.vscale.io/v1/scalets"
#define DEFAULT_SERVER_TAGS_API_URL "https://api.vscale.io/v1/scalets/tags"

namespace vscale {

using std::string;
typedef Json::Value JsonValue;

class Vscale {
public:
	Vscale(const string &token, const string &url);
	virtual ~Vscale();

	virtual void List(JsonValue &) const = 0;
	virtual void Create(const JsonValue &, JsonValue &) const = 0;
	virtual void Update(int, const JsonValue &, JsonValue &) const = 0;
	virtual void Delete(int, JsonValue &) const = 0;
	virtual void Info(int, JsonValue &) const = 0;

protected:
	struct PrivateData;
	std::shared_ptr<PrivateData> m_data;
};

class Account : public Vscale {
public:
	Account(const string &token, const string url=DEFAULT_ACCOUNT_API_URL);
	virtual ~Account();

	virtual void List(JsonValue &) const {}
	virtual void Create(const JsonValue &, JsonValue &) const {}
	virtual void Update(int, const JsonValue &, JsonValue &) const {}
	virtual void Delete(int, JsonValue &) const {}
	virtual void Info(int, JsonValue &response) const;
};

class Scalets : public Vscale {
public:
	Scalets(const string &token, const string &url=DEFAULT_SCALETS_API_URL);
	virtual ~Scalets();

	virtual void List(JsonValue &response) const;
	virtual void Create(const JsonValue &params, JsonValue &response) const;
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Delete(int id, JsonValue &response) const;
	virtual void Info(int id, JsonValue &response) const = 0;
	virtual void Restart(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Rebuild(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Stop(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Start(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Upgrade(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Tasks(JsonValue &response) const;
	virtual void Backup(int id, const JsonValue &params, JsonValue &response) const;
};

class ServerTags : public Vscale {
public:
	ServerTags(const string &token, const string &url=DEFAULT_SERVER_TAGS_API_URL);
	virtual ~ServerTags();

	virtual void List(JsonValue &response) const;
	virtual void Create(const JsonValue &params, JsonValue &response) const;
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Delete(int id, JsonValue &response) const;
	virtual void Info(int, JsonValue &) const {}
};

} // namespace vscale

#endif // __VSCALE_H__

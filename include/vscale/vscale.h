#ifndef __VSCALE_H__
#define __VSCALE_H__

#include <json/json.h>
#include <memory>
#include <string>

#define VSCALE_ACCOUNT_API_URL "https://api.vscale.io/v1/account"
#define VSCALE_SCALETS_API_URL "https://api.vscale.io/v1/scalets"
#define VSCALE_SERVER_TAGS_API_URL "https://api.vscale.io/v1/scalets/tags"
#define VSCALE_BACKUP_API_URL "https://api.vscale.io/v1/backups"
#define VSCALE_SSHKEYS_API_URL "https://api.vscale.io/v1/sshkeys"
#define VSCALE_NOTIFICATIONS_API_URL "https://api.vscale.io/v1/billing/notify"
#define VSCALE_DOMAIN_API_URL "https://api.vscale.io/v1/domains/"

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
	Account(const string &token, const string url=VSCALE_ACCOUNT_API_URL);
	virtual ~Account();

	virtual void List(JsonValue &) const {}
	virtual void Create(const JsonValue &, JsonValue &) const {}
	virtual void Update(int, const JsonValue &, JsonValue &) const {}
	virtual void Delete(int, JsonValue &) const {}
	virtual void Info(int, JsonValue &response) const;
};

class Scalets : public Vscale {
public:
	Scalets(const string &token, const string &url=VSCALE_SCALETS_API_URL);
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
	ServerTags(const string &token, const string &url=VSCALE_SERVER_TAGS_API_URL);
	virtual ~ServerTags();

	virtual void List(JsonValue &response) const;
	virtual void Create(const JsonValue &params, JsonValue &response) const;
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const;
	virtual void Delete(int id, JsonValue &response) const;
	virtual void Info(int, JsonValue &) const {}
};

class Backup : public Vscale {
public:
	Backup(const string &token, const string &url=VSCALE_BACKUP_API_URL);
	virtual ~Backup();

	virtual void List(JsonValue &response) const;
	virtual void Create(const JsonValue &params, JsonValue &response) const {}
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const {}
	virtual void Delete(int id, JsonValue &response) const;
	virtual void Delete(const string &id, JsonValue &response) const;
	virtual void Info(int, JsonValue &) const;
	virtual void Info(const string &id, JsonValue &response) const;
};

class Background : public Vscale {
public:
	Background(const string &token);
	virtual ~Background();

	virtual void List(JsonValue &) const {}
	virtual void Create(const JsonValue &, JsonValue &) const {}
	virtual void Update(int, const JsonValue &, JsonValue &) const {}
	virtual void Delete(int, JsonValue &) const {}
	virtual void Info(int, JsonValue &) const {}

	virtual void Locations(JsonValue &response) const;
	virtual void Images(JsonValue &response) const;
};

class Configurations : public Vscale {
public:
	Configurations(const string &token);
	virtual ~Configurations();

	virtual void List(JsonValue &) const {}
	virtual void Create(const JsonValue &, JsonValue &) const {}
	virtual void Update(int, const JsonValue &, JsonValue &) const {}
	virtual void Delete(int, JsonValue &) const {}
	virtual void Info(int, JsonValue &) const {}

	virtual void RPlans(JsonValue &response) const;
	virtual void BillingPrices(JsonValue &response) const;
};

class SSHKeys : public Vscale {
public:
	SSHKeys(const string &token, const string &url=VSCALE_SSHKEYS_API_URL);
	virtual ~SSHKeys();

	virtual void List(JsonValue &) const;
	virtual void Create(const JsonValue &, JsonValue &) const;
	virtual void Update(int, const JsonValue &, JsonValue &) const {}
	virtual void Delete(int, JsonValue &) const;
	virtual void Info(int, JsonValue &) const {}
};

class Notifications : public Vscale {
public:
	Notifications(const string &token, const string &url=VSCALE_NOTIFICATIONS_API_URL);
	virtual ~Notifications();

	virtual void List(JsonValue &) const {}
	virtual void Create(const JsonValue &, JsonValue &) const {}
	virtual void Update(int, const JsonValue &, JsonValue &) const;
	virtual void Delete(int, JsonValue &) const {}
	virtual void Info(int, JsonValue &) const;
};

class Billing : public Vscale {
public:
	Billing(const string &token);
	virtual ~Billing();

	virtual void List(JsonValue &) const {}
	virtual void Create(const JsonValue &, JsonValue &) const {}
	virtual void Update(int, const JsonValue &, JsonValue &) const {}
	virtual void Delete(int, JsonValue &) const {}
	virtual void Info(int, JsonValue &) const {}

	virtual void Balance(JsonValue &) const;
	virtual void Payments(JsonValue &) const;
	virtual void Consumption(const string &start_date, const string &end_date, JsonValue &response) const;
};

class Domain : public Vscale {
public:
	Domain(const string &token, const string &url=VSCALE_DOMAIN_API_URL);
	virtual ~Domain();

	virtual void List(JsonValue &) const;
	virtual void Create(const JsonValue &, JsonValue &) const;
	virtual void Update(int, const JsonValue &, JsonValue &) const;
	virtual void Delete(int, JsonValue &) const;
	virtual void Info(int, JsonValue &) const;
};

} // namespace vscale

#endif // __VSCALE_H__

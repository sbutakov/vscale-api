#include <vscale/vscale.h>
#include <curl/curl.h>

#define SUCCESS_RESPONSE_CODE_200 			200
#define SUCCESS_RESPONSE_CODE_204 			204
#define VSCALE_ERROR_MESSAGE				"VSCALE-ERROR-MESSAGE"
#define DEFAULT_BAD_REQUEST					"bad request with code"
#define HEADER_TOKEN(A) 					"X-Token: " + A
#define HEADER_APPLICATION_JSON 			"Content-Type: application/json;charset=UTF-8"

#define VSCALE_ACCOUNT_API_URL 				"https://api.vscale.io/v1/account"
#define VSCALE_SCALETS_API_URL 				"https://api.vscale.io/v1/scalets"
#define VSCALE_SERVER_TAGS_API_URL 			"https://api.vscale.io/v1/scalets/tags"
#define VSCALE_BACKUP_API_URL 				"https://api.vscale.io/v1/backups"
#define VSCALE_SSHKEYS_API_URL 				"https://api.vscale.io/v1/sshkeys"
#define VSCALE_NOTIFICATIONS_API_URL 		"https://api.vscale.io/v1/billing/notify"
#define VSCALE_DOMAIN_API_URL 				"https://api.vscale.io/v1/domains/"
#define VSCALE_DOMAIN_TAGS_API_URL 			"https://api.vscale.io/v1/domains/tags/"
#define VSCALE_PTR_RECORDS_API_URL 			"https://api.vscale.io/v1/domains/ptr/"
#define VSCALE_LOCATIONS_API_URL 			"https://api.vscale.io/v1/locations"
#define VSCALE_IMAGES_API_URL 				"https://api.vscale.io/v1/images"
#define VSCALE_RPLANS_API_URL 				"https://api.vscale.io/v1/rplans"
#define VSCALE_BILLING_PRICES_API_URL 		"https://api.vscale.io/v1/billing/prices"
#define VSCALE_BILLING_BALANCE_API_URL 		"https://api.vscale.io/v1/billing/balance"
#define VSCALE_BILLING_PAYMENTS_API_URL 	"https://api.vscale.io/v1/billing/payments"
#define VSCALE_BILLING_CONSUMPTION_API_URL 	"https://api.vscale.io/v1/billing/consumption"

namespace vscale {

class HttpRequest {
public:
	enum MethodRequest {
		mrGET,
		mrPOST,
		mrPUT,
		mrPATCH,
		mrDELETE
	};

	HttpRequest(): m_headers(nullptr) {
		m_curl = curl_easy_init();
	}

	~HttpRequest() {
		ClearHeaders();

		if (m_curl)
			curl_easy_cleanup(m_curl);
	}

	HttpRequest &SetURL(const string &url) {
		curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
		if (url.compare(0, 8 , "https://") == 0) {
			curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
		}
		return *this;
	}

	HttpRequest &SetHeader(const string &value) {
		m_headers = curl_slist_append(m_headers, value.c_str());
		return *this;
	}

	HttpRequest &ClearHeaders() {
		if (m_headers != nullptr) {
			curl_slist_free_all(m_headers);
			m_headers = nullptr;
		}
		return *this;
	}

	static size_t WriteFuncCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
		size_t realsize = size * nmemb;
		if (realsize <= 0)
			return 0;
		string *response = (string *) userdata;
		response->reserve(realsize);
		response->append(ptr);

		return realsize;
	}

	static size_t HeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata) {
		size_t realsize = size * nitems;
		if (realsize <= 0)
			return 0;
		string res(buffer);
		string *header = (string *) userdata;
		while (!res.empty()) {
			string::size_type pos = res.find("\n");
			if (pos == string::npos)
				return realsize;
			else {
				string tmp = res.substr(0, pos);
				size_t len = strlen(VSCALE_ERROR_MESSAGE);
				if (tmp.compare(0, len, VSCALE_ERROR_MESSAGE) == 0) {
					header->append(tmp.substr(len + 1, tmp.size() - len));
					return realsize;
				}
				res.erase(0, pos + 1);
			}
		}
		return realsize;
	}

	string Perform(MethodRequest method=mrGET, const string &data="") {
		CURLcode code;
		switch (method) {
			case mrGET:
				code = curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);
				break;
			case mrPOST:
				code = curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
				break;
			case mrPUT:
				code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
				break;
			case mrPATCH:
				code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PATCH");
				break;
			case mrDELETE:
				code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
				break;
			default:
				code = CURLE_FAILED_INIT;
		}

		if (code == CURLE_OK && !data.empty()) {
			curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data.c_str()));
			code = curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());
		}

		if (m_headers != nullptr && code == CURLE_OK)
			code = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);

		string response, vscale_error_msg;
		if (code == CURLE_OK) {
			curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteFuncCallback);
			curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);
			curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
			curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, &vscale_error_msg);
			curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 30);
			curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 30);
		}

		if (code == CURLE_OK)
			code = curl_easy_perform(m_curl);

		if (code == CURLE_OK) {
			long response_code = 0;
			code = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);
			if (code == CURLE_OK && (response_code != SUCCESS_RESPONSE_CODE_200 || response_code != SUCCESS_RESPONSE_CODE_204)) {
				if (!vscale_error_msg.empty())
					throw BadRequest(vscale_error_msg);
				throw BadRequest(DEFAULT_BAD_REQUEST + std::to_string(response_code));
			}
		}

		return response;
	}

private:
	CURL *m_curl;
	struct curl_slist *m_headers;
};

string AppendURLPath(const string &url, const string &path) {
	if (url.compare(url.size() - 1, 1, "/") == 0)
		return url + path;
	return url + "/" + path;
}

string TruncateURLPath(const string &src) {
	string res = src;
	const string::size_type pos = res.rfind('/');
	if (pos != string::npos)
		return res.substr(pos + 1);
	return res;
}

BadRequest::BadRequest(const string &what): m_what(what) {}

const char* BadRequest::what() const throw() {
	return m_what.c_str();
}

struct VscalePrivateData::PrivateData {
	string url, token;
	HttpRequest http;
};

VscalePrivateData::VscalePrivateData(const string &url, const string &token)
		: m_data(new PrivateData)
{
	m_data->url = url;
	m_data->token = token;
	m_data->http.SetURL(url);
	m_data->http.SetHeader(HEADER_TOKEN(token));
}

Account::Account(const string &token): VscalePrivateData(VSCALE_ACCOUNT_API_URL, token) {}
Account::~Account() {}

void Account::Info(JsonValue &response) const {
	response = m_data->http.Perform();
}

Scalets::Scalets(const string &token): VscalePrivateData(VSCALE_SCALETS_API_URL, token) {}
Scalets::~Scalets() {}

void Scalets::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void Scalets::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token));
}

void Scalets::Delete(int id, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Info(int id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform();
	m_data->http.SetURL(m_data->url);
}

void Scalets::Restart(int id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/restart"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, "{\"id\": \"" + std::to_string(id) + "\"}");
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Rebuild(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/rebuild"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Stop(int id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/stop"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, "{\"id\": \"" + std::to_string(id) + "\"}");
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Start(int id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/start"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, "{\"id\": \"" + std::to_string(id) + "\"}");
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Upgrade(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/upgrade"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Tasks(JsonValue &response) const {
	response = m_data->http.SetURL(AppendURLPath(TruncateURLPath(m_data->url), "/task")).Perform();
	m_data->http.SetURL(m_data->url);
}

void Scalets::Backup(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/backup"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

ServerTags::ServerTags(const string &token): VscalePrivateData(VSCALE_SERVER_TAGS_API_URL, token) {}
ServerTags::~ServerTags() {}

void ServerTags::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void ServerTags::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token));
}

void ServerTags::Update(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform(HttpRequest::mrPUT, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void ServerTags::Delete(int id, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

Backup::Backup(const string &token): VscalePrivateData(VSCALE_BACKUP_API_URL, token) {}
Backup::~Backup() {}

void Backup::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void Backup::Delete(const string &id, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.SetURL(AppendURLPath(m_data->url, id))
			.Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Backup::Info(const string &id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, id));
	response = m_data->http.Perform();
	m_data->http.SetURL(m_data->url);
}

Background::Background(const string &token): VscalePrivateData("", token) {}
Background::~Background() {}

void Background::Locations(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_LOCATIONS_API_URL);
	response = m_data->http.Perform();
}

void Background::Images(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_IMAGES_API_URL);
	response = m_data->http.Perform();
}

Configurations::Configurations(const string &token): VscalePrivateData("", token) {}
Configurations::~Configurations() {}

void Configurations::RPlans(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_RPLANS_API_URL);
	response = m_data->http.Perform();
}

void Configurations::BillingPrices(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_BILLING_PRICES_API_URL);
	response = m_data->http.Perform();
}

SSHKeys::SSHKeys(const string &token): VscalePrivateData(VSCALE_SSHKEYS_API_URL, token) {}
SSHKeys::~SSHKeys() {}

void SSHKeys::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void SSHKeys::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token));
}

void SSHKeys::Delete(int id, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

Notifications::Notifications(const string &token): VscalePrivateData(VSCALE_NOTIFICATIONS_API_URL, token) {}
Notifications::~Notifications() {}

void Notifications::Update(const JsonValue &params, JsonValue &response) const {
	response = m_data->http.Perform(HttpRequest::mrPUT, params.toStyledString());
}

void Notifications::Info(JsonValue &response) const {
	response = m_data->http.Perform();
}

Billing::Billing(const string &token): VscalePrivateData("", token) {}
Billing::~Billing() {}

void Billing::Balance(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_BILLING_BALANCE_API_URL);
	response = m_data->http.Perform();
}

void Billing::Payments(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_BILLING_PAYMENTS_API_URL);
	response = m_data->http.Perform();
}

void Billing::Consumption(const string &start_date, const string &end_date, JsonValue &response) const {
	m_data->http.SetURL(VSCALE_BILLING_CONSUMPTION_API_URL "?start=" + start_date + "&end=" + end_date);
	response = m_data->http.Perform();
}

Domain::Domain(const string &token): VscalePrivateData(VSCALE_DOMAIN_API_URL, token) {}
Domain::~Domain() {}

void Domain::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void Domain::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token));
}

void Domain::Update(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform(HttpRequest::mrPATCH, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Domain::Delete(int id, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void Domain::Info(int id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform();
	m_data->http.SetURL(m_data->url);
}

DomainRecord::DomainRecord(const string &token): VscalePrivateData(VSCALE_DOMAIN_API_URL, token) {}
DomainRecord::~DomainRecord() {}

void DomainRecord::List(int domain_id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(domain_id) + "/records"));
	response = m_data->http.Perform();
	m_data->http.SetURL(m_data->url);
}

void DomainRecord::Create(int domain_id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(domain_id) + "/records"))
			.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void DomainRecord::Update(int domain_id, int record_id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(domain_id) + "/records/" + std::to_string(record_id)))
			.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void DomainRecord::Delete(int domain_id, int record_id) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(domain_id) + "/records/" + std::to_string(record_id)))
			.SetHeader(HEADER_APPLICATION_JSON).Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void DomainRecord::Info(int domain_id, int record_id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(domain_id) + "/records/" + std::to_string(record_id)));
	response = m_data->http.Perform();
}

DomainsTags::DomainsTags(const string &token): VscalePrivateData(VSCALE_DOMAIN_TAGS_API_URL, token) {}
DomainsTags::~DomainsTags() {}

void DomainsTags::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void DomainsTags::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token));
}

void DomainsTags::Update(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform(HttpRequest::mrPUT, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void DomainsTags::Delete(int id) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void DomainsTags::Info(int id, JsonValue &response) const {
	response = m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id))).Perform();
}

PTRRecords::PTRRecords(const string &token): VscalePrivateData(VSCALE_PTR_RECORDS_API_URL, token) {}
PTRRecords::~PTRRecords() {}

void PTRRecords::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void PTRRecords::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token));
}

void PTRRecords::Update(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform(HttpRequest::mrPUT, params.toStyledString());
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void PTRRecords::Delete(int id) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->http.ClearHeaders().SetHeader(HEADER_TOKEN(m_data->token)).SetURL(m_data->url);
}

void PTRRecords::Info(int id, JsonValue &response) const {
	response = m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id))).Perform();
}
} // namespace vscale

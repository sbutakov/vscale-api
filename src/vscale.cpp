#include <vscale/vscale.h>
#include <curl/curl.h>

#define TOKEN(A) "X-Token: " + A
#define HEADER_APPLICATION_JSON "Content-Type: application/json;charset=UTF-8"

#define VSCALE_LOCATIONS_API_URL "https://api.vscale.io/v1/locations"
#define VSCALE_IMAGES_API_URL "https://api.vscale.io/v1/images"
#define VSCALE_RPLANS_API_URL "https://api.vscale.io/v1/rplans"
#define VSCALE_BILLING_PRICES_API_URL "https://api.vscale.io/v1/billing/prices"
#define VSCALE_BILLING_BALANCE_API_URL "https://api.vscale.io/v1/billing/balance"
#define VSCALE_BILLING_PAYMENTS_API_URL "https://api.vscale.io/v1/billing/payments"
#define VSCALE_BILLING_CONSUMPTION_API_URL "https://api.vscale.io/v1/billing/consumption"

namespace vscale {

static size_t WriteFuncCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	size_t realsize = size * nmemb;
	if (realsize <= 0)
		return 0;
	string *response = (string *) userdata;
	response->reserve(realsize);
	response->append(ptr);

	return realsize;
}

class HttpRequest {
public:
	enum MethodRequest {
		mrGET,
		mrPOST,
		mrPUT,
		mrPATCH,
		mrDELETE
	};

	HttpRequest(): m_curl(curl_easy_init()), m_headers(nullptr) {}

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
		if (m_headers) {
			curl_slist_free_all(m_headers);
			m_headers = nullptr;
		}
		return *this;
	}

	string Perform(MethodRequest method=mrGET, const string &data="") {
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteFuncCallback);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response);
		curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 30);
		curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 30);

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

		if (m_headers && code == CURLE_OK)
			code = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);

		if (code == CURLE_OK)
			curl_easy_perform(m_curl);

		return m_response;
	}

private:
	CURL *m_curl;
	struct curl_slist *m_headers;
	string m_response;
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

struct Vscale::PrivateData {
	string token;
	string url;
	HttpRequest http;
};

Vscale::Vscale(const string &token, const string &url): m_data(new PrivateData) {
	m_data->token = token;
	m_data->url = url;

	m_data->http.ClearHeaders();
	m_data->http.SetHeader(TOKEN(token));
	m_data->http.SetURL(url);
}

Vscale::~Vscale() {}

Account::Account(const string &token, const string url): Vscale(token, url) {}
Account::~Account() {}

void Account::Info(int, JsonValue &response) const {
	response = m_data->http.Perform();
}

Scalets::Scalets(const string &token, const string &url): Vscale(token, url) {}
Scalets::~Scalets() {}

void Scalets::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void Scalets::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token));
}

void Scalets::Update(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform(HttpRequest::mrPUT, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Delete(int id, JsonValue &response) const {
	m_data->.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Info(int id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform();
	m_data->http.SetURL(m_data->url);
}

void Scalets::Restart(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/restart"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Rebuild(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/rebuild"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Stop(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/stop"));
	m_data->.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Start(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/start"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPATCH, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Upgrade(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/upgrade"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Scalets::Tasks(JsonValue &response) const {
	response = m_data->http.SetURL(AppendURLPath(TruncateURLPath(m_data->url), "/task")).Perform();
	m_data->http.SetURL(m_data->url);
}

void Scalets::Backup(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/backup"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

ServerTags::ServerTags(const string &token, const string &url): Vscale(token, url) {}
ServerTags::~ServerTags() {}

void ServerTags::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void ServerTags::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token));
}

void ServerTags::Update(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform(HttpRequest::mrPUT, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void ServerTags::Delete(int id, JsonValue &response) const {
	m_data->.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

Backup::Backup(const string &token, const string &url): Vscale(token, url) {}
Backup::~Backup() {}

void Backup::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void Backup::Delete(int id, JsonValue &response) const {
	m_data->.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Backup::Delete(const string &id, JsonValue &response) const {
	m_data->.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->.SetURL(AppendURLPath(m_data->url, id))
			.Perform(HttpRequest::mrDELETE);
	m_data->.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

void Backup::Info(int id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform();
	m_data->http.SetURL(m_data->url);
}

void Backup::Info(const string &id, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, id));
	response = m_data->http.Perform();
	m_data->http.SetURL(m_data->url);
}

Background::Background(const string &token): Vscale(token, "") {}
Background::~Background() {}

void Background::Locations(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_LOCATIONS_API_URL);
	response = m_data->http.Perform();
}

void Background::Images(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_IMAGES_API_URL);
	response = m_data->http.Perform();
}

Configurations::Configurations(const string &token): Vscale(token, "") {}
Configurations::~Configurations() {}

void Configurations::RPlans(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_RPLANS_API_URL);
	response = m_data->http.Perform();
}

void Configurations::BillingPrices(JsonValue &response) const {
	m_data->http.SetURL(VSCALE_BILLING_PRICES_API_URL);
	response = m_data->http.Perform();
}

SSHKeys::SSHKeys(const string &token, const string &url): Vscale(token, url) {}
SSHKeys::~SSHKeys() {}

void SSHKeys::List(JsonValue &response) const {
	response = m_data->http.Perform();
}

void SSHKeys::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token));
}

void SSHKeys::Delete(int id, JsonValue &response) const {
	m_data->.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->.SetURL(AppendURLPath(m_data->url, std::to_string(id)))
			.Perform(HttpRequest::mrDELETE);
	m_data->.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

Notifications::Notifications(const string &token, const string &url): Vscale(token, url) {}
Notifications::~Notifications();

void Notifications::Update(int, const JsonValue &params, JsonValue &response) const {
	response = m_data->http.Perform(HttpRequest::mrPUT, params.asString());
}

void Notifications::Info(int, JsonValue &response) const {
	response = m_data->http.Perform();
}

Billing::Billing(const string &token): Vscale(token, "") {}
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
	m_data->http.SetURL(VSCALE_BILLING_CONSUMPTION_API_URL + "?start=" + start_date + "&end=" + end_date);
	response = m_data->http.Perform();
}

} // namespace vscale

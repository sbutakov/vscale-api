#include <vscale/vscale.h>
#include <curl/curl.h>

#define TOKEN(A) "X-Token: " + A
#define HEADER_APPLICATION_JSON "Content-Type: application/json;charset=UTF-8"

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

	string Perform(MethodRequest method, const string &data="") {
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteFuncCallback);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response);
		// TODO: SetTimeout
		// TODO: Other settings
		CURLcode code;
		switch (method) {
			case mrGET: 
				code = curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);
				break;
			case mrPOST:
			case mrPATCH:
				method == mrPOST ? curl_easy_setopt(m_curl, CURLOPT_POST, 1L)
								 : curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PATCH");
				curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data.c_str()));
				code = curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());
				break;
			case mrDELETE:
				code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
				break;
			default:
				code = CURLE_FAILED_INIT;
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
	response = m_data->http.Perform(HttpRequest::mrGET);
}

Scalets::Scalets(const string &token, const string &url): Vscale(token, url) {}
Scalets::~Scalets() {}

void Scalets::List(JsonValue &response) const {
	response = m_data->http.Perform(HttpRequest::mrGET);
}

void Scalets::Create(const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token));
}

void Scalets::Update(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id)));
	response = m_data->http.Perform(HttpRequest::mrPATCH, params.asString());
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
	response = m_data->http.Perform(HttpRequest::mrGET);
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
	response = m_data->http.SetURL(AppendURLPath(TruncateURLPath(m_data->url), "/task"))
			.Perform(HttpRequest::mrGET);
	m_data->http.SetURL(m_data->url);
}

void Scalets::Backup(int id, const JsonValue &params, JsonValue &response) const {
	m_data->http.SetURL(AppendURLPath(m_data->url, std::to_string(id) + "/backup"));
	m_data->http.SetHeader(HEADER_APPLICATION_JSON);
	response = m_data->http.Perform(HttpRequest::mrPOST, params.asString());
	m_data->http.ClearHeaders().SetHeader(TOKEN(m_data->token)).SetURL(m_data->url);
}

} // namespace vscale
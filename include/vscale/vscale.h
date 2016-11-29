#ifndef __VSCALE_H__
#define __VSCALE_H__

#include <json/json.h>
#include <memory>
#include <string>

namespace vscale {

using std::string;
typedef Json::Value JsonValue;

/*
* @brief Базовый класс хранящий данные для выполнения запросов к Vscale
* @detail Нельзя создавать объекты данного класса. Используется только
* в наследовании для доступа потомкам к данным для выполнения запросов
*/
class VscalePrivateData {
protected:
	VscalePrivateData() = delete;

	/*
	* @brief Конструктор, принимающий адрес для выполнения запроса и токен
	*/
	VscalePrivateData(const string &url, const string &token);

	struct PrivateData;
	std::shared_ptr<PrivateData> m_data;
};

/*
* @brief Информация о пользователе
*/
class Account : public VscalePrivateData {
public:
	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Account(const string &token);

	/*
	* @brief Получить информацию о пользователе
	* @param [out] response Информация о пользователе в объекте json
	* @code
	* 	JsonValue response;
	* 	Account("token").Info(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void Info(JsonValue &response) const;
};

/*
* @brief Управление серверами
*/
class Scalets : public VscalePrivateData {
public:
	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Scalets(const string &token);

	/// Виртуальный деструктор
	virtual ~Scalets();

	/*
	* @brief Возвращает список серверов
	* @param [out] response Список серверов
	* @code
	* 	Json::Value response;
	* 	Scalets("token").List(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(JsonValue &response) const;

	/*
	* @brief Создать сервер с переданными параметрами
	* @param [in] params Параметры создаваемого сервера
	* @param [out] response Информация о созданном сервере
	*/
	virtual void Create(const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Удалить сервер
	* @param [in] id Идентификатор сервера
	* @param [out] response Информация об удаленном сервере
	*/
	virtual void Delete(int id, JsonValue &response) const;

	/*
	* @brief Информация о сервере
	* @param [in] id Идентификатор сервера
	* @param [out] response Информация о сервере
	*/
	virtual void Info(int id, JsonValue &response) const;

	/*
	* @brief Перезапуск сервера
	* @param [in] id Идентификатор сервера
	* @param [out] response Информация о сервере
	*/
	virtual void Restart(int id, JsonValue &response) const;

	/*
	* @brief Откатить ОС или восстановить из резервной копии
	* @param [in] id Идентификатор сервера
	* @param [in] params данные для отката или восстановления из резервной копии
	* @param [out] response Информация о сервере
	*/
	virtual void Rebuild(int id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Выключение сервера
	* @param [in] id Идентификатор сервера
	* @param [out] response Информация о сервере
	*/
	virtual void Stop(int id, JsonValue &response) const;

	/*
	* @brief Включение сервера
	* @param [in] id Идентификатор сервера
	* @param [out] response Информация о сервере
	*/
	virtual void Start(int id, JsonValue &response) const;

	/*
	* @brief Апгрейд конфигурации
	* @detail Переводит сервер на другой тарифный план (только в сторону увеличения)
	* @param [in] id Идентификатор сервера
	* @param [in] params информация о новом тарифном плане
	* @param [out] response Информация о сервере
	*/
	virtual void Upgrade(int id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Просмотр информации о статусе текущих операций
	* @param [out] response Информация о статусе
	*/
	virtual void Tasks(JsonValue &response) const;

	/*
	* @brief Создание резервной копии
	* @params [in] params данные резервной копии
	* @param [out] response Информация о статусе
	*/
	virtual void Backup(int id, const JsonValue &params, JsonValue &response) const;
};

/*
* @brief Управление тегами
*/
class ServerTags : public VscalePrivateData {
public:
	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	ServerTags(const string &token);

	/// Виртуальный деструктор
	virtual ~ServerTags();

	/*
	* @brief Список тегов
	* @param [out] response Список тегов
	* @code
	* 	Json::Value response;
	* 	ServerTags("token").List(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(JsonValue &response) const;

	/*
	* @brief Создание нового тега
	* @param [in] params Параметры создаваемого тега
	* @param [out] response Информация о созданном теге
	*/
	virtual void Create(const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Обновление информации о теге
	* @param [id] id Идентификатор тега
	* @param [in] params Новые параметры тега
	* @param [out] response Информация об измененном теге
	*/
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Удаление тега
	* @param [id] id Идентификатор тега
	* @param [out] response Информация об удаленном теге
	*/
	virtual void Delete(int id, JsonValue &response) const;
};

/*
* @brief Управление резервными копиями
*/
class Backup : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Backup(const string &token);

	/// Виртуальный деструктор
	virtual ~Backup();

	/*
	* @brief Список резервных копий
	* @param [out] response Список тегов
	* @code
	* 	Json::Value response;
	* 	Backup("token").List(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(JsonValue &response) const;

	/*
	* @brief Удаление резервной копии
	* @param [out] response Информация о резервной копии
	*/
	virtual void Delete(const string &id, JsonValue &response) const;

	/*
	* @brief Информация о резервной копии
	* @param [out] response Информация о резервной копии
	*/
	virtual void Info(const string &id, JsonValue &response) const;
};

/*
* @brief Служебная информация
*/
class Background : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Background(const string &token);

	/// Виртуальный деструктор
	virtual ~Background();

	/*
	* @brief Получение списка дата-центров
	* @param [out] response Список дата-центров
	*/
	virtual void Locations(JsonValue &response) const;

	/*
	* @brief Получение списка доступных образов
	* @param [out] response Список образов
	*/
	virtual void Images(JsonValue &response) const;
};

/*
* @brief Информация о конфигурациях
*/
class Configurations : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Configurations(const string &token);

	/// Виртуальный деструктор
	virtual ~Configurations();

	/*
	* @brief Список доступных конфигураций
	* @param [out] response Список доступных конфигураций
	*/
	virtual void RPlans(JsonValue &response) const;

	/*
	* @brief Информация о стоимости использования каждой из доступных конфигураций за час и за месяц
	* @param [out] response
	*/
	virtual void BillingPrices(JsonValue &response) const;
};

/*
* @brief Управление SSH-ключами
*/
class SSHKeys : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	SSHKeys(const string &token);

	/// Виртуальный деструктор
	virtual ~SSHKeys();

	/*
	* @brief Список ssh-ключей
	* @param [out] response Список тегов
	* @code
	* 	Json::Value response;
	* 	SSHKeys("token").List(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(JsonValue &response) const;

	/*
	* @brief Добавление нового ключа
	* @param [in] params Параметры создаваемого ssh-ключа
	* @param [out] response Информация о созданном ssh-ключе
	*/
	virtual void Create(const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Удаление ключа из клиентской панели
	* @param [in] Идентификатор ключа
	* @param [out] response Информация об удаленном ssh-ключе
	*/
	virtual void Delete(int id, JsonValue &response) const;
};

/*
* @brief Управление уведомлением
*/
class Notifications : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Notifications(const string &token);

	/// Виртуальный деструктор
	virtual ~Notifications();

	/*
	* @brief Изменение настроек уведомлений об исчерпании баланса
	* @param [out] response
	*/
	virtual void Update(const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Просмотр настроек уведомлений об исчерпании баланса
	* @param [out] response
	*/
	virtual void Info(JsonValue &response) const;
};

/*
* @brief Биллинг
*/
class Billing : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Billing(const string &token);

	/// Виртуальный деструктор
	virtual ~Billing();

	/*
	* @brief Просмотр информации о текущем состоянии баланса
	*/
	virtual void Balance(JsonValue &response) const;

	/*
	* @brief Просмотр информации о пополнении счёта
	*/
	virtual void Payments(JsonValue &response) const;

	/*
	* @brief Просмотр информации о списаниях
	* @param [in] start_date Начальная дата
	* @param [in] end_date Конечная дата
	* @param [out] response
	* @code
	* 	Json::Value response;
	*	Billing("token").Consumption("2015-07-01", "2015-08-01", response);
	*	std::cout << response.toStyledString() << std::endl;
	* #endcode
	*/
	virtual void Consumption(const string &start_date, const string &end_date, JsonValue &response) const;
};

/*
* @brief Управление доменами
*/
class Domain : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	Domain(const string &token);

	/// Виртуальный деструктор
	virtual ~Domain();

	/*
	* @brief Возвращает список доменов
	* @param [out] response Список доменов
	* @code
	* 	Json::Value response;
	* 	Domain("token").List(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(JsonValue &) const;

	/*
	* @brief Создание домена
	* @param [in] params Параметры создаваемого домена
	* @param [out] response Информация о созданном домене
	*/
	virtual void Create(const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Изменение информации о домене
	* @param [in] params Параметры домена
	* @param [out] response Информация об измененном домене
	*/
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Удаление домена
	* @param [in] id Идентификатор домена
	* @param [out] response Описание изменённого домена
	*/
	virtual void Delete(int id, JsonValue &response) const;

	/*
	* @brief Информация о домене
	* @param [in] id Идентификатор домена
	* @param [out] response Описание изменённого домена
	*/
	virtual void Info(int id, JsonValue &response) const;
};

/*
* @brief Управление списоком записей домена
*/
class DomainRecord : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	DomainRecord(const string &token);

	/// Виртуальный деструктор
	virtual ~DomainRecord();

	/*
	* @brief Список записей домена
	* @param [in] domain_id Идентификатор домена
	* @param [out] response Список записей
	* @code
	* 	Json::Value response;
	* 	DomainRecord("token").List(1, response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(int domain_id, JsonValue &response) const;

	/*
	* @brief Создать ресурсную запись для домена
	* @params [in] domain_id Идентификатор домена
	* @param [in] params Параметры создаваемой ресурсной записи
	* @param [out] response Созданная ресурсная запись
	*/
	virtual void Create(int domain_id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Обновить ресурсную запись для домена
	* @params [in] domain_id Идентификатор домена
	* @params [in] record_id Идентификатор ресурсной записи
	* @param [in] params Параметры создаваемой ресурсной записи
	* @param [out] response Обновлённая ресурсная запись
	*/
	virtual void Update(int domain_id, int record_id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Удалить ресурсную запись домена
	* @params [in] domain_id Идентификатор домена
	* @params [in] record_id Идентификатор ресурсной записи
	* @param [out] response Созданная ресурсная запись
	*/
	virtual void Delete(int domain_id, int record_id, JsonValue &) const;

	/*
	* @brief Получить ресурсную запись
	* @params [in] domain_id Идентификатор домена
	* @params [in] record_id Идентификатор ресурсной записи
	* @param [out] response Выбранная ресурсная запись
	*/
	virtual void Info(int domain_id, int record_id, JsonValue &response) const;
};

/*
* @brief Управление тегами доменов
*/
class DomainsTags : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	DomainsTags(const string &token);

	/// Виртуальный деструктор
	virtual ~DomainsTags();

	/*
	* @brief Список пользовательских тегов
	* @param [out] response Список тегов
	* @code
	* 	Json::Value response;
	* 	DomainsTags("token").List(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(JsonValue &response) const;

	/*
	* @brief Создать тег
	* @param [in] params Параметры создаваемого тега
	* @param [out] response Созданная ресурсная запись
	*/
	virtual void Create(const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Обновить тег
	* @param [id] Идентификатор тега
	* @param [in] params Параметры тега
	* @param [out] response Информация о теге
	*/
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Удалить тег
	* @param [id] Идентификатор тега
	* @param [in] params Параметры тега
	*/
	virtual void Delete(int id) const;

	/*
	* @brief Информация о теге
	* @param [id] Идентификатор тега
	* @param [out] response Информация о теге
	*/
	virtual void Info(int id, JsonValue &response) const;
};

/*
* @brief Управление обратными записями
*/
class PTRRecords : public VscalePrivateData {
public:

	/*
	* @brief Конструктор, принимающий токен для выполнения запроса
	* @param [in] token Токен для выполнения запроса
	*/
	PTRRecords(const string &token);

	/// Виртуальный деструктор
	virtual ~PTRRecords();

	/*
	* @brief Список обратных записей
	* @param [out] response Информация об имеющихся обратных записях
	* @code
	* 	Json::Value response;
	* 	PTRRecords("token").List(response);
	* 	std::cout << response.toStyledString() << std::endl;
	* @endcode
	*/
	virtual void List(JsonValue &) const;

	/*
	* @brief Создать обратную запись
	* @param [in] params Параметры создаваемой обратной записи
	* @param [out] response Информация о созданной обратной записи
	*/
	virtual void Create(const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Изменить обратную запись
	* @param [in] id Идентификатор обратной записи
	* @param [in] params Параметры создаваемой обратной записи
	* @param [out] response Информация об изменённой обратной записи
	*/
	virtual void Update(int id, const JsonValue &params, JsonValue &response) const;

	/*
	* @brief Удалить обратную запись
	* @param [in] id Идентификатор обратной записи
	* @param [out] response Информация об изменённой обратной записи
	*/
	virtual void Delete(int id) const;

	/*
	* @brief Информация об обратной записи
	* @param [in] id Идентификатор обратной записи
	* @param [out] response Информация об изменённой обратной записи
	*/
	virtual void Info(int id, JsonValue &response) const;
};

} // namespace vscale

#endif // __VSCALE_H__

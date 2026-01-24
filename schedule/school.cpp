#include "school.h"
#include <QDebug>

School::School(QObject *parent) : QObject(parent)
{}
QString School::get_login() { return login;}
QString School::get_password() { return password;}
QString School::get_devkey() { return devkey;}
QString School::get_vendor() { return vendor;}
QString School::get_token() { return token;}
QString School::get_expires() { return expires;}

void School::set_login(QString newlogin) {login = newlogin;}
void School::set_password(QString newpassword) {password = newpassword;}
void School::set_devkey(QString newdevkey) {devkey = newdevkey;}
void School::set_vendor(QString newvendor) {vendor = newvendor;}
void School::set_token(QString newtoken) {token = newtoken;}
void School::set_expires(QString newexpires) {expires = newexpires;}

QString School::get_token_url()
{
    QString schoolurl = "";
    schoolurl.append(url);
    schoolurl.append("/auth?login=");
    schoolurl.append(get_login());
    schoolurl.append("&password=");
    schoolurl.append(get_password());
    schoolurl.append("&devkey=");
    schoolurl.append(get_devkey());
    schoolurl.append("&vendor=");
    schoolurl.append(get_vendor());

    return schoolurl;
}


QString School::get_shedule_url(const QString clas)
{
    QString schoolurl = "";
    QString date = TimeofApp::returnTime().toString("yyyyMMdd");
    schoolurl.append(url);
    schoolurl.append("/getschedule?student=&days=");
    schoolurl.append(date);
    schoolurl.append("&class=");
    schoolurl.append(clas);
    schoolurl.append("&rings=yes&vendor=");
    schoolurl.append(vendor);
    schoolurl.append("&devkey=");
    schoolurl.append(devkey);
    schoolurl.append("&login=");
    schoolurl.append(login);
    schoolurl.append("&password=");
    schoolurl.append(password);
    return schoolurl;
}

QString School::get_rules_url()
{
    QString schoolurl = "";
    schoolurl.append(url);
    schoolurl.append("/getrules?&vendor=");
    schoolurl.append(vendor);
    schoolurl.append("&devkey=");
    schoolurl.append(devkey);
    schoolurl.append("&login=");
    schoolurl.append(login);
    schoolurl.append("&password=");
    schoolurl.append(password);
    return schoolurl;
}

QStringList School::get_schedule_urls_for_week(const QString clas)
{
    QStringList urls;
    QDate currentDate = TimeofApp::returnTime().date();

    int daysToMonday = currentDate.dayOfWeek() - 1;
    if (daysToMonday < 0)
        daysToMonday += 7;
    QDate monday = currentDate.addDays(-daysToMonday);

    for (int i = 0; i < 7; i++)
    {
        QString date = monday.addDays(i).toString("yyyyMMdd");
        QString schoolurl = "";
        schoolurl.append(url);
        schoolurl.append("/getschedule?student=&days=");
        schoolurl.append(date);
        schoolurl.append("&class=");
        schoolurl.append(clas);
        schoolurl.append("&rings=yes&vendor=");
        schoolurl.append(vendor);
        schoolurl.append("&devkey=");
        schoolurl.append(devkey);
        schoolurl.append("&login=");
        schoolurl.append(login);
        schoolurl.append("&password=");
        schoolurl.append(password);
        urls.append(schoolurl);
    }

    return urls;
}

QStringList School::get_replaceLes_urls_for_week(const QString clas)
{
    QStringList urls;
    QDate currentDate = TimeofApp::returnTime().date();

    int daysToMonday = currentDate.dayOfWeek() - 1;
    if (daysToMonday < 0)
        daysToMonday += 7;
    QDate monday = currentDate.addDays(-daysToMonday);

    for (int i = 0; i < 7; i++)
    {
        QString date = monday.addDays(i).toString("yyyyMMdd");
        QString schoolurl = "";
        schoolurl.append(url);
        schoolurl.append("/getreplaces?days=");
        schoolurl.append(date);
        schoolurl.append("&class=");
        schoolurl.append(clas);
        schoolurl.append("&vendor=");
        schoolurl.append(vendor);
        schoolurl.append("&devkey=");
        schoolurl.append(devkey);
        schoolurl.append("&login=");
        schoolurl.append(login);
        schoolurl.append("&password=");
        schoolurl.append(password);
        urls.append(schoolurl);
    }

    return urls;
}

QString School::get_replace_url(const QString clas)
{
    QString schoolurl = "";
    QString date = TimeofApp::returnTime().toString("yyyyMMdd");
    schoolurl.append(url);
    schoolurl.append("/getreplaces?&days=");
    schoolurl.append(date);
    schoolurl.append("&class=");
    schoolurl.append(clas);
    schoolurl.append("&vendor=");
    schoolurl.append(vendor);
    schoolurl.append("&devkey=");
    schoolurl.append(devkey);
    schoolurl.append("&login=");
    schoolurl.append(login);
    schoolurl.append("&password=");
    schoolurl.append(password);
    return schoolurl;
}

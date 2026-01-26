#ifndef SCHOOL_H
#define SCHOOL_H
#include "time/timeofapp.h"

class School : public QObject
{
    Q_OBJECT
private:
    QString url;
    QString login;
    QString password;
    QString devkey;
    QString vendor;
    QString token;
    QString expires;
public:
    explicit School(QObject *parent = nullptr);
    QString get_login();
    QString get_password();
    QString get_devkey();
    QString get_vendor();
    QString get_token();
    QString get_expires();

    void set_login(QString newlogin);
    void set_password(QString newpassword);
    void set_devkey(QString newdevkey);
    void set_vendor(QString newvendor);
    void set_token(QString newtoken);
    void set_expires(QString newexpires);

    QString get_token_url();
    QString get_shedule_url(const QString clas);
    QString get_rules_url();
    QStringList get_schedule_urls_for_week(const QString clas);
    QStringList get_replaceLes_urls_for_week(const QString clas);
    QString get_replace_url(const QString clas);
signals:

};

#endif // SCHOOL_H

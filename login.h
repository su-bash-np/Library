#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include<QSqlError>
#include<QSqlQuery>
#include "signup.h"
#include "database.h"
#include "student.h";
#include "admin.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class SignUp;

class Login : public QDialog
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_Sign_Up_clicked();


    void on_login_clicked();

    void openAdmin();
    void openStudent();

private:
    Ui::Login *ui;

    SignUp *signup;

    void connectionStatus(bool Connect);// function declared to show database is connected or not;


    Student *student;
    Admin *admin;
};
#endif // LOGIN_H

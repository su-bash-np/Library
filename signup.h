#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include "login.h"

namespace Ui {
class SignUp;
}


class Login;

class SignUp : public QDialog
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();

private slots:
    void on_Log_in_clicked();

    void on_signUp_clicked();

private:
    Ui::SignUp *ui;

    Login *login;

     void connectionDB(bool Connect);// function declared to show database is connected or not;
};

#endif // SIGNUP_H

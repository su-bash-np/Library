#include "signup.h"
#include "ui_signup.h"

#include <QSqlQuery>
#include <QCryptographicHash>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>


SignUp::SignUp(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SignUp)
{
    ui->setupUi(this);


    //connect database
    bool Connect = Database::connect();

    //update the connection status
    connectionDB(Connect);

}

SignUp::~SignUp()
{
    delete ui;
}

void SignUp::on_Log_in_clicked()
{
    login= new Login();
    login ->show();
    this ->close();
}


//Function for connection status
void SignUp::connectionDB(bool Connect){

    if(Connect){
        ui->labeldb->setText("Database is Connected!!!");
        ui->labeldb->setStyleSheet("color:green;");
    }else{
        ui->labeldb->setText("Database is not Connected!!!");
        ui->labeldb->setStyleSheet("color:red;");
    }
}




void SignUp::on_signUp_clicked()
{
    QString name=ui->nameLineEdit->text();
    QString idcard=ui->idcardLineEdit->text();
    QString password=ui->passwordLineEdit->text();
    QString confirmPassword=ui->confirmpasswordLineEdit->text();
    QString role;
    if(ui->adminRadioButton->isChecked()){
        role = "admin";
    }else if(ui->studentRadioButton->isChecked()){
        role = "student";
    }else{
        QMessageBox::warning(this,"Error","Please select your role");
        return;
    }

    // Validate inputs
    if (name.isEmpty() || idcard.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Error", "All fields are required.");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Error", "Passwords do not match.");
        return;
    }

    // Hash the password
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Insert new user into the database
    QSqlQuery query;
    query.prepare("INSERT INTO user (name, idcard, password, role) VALUES (:name, :idcard, :password, :role)");
    query.bindValue(":name", name);
    query.bindValue(":idcard", idcard);
    query.bindValue(":password", hashedPassword);
    query.bindValue(":role", role);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to create account. Please try again.");
        qDebug() << "Error:" << query.lastError().text();
    } else {
        QMessageBox::information(this, "Success", "Account created successfully! Please login.");
        this->on_Log_in_clicked(); // call the login dialog
    }
}


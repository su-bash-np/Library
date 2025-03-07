#include "login.h"
#include "./ui_login.h"

#include"database.h"
#include "sessionmanager.h"

#include <QDebug>
#include <QCryptographicHash>// For password hashing
#include <QMessageBox>


Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    //connect database
    bool Connect = Database::connect();

    //update the connection status
    connectionStatus(Connect);
}

Login::~Login()
{
    delete ui;
}

void Login::on_Sign_Up_clicked()
{
    signup= new SignUp();
    signup->show();
    this ->close();

}

//Function for connection status
void Login::connectionStatus(bool Connect){

    if(Connect){
        ui->dblabel->setText("Database is Connected!!!");
        ui->dblabel->setStyleSheet("color:green;");
    }else{
        ui->dblabel->setText("Database is not Connected!!!");
        ui->dblabel->setStyleSheet("color:red;");
    }
}

void Login::openStudent()
{
    student = new Student();  // No parent to prevent auto-deletion
    student->show();
    this->close();  // Close login window safely
}

void Login::openAdmin()
{
    admin = new Admin();
    admin->show();
    this->close();  // Close login window safely
}



void Login::on_login_clicked()
{
    QString idcard = ui->idcardLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString role;

    // Get the selected role from radio buttons
    if (ui->adminRadioButton->isChecked()) {
        role = "admin";
    } else if (ui->studentRadioButton->isChecked()) {
        role = "student";
    } else {
        QMessageBox::warning(this, "Login Failed", "Please select your role.");
        return;
    }

    // Validate input
    if (idcard.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Failed", "Both fields are required.");
        return;
    }

    // Check if database is connected
    if (!Database::connect()) {
        QMessageBox::critical(this, "Database Error", "Database is not connected.");
        return;
    }

    // Hash the entered password
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Prepare the query to check user credentials
    QSqlQuery query;
    query.prepare("SELECT id, idcard, name FROM user WHERE idcard = :idcard AND password = :password AND role = :role");
    query.bindValue(":idcard", idcard);
    query.bindValue(":password", hashedPassword);
    query.bindValue(":role", role);

    if (!query.exec()) {
        QMessageBox::critical(this, "Login Error", "Failed to execute query.");
        qDebug() << "Error:" << query.lastError().text();
        return;
    }

    // Check if the student exists and get the ID
    if (query.next()) {
        QString user_id = query.value("id").toString();  // Fetch Student ID
        QString user_Name = query.value("name").toString();  // Fetch Student Name
        QString user_idCard = query.value("idcard").toString();  // Fetch Student ID-Card

        // Store Student id,idcard,Name in the global session variable
        if (role == "student") {
            SessionManager::currentStudentID = user_id;
            SessionManager::currentStudentName=user_Name;
            SessionManager::currentStudentID_Card=user_idCard;
        }
        else if (role == "admin") {
            SessionManager::currentAdminName=user_Name;
            SessionManager::currentAdminID_Card=user_idCard;
        }

        // Login successful, open the respective dashboard
        if (role == "admin") {
            openAdmin();
        } else if (role == "student") {
            openStudent();
        }
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid ID card or password.");
    }
}



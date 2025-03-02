#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "database.h"


bool Database::connect(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("mydatabase");
    db.setUserName("root");
    db.setPassword("qwerty@123");
    db.setPort(3306);


    if(!db.open()){

        qDebug()<<"Database is not connected";
        qDebug()<<db.lastError().text();
        return false;
    }
    qDebug()<<"Database is connected";
    return true;
}

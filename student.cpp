#include "student.h"
#include "ui_student.h"

#include"database.h"


#include<QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include<QDebug>

Student::Student(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Student)
{
    ui->setupUi(this);
}

Student::~Student()
{
    delete ui;
}

void Student::on_ViewpushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

    // Clear previous data
    ui->bookTable->setRowCount(0);


    //colume ko lagi
    ui->bookTable->setColumnCount(6);
    QStringList headers = {"Book ID", "Title", "Author", "ISBN", "Quantity", "Available"};
    ui->bookTable->setHorizontalHeaderLabels(headers);



    // Query to fetch books
    QSqlQuery query("SELECT BookID, Title, Author, ISBN, Quantity, Available FROM Books");

    int row = 0;
    while (query.next()) {
        ui->bookTable->insertRow(row);
        ui->bookTable->verticalHeader()->setVisible(false);
        ui->bookTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // BookId
        ui->bookTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // Title
        ui->bookTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // Authr
        ui->bookTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString())); // ISBN
        ui->bookTable->setItem(row, 4, new QTableWidgetItem(query.value(4).toString())); // Quantity
        ui->bookTable->setItem(row, 5, new QTableWidgetItem(query.value(5).toString())); // Available
        row++;
    }

    //Resize colume

    ui->bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void Student::on_SearchpushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void Student::on_GetpushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void Student::on_ReturnpushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void Student::on_ProfilepushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}







void Student::on_pushButton_6_clicked()
{

    QString searchText = ui->SearchlineEdit->text();  // Get text from search bar
    QSqlQuery query;

    // Search by BookID, ISBN, Title, or Author
    query.prepare("SELECT BookID, Title, Author, ISBN, Quantity, Available FROM Books "
                  "WHERE BookID LIKE :search OR ISBN LIKE :search OR Title LIKE :search OR Author LIKE :search");
    query.bindValue(":search", "%" + searchText + "%");  // Using wildcard for partial search

    if (!query.exec()) {
        qDebug() << "Search failed:" << query.lastError().text();
        return;
    }

    // Clear existing table content
    ui->bookTable->setRowCount(0);

    int row = 0;
    while (query.next()) {
        ui->bookTable->insertRow(row);
        ui->bookTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // BookID
        ui->bookTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // Title
        ui->bookTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // Author
        ui->bookTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString())); // ISBN
        ui->bookTable->setItem(row, 4, new QTableWidgetItem(query.value(4).toString())); // Quantity
        ui->bookTable->setItem(row, 5, new QTableWidgetItem(query.value(5).toString())); // Available
        row++;
    }
}


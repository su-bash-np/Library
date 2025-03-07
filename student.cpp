#include "student.h"
#include "ui_student.h"

#include"database.h"
#include "sessionmanager.h"
#include "login.h"


#include<QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include<QDebug>
#include<QDate>
#include<QString>

Student::Student(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Student)
{
    ui->setupUi(this);


    // Automatically open the Profile section
    ui->stackedWidget->setCurrentIndex(4);

    // Load Profile details
    on_ProfilepushButton_clicked();
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
    ui->bookTable->setColumnCount(7);
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

        // Add "GetBook" Button in the last column
        QPushButton *getBookBtn = new QPushButton("Get");
        ui->bookTable->setCellWidget(row, 6, getBookBtn); // Column 6 for buttons

        // Capture book ID and connect button to function
        QString bookId = query.value(0).toString();
        connect(getBookBtn, &QPushButton::clicked, this, [=]() {
            getBook(bookId);  // Call function to handle book retrieval
        });

        row++;
    }

    //Resize colume

    ui->bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}





void Student::getBook(QString bookID)
{
    QDate issueDate = QDate::currentDate();
    QDate dueDate = issueDate.addMonths(1);

    // Get Student ID from session (assumed session handling)
    int studentID = SessionManager::currentStudentID.toInt();
    if (studentID <= 0) {
        QMessageBox::critical(this, "Error", "Invalid Student ID. Please log in again.");
        return;
    }

    // Check if the student already has this book and has not returned it
    QSqlQuery checkExisting;
    checkExisting.prepare("SELECT TransactionID FROM Transactions WHERE BookID = :bookID AND id = :studentID AND ReturnDate IS NULL");
    checkExisting.bindValue(":bookID", bookID.toInt());
    checkExisting.bindValue(":studentID", studentID);

    if (!checkExisting.exec()) {
        qDebug() << "Check existing book failed:" << checkExisting.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to check existing book record.");
        return;
    }

    if (checkExisting.next()) {
        QMessageBox::warning(this, "Already Issued", "You have already taken this book. Return it before issuing again.");
        return;
    }

    // Check if the book is available
    QSqlQuery checkBook;
    checkBook.prepare("SELECT Available FROM Books WHERE BookID = :bookID");
    checkBook.bindValue(":bookID", bookID.toInt());

    if (!checkBook.exec()) {
        qDebug() << "Book check failed:" << checkBook.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to check book availability.");
        return;
    }

    if (!checkBook.next()) {
        QMessageBox::warning(this, "Not Found", "Book not found in the library.");
        return;
    }

    int available = checkBook.value(0).toInt();
    if (available <= 0) {
        QMessageBox::warning(this, "Not Available", "This book is currently out of stock.");
        return;
    }

    // Issue the book
    QSqlQuery issueQuery;
    issueQuery.prepare("INSERT INTO Transactions (BookID, id, IssueDate, DueDate) VALUES (:bookID, :studentID, :issueDate, :dueDate)");
    issueQuery.bindValue(":bookID", bookID.toInt());
    issueQuery.bindValue(":studentID", studentID);
    issueQuery.bindValue(":issueDate", issueDate.toString("yyyy-MM-dd"));
    issueQuery.bindValue(":dueDate", dueDate.toString("yyyy-MM-dd"));

    if (!issueQuery.exec()) {
        qDebug() << "Issue book failed:" << issueQuery.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to issue the book.");
        return;
    }

    // Update available count
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE Books SET Available = Available - 1 WHERE BookID = :bookID");
    updateQuery.bindValue(":bookID", bookID.toInt());

    if (!updateQuery.exec()) {
        qDebug() << "Update availability failed:" << updateQuery.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to update book availability.");
        return;
    }

    QMessageBox::information(this, "Success", "Book issued successfully!\nDue Date: " + dueDate.toString("yyyy-MM-dd"));


    on_ViewpushButton_clicked();

}




void Student::on_SearchBookBtn_clicked()
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

            // Add "GetBook" Button in the last column
            QPushButton *getBookBtn = new QPushButton("Get");
            ui->bookTable->setCellWidget(row, 6, getBookBtn); // Column 6 for buttons

            // Capture book ID and connect button to function
            QString bookId = query.value(0).toString();
            connect(getBookBtn, &QPushButton::clicked, this, [=]() {
                getBook(bookId);  // Call function to handle book retrieval
            });

            row++;
        }

}

void Student::returnBook(QString bookID){


    // Get Student ID from session (assumed session handling)
    int idCard = SessionManager::currentStudentID.toInt();





        // Query to get DueDate for the transaction
        QSqlQuery query;
        query.prepare("SELECT DueDate FROM Transactions "
                      "JOIN Books ON Transactions.BookID = Books.BookID "
                      "WHERE Transactions.BookID = :bookID "

                      "AND Transactions.ReturnDate IS NULL");

        query.bindValue(":bookID", bookID);
        query.bindValue(":idCard", idCard);

        if (!query.exec() || !query.next()) {
            QMessageBox::warning(this, "Return Error", "No active transaction found for this book with matching title.");
            return;
        }

        // Get the due date and calculate fine
        QDate dueDate = query.value("DueDate").toDate();
        QDate today = QDate::currentDate();
        int fineAmount = (today > dueDate) ? dueDate.daysTo(today) * 5 : 0; // ₹5 per late day

        // Prepare to update the transaction with return date and fine
        query.prepare("UPDATE Transactions SET ReturnDate = :returnDate, Fine = :fine "
                      "WHERE BookID = :bookID AND id = :idCard AND ReturnDate IS NULL");
        query.bindValue(":returnDate", today);
        query.bindValue(":fine", fineAmount);
        query.bindValue(":bookID", bookID);
        query.bindValue(":idCard", idCard);

        if (query.exec()) {
            // Update the Available books count after returning the book
            QSqlQuery updateBookQuery;
            updateBookQuery.prepare("UPDATE Books SET Available = Available + 1 WHERE BookID = :bookID");
            updateBookQuery.bindValue(":bookID", bookID);
            updateBookQuery.exec();

            QMessageBox::information(this, "Success", QString("Book returned successfully! Fine: ₹%1").arg(fineAmount));

            on_ProfilepushButton_clicked();  // Refresh issued books table
        } else {
            QMessageBox::critical(this, "Error", "Failed to update return details.");
        }




}

void Student::on_ProfilepushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);

    ui->c_name->setText(SessionManager::currentStudentName);
    ui->c_Id->setText(SessionManager::currentStudentID_Card);


    // Clear previous data
    ui->pBookTable->setRowCount(0);

    // Column setup
    ui->pBookTable->setColumnCount(5);
    QStringList headers = {"Book ID", "Title", "Author", "ISBN"};
    ui->pBookTable->setHorizontalHeaderLabels(headers);

    // Query to fetch books issued by the logged-in student
    QSqlQuery query;
    query.prepare("SELECT b.BookID, b.Title, b.Author, b.ISBN "
                  "FROM Books b "
                  "JOIN Transactions t ON b.BookID = t.BookID "
                  "WHERE t.id = :studentID AND t.ReturnDate IS NULL"); // Only books that are not returned
    query.bindValue(":studentID", SessionManager::currentStudentID.toInt());

    if (!query.exec()) {
        qDebug() << "Failed to retrieve issued books:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->pBookTable->insertRow(row);
        ui->pBookTable->verticalHeader()->setVisible(false);
        ui->pBookTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // BookId
        ui->pBookTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // Title
        ui->pBookTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // Author
        ui->pBookTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString())); // ISBN

        // Add "Return Book" button
        QPushButton *returnBookBtn = new QPushButton("Return");
        ui->pBookTable->setCellWidget(row, 4, returnBookBtn);

        // Capture book ID and connect button to function
        QString bookId = query.value(0).toString();
        connect(returnBookBtn, &QPushButton::clicked, this, [=]() {
            returnBook(bookId);
        });

        row++;
    }

    // Resize columns
    ui->pBookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }


    void Student::on_Exit_clicked()
    {
        // Clear admin session data
        SessionManager::currentStudentID = "";
        SessionManager::currentStudentName = "";
         SessionManager::currentStudentID_Card = "";

        // Show login window
         Login *loginPage= new Login();
         loginPage ->show();
         this ->close();
        // Close the current admin window
    }


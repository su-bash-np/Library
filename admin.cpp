#include "admin.h"
#include "ui_admin.h"
#include "database.h"
#include "sessionmanager.h"
#include "login.h"

#include<QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include<QDebug>
#include <QDate>

Admin::Admin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Admin)
{
    ui->setupUi(this);

    // Automatically open and refrace the view  section
    ui->stackedWidget->setCurrentIndex(0);
    on_ViewpushButton_clicked();
}

Admin::~Admin()
{
    delete ui;
}

void Admin::viewBookList(){

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


void Admin::on_AddBook_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void Admin::on_Update_Delete_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void Admin::on_ViewpushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    viewBookList();
}


void Admin::on_ReturnpushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void Admin::on_ProfilepushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->aName->setText(SessionManager::currentAdminName);
    ui->aID->setText(SessionManager::currentAdminID_Card);
}


void Admin::on_addBook_clicked()
{
    // Get values from input fields
    QString title = ui->titleInput->text();
    QString author = ui->authorInput->text();
    QString isbn = ui->isbnInput->text();
    int quantity = ui->quantityInput->text().toInt();
    int available = quantity;

    // Check if any field is empty
    if (title.isEmpty() || author.isEmpty() || isbn.isEmpty() || quantity <= 0) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields correctly!");
        return;
    }

    // Insert into database
    QSqlQuery query;
    query.prepare("INSERT INTO Books (Title, Author, ISBN, Quantity, Available) VALUES (:title, :author, :isbn, :quantity, :available)");
    query.bindValue(":title", title);
    query.bindValue(":author", author);
    query.bindValue(":isbn", isbn);
    query.bindValue(":quantity", quantity);
    query.bindValue(":available",available);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Book added successfully!");
        ui->titleInput->clear();
        ui->authorInput->clear();
        ui->isbnInput->clear();
        ui->quantityInput->clear();
    } else {
        QMessageBox::critical(this, "Database Error", "Failed to add book: " + query.lastError().text());
    }

}


void Admin::on_SearchPushButton_clicked()
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


void Admin::on_upDate_clicked()
{
    QString bookID = ui->up_bookid->text();
    QString title = ui->up_title->text();
    QString author = ui->up_author->text();
    QString isbn = ui->up_isbn->text();
    QString quantity = ui->up_quentity->text();

    if (bookID.isEmpty() || title.isEmpty() || author.isEmpty() || isbn.isEmpty() || quantity.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields!");
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE Books SET Title = :title, Author = :author, ISBN = :isbn, Quantity = :quantity WHERE BookID = :bookID");
    query.bindValue(":title", title);
    query.bindValue(":author", author);
    query.bindValue(":isbn", isbn);
    query.bindValue(":quantity", quantity);
    query.bindValue(":bookID", bookID);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Book updated successfully!");
    } else {
        QMessageBox::critical(this, "Update Error", query.lastError().text());
    }
}


void Admin::on_deLete_clicked()
{
        QString isbn = ui->del_isbn->text();
        QString title = ui->del_title->text();
        QString author = ui->del_auther->text();

        if (isbn.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please enter the ISBN of the book to delete!");
            return;
        }

        // Query to fetch the title and author based on ISBN
        QSqlQuery query;
        query.prepare("SELECT Title, Author FROM Books WHERE ISBN = :isbn");
        query.bindValue(":isbn", isbn);

        if (!query.exec()) {
            QMessageBox::critical(this, "Query Error", query.lastError().text());
            return;
        }

        if (query.next()) {
            QString dbTitle = query.value("title").toString();
            QString dbAuthor = query.value("author").toString();

            // Check if the entered title and author match the ones in the database
            if (!title.isEmpty() && title != dbTitle) {
                QMessageBox::warning(this, "Title Mismatch", "The entered title does not match the database record.");
                return;
            }

            if (!author.isEmpty() && author != dbAuthor) {
                QMessageBox::warning(this, "Author Mismatch", "The entered author does not match the database record.");
                return;
            }

            // Ask for confirmation with title and author info
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete Book",
                                          QString("Are you sure you want to delete the book:\n\nTitle: %1\nAuthor: %2")
                                              .arg(dbTitle).arg(dbAuthor),
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // Proceed to delete the book from the database
                query.prepare("DELETE FROM Books WHERE ISBN = :isbn");
                query.bindValue(":isbn", isbn);

                if (query.exec()) {
                    QMessageBox::information(this, "Success", "Book deleted successfully!");
                } else {
                    QMessageBox::critical(this, "Delete Error", query.lastError().text());
                }
            }
        } else {
            QMessageBox::warning(this, "Not Found", "No book found with the given ISBN.");
        }

}


void Admin::on_pushButton_5_clicked()
{
    QString bookID = ui->Re_bookid->text();
    QString idCard = ui->Re_id->text();

    // Check for empty fields
    if (bookID.isEmpty() || idCard.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter BookID, Title, and ID-Card.");
        return;
    }

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
    } else {
        QMessageBox::critical(this, "Error", "Failed to update return details.");
    }
}



void Admin::on_aExit_clicked()
{

    // Clear admin session data
    SessionManager::currentAdminName= "";
    SessionManager::currentAdminID_Card="";

    // Show login window
    Login *login_Page= new Login();
    login_Page ->show();
    this ->close();
    // Close the current admin window
}



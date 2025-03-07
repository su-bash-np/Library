#ifndef ADMIN_H
#define ADMIN_H

#include <QWidget>

namespace Ui {
class Admin;
}

class Admin : public QWidget
{
    Q_OBJECT

public:
    explicit Admin(QWidget *parent = nullptr);
    ~Admin();

private slots:


    void on_Update_Delete_clicked();

    void on_ViewpushButton_clicked();

    void on_ReturnpushButton_clicked();

    void on_ProfilepushButton_clicked();

    void on_addBook_clicked();

    void viewBookList();

    void on_SearchPushButton_clicked();

    void on_upDate_clicked();

    void on_deLete_clicked();

    void on_pushButton_5_clicked();


    void on_aExit_clicked();

    void on_AddBook_clicked();

private:
    Ui::Admin *ui;

};

#endif // ADMIN_H

#ifndef STUDENT_H
#define STUDENT_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class Student;
}

class Student : public QWidget
{
    Q_OBJECT

public:
    explicit Student(QWidget *parent = nullptr);
    ~Student();

private slots:
    void on_ViewpushButton_clicked();


    void on_ProfilepushButton_clicked();

    void getBook(QString bookId);

      void returnBook(QString bookId);


    void on_SearchBookBtn_clicked();

      void on_Exit_clicked();

  private:
    Ui::Student *ui;
   QPushButton *getBookBtn;
   QPushButton *returnBookBtn;
};

#endif // STUDENT_H

#ifndef STUDENT_H
#define STUDENT_H

#include <QWidget>

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

    void on_SearchpushButton_clicked();

    void on_GetpushButton_clicked();

    void on_ReturnpushButton_clicked();

    void on_ProfilepushButton_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::Student *ui;
};

#endif // STUDENT_H

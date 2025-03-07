#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QString>

class SessionManager {
public:
    static QString currentStudentID;    // Store logged-in  for student
    static QString currentStudentName;
    static QString currentStudentID_Card;

    static QString currentAdminName;  // Store logged-in for student admin
    static QString currentAdminID_Card;


};

#endif // SESSIONMANAGER_H


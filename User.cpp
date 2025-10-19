#include "User.h"

User::User() = default;
User::User(const std::string& id, const std::string& uname, const std::string& pwd) 
    : userID(id), username(uname), password(pwd) {}

void User::createBankAccount(const std::string& accID, double initialBalance) {
    bankAccount = BankAccount(accID, userID, initialBalance);
}
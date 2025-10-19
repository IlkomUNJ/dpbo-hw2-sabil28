#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

#include "DataTypes.h"

class BankAccount {
private:
    std::string accountID;
    double balance;
    std::vector<std::pair<std::string, double>> cashFlow;
    std::string ownerUserID;

public:
    BankAccount();
    BankAccount(const std::string& id, const std::string& ownerId, double initialBalance = 0.0);
    std::string getOwnerUserID() const { return ownerUserID; }
    double getBalance() const { return balance; }
    std::string getAccountID() const { return accountID; }
    const std::vector<std::pair<std::string, double>>& getCashFlow() const { return cashFlow; }

    bool deposit(double amount, const std::string& reason);
    bool withdraw(double amount, const std::string& reason);
    void listCashFlow(int days = 0, int months = 0) const;

    void serialize(std::ostream& os) const;
    static BankAccount deserialize(std::istream& is);
};

#endif // BANKACCOUNT_H
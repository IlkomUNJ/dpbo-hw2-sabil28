#ifndef BANK_H
#define BANK_H

#include "BankAccount.h"
#include "Transaction.h"
#include <map>

class Bank {
private:
    std::map<std::string, BankAccount> accounts; 
    Bank() = default; 

public:
    std::map<std::string, Transaction> allTransactions;

    static Bank& getInstance();

    void addAccount(const BankAccount& account);
    BankAccount* getAccount(const std::string& ownerUserID);
    
    bool executeTransaction(const std::string& buyerID, const std::string& sellerID, double amount, const std::string& reason);
    void addTransactionRecord(const Transaction& transaction);

    void listTransactionsLastWeek() const;
    void listAllCustomers() const;
    void listDormantAccounts(int months) const;
    void listNTopUsers(int n) const; 

    void save(const std::string& filename) const;
    void load(const std::string& filename);
};

#endif // BANK_H
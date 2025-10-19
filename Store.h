#ifndef STORE_H
#define STORE_H

#include "User.h"
#include "Bank.h"

class Store {
private:
    std::map<std::string, Buyer> buyers;
    std::map<std::string, Seller> sellers;
    Bank& bank = Bank::getInstance();

    Store() = default; 

public:
    static Store& getInstance();

    bool registerUser(const std::string& username, const std::string& password, const std::string& type, const std::string& accID, double initialBalance);
    User* login(const std::string& username, const std::string& password);

    bool purchaseItem(Buyer& buyer, const std::string& itemID, int quantity);
    void listAllOrders(const Buyer& buyer, const std::string& filter = "ALL") const;
    void updateTransactionStatus(const std::string& transID, TransactionStatus newStatus);

    void listTransactions(int kDays) const;
    void listPaidButIncomplete() const;
    void listMostFrequentItemTransactions(int m) const;
    void listMostActiveUsers(int k, bool isBuyer) const;

    void save(const std::string& filenameBuyers, const std::string& filenameSellers) const;
    void load(const std::string& filenameBuyers, const std::string& filenameSellers);
};

#endif // STORE_H
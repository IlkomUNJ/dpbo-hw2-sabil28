#ifndef USER_H
#define USER_H

#include "DataTypes.h"
#include "BankAccount.h"
#include "Item.h"
#include "Transaction.h"

class User {
protected:
    std::string userID;
    std::string username;
    std::string password; 
    BankAccount bankAccount;

public:
    User();
    User(const std::string& id, const std::string& uname, const std::string& pwd);
    virtual ~User() = default;

    std::string getUserID() const { return userID; }
    std::string getUsername() const { return username; }
    const BankAccount& getBankAccount() const { return bankAccount; }
    BankAccount& getBankAccountRef() { return bankAccount; } 

    bool checkPassword(const std::string& pwd) const { return password == pwd; }
    void createBankAccount(const std::string& accID, double initialBalance = 0.0);

    virtual void serialize(std::ostream& os) const = 0;
    virtual std::string getType() const = 0;
};

class Buyer : public User {
private:
    std::vector<std::string> orderHistory;

public:
    Buyer();
    Buyer(const std::string& id, const std::string& uname, const std::string& pwd);

    void addOrder(const std::string& transactionID);
    const std::vector<std::string>& getOrderHistory() const { return orderHistory; }
    
    double checkSpending(int kDays, const std::map<std::string, Transaction>& allTransactions) const;

    void serialize(std::ostream& os) const override;
    std::string getType() const override { return "Buyer"; }
};

class Seller : public User {
private:
    std::map<std::string, Item> items;

public:
    Seller();
    Seller(const std::string& id, const std::string& uname, const std::string& pwd);

    void registerNewItem(const Item& newItem);
    Item* getItem(const std::string& itemID);
    bool manageItemStock(const std::string& itemID, int quantityChange, bool isReplenish);
    void setItemPrice(const std::string& itemID, double newPrice);

    std::vector<std::pair<std::string, int>> discoverTopKItems(int k, const std::map<std::string, Transaction>& allTransactions) const;
    std::map<std::string, int> discoverLoyalCustomer(int month, int year, const std::map<std::string, Transaction>& allTransactions) const;

    void serialize(std::ostream& os) const override;
    std::string getType() const override { return "Seller"; }
};

#endif // USER_H
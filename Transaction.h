#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "DataTypes.h"

class Transaction {
private:
    std::string transactionID;
    std::string buyerID;
    std::string sellerID;
    std::string itemID;
    int quantity;
    double totalAmount;
    std::string transactionDate;
    TransactionStatus status;

public:
    Transaction();
    Transaction(const std::string& id, const std::string& buyerId, const std::string& sellerId, 
                const std::string& itemId, int qty, double amount, TransactionStatus initialStatus = TransactionStatus::PAID);
    int getQuantity() const { return quantity; }
    std::string getTransactionID() const { return transactionID; }
    std::string getBuyerID() const { return buyerID; }
    std::string getSellerID() const { return sellerID; }
    std::string getItemID() const { return itemID; }
    double getTotalAmount() const { return totalAmount; }
    std::string getTransactionDate() const { return transactionDate; }
    TransactionStatus getStatus() const { return status; }

    void setStatus(TransactionStatus newStatus);

    void serialize(std::ostream& os) const;
    static Transaction deserialize(std::istream& is);
};

#endif // TRANSACTION_H
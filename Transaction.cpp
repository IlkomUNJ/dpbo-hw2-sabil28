#include "Transaction.h"

Transaction::Transaction() : quantity(0), totalAmount(0.0), status(TransactionStatus::PAID) {}
Transaction::Transaction(const std::string& id, const std::string& buyerId, const std::string& sellerId, 
                         const std::string& itemId, int qty, double amount, TransactionStatus initialStatus)
    : transactionID(id), buyerID(buyerId), sellerID(sellerId), itemID(itemId), quantity(qty), 
      totalAmount(amount), transactionDate(getCurrentDateTime()), status(initialStatus) {}

void Transaction::setStatus(TransactionStatus newStatus) {
    status = newStatus;
}

void Transaction::serialize(std::ostream& os) const {
    os << transactionID << "|" << buyerID << "|" << sellerID << "|" << itemID << "|" 
       << quantity << "|" << std::fixed << std::setprecision(2) << totalAmount << "|" 
       << transactionDate << "|" << statusToString(status) << "\n";
}

Transaction Transaction::deserialize(std::istream& is) {
    Transaction trans;
    std::string line;
    if (std::getline(is, line)) {
        std::stringstream ss(line);
        std::string sQuantity, sTotalAmount, sStatus;
        
        if (std::getline(ss, trans.transactionID, '|') &&
            std::getline(ss, trans.buyerID, '|') &&
            std::getline(ss, trans.sellerID, '|') &&
            std::getline(ss, trans.itemID, '|') &&
            std::getline(ss, sQuantity, '|') &&
            std::getline(ss, sTotalAmount, '|') &&
            std::getline(ss, trans.transactionDate, '|') &&
            std::getline(ss, sStatus)) 
        {
            try {
                trans.quantity = std::stoi(sQuantity);
                trans.totalAmount = std::stod(sTotalAmount);
                trans.status = stringToStatus(sStatus);
            } catch (...) {
                return Transaction();
            }
        }
    }
    return trans;
}
#include "User.h"

Buyer::Buyer() = default;
Buyer::Buyer(const std::string& id, const std::string& uname, const std::string& pwd) 
    : User(id, uname, pwd) {}

void Buyer::addOrder(const std::string& transactionID) {
    orderHistory.push_back(transactionID);
}

double Buyer::checkSpending(int kDays, const std::map<std::string, Transaction>& allTransactions) const {
    double totalSpending = 0.0;
    time_t now = time(nullptr);
    time_t cutoff = now - (kDays * 24 * 3600);

    for (const auto& id : orderHistory) {
        auto it = allTransactions.find(id);
        if (it != allTransactions.end() && it->second.getBuyerID() == userID && 
            it->second.getStatus() == TransactionStatus::COMPLETED && 
            stringToTime(it->second.getTransactionDate()) >= cutoff) {
            totalSpending += it->second.getTotalAmount();
        }
    }
    return totalSpending;
}

void Buyer::serialize(std::ostream& os) const {
    os << "Buyer|" << userID << "|" << username << "|" << password << "\n";
    bankAccount.serialize(os);
    os << orderHistory.size() << "\n";
    for (const auto& id : orderHistory) {
        os << id << "\n";
    }
}
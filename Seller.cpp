#include "User.h"

Seller::Seller() = default;
Seller::Seller(const std::string& id, const std::string& uname, const std::string& pwd) 
    : User(id, uname, pwd) {}

void Seller::registerNewItem(const Item& newItem) {
    items[newItem.getItemID()] = newItem;
}

void Seller::setItemPrice(const std::string& itemID, double newPrice) {
    Item* item = getItem(itemID);
    if (item) {
        item->setPrice(newPrice);
    } else {
        std::cout << "Error: Item ID not found for price update.\n";
    }
}

Item* Seller::getItem(const std::string& itemID) {
    auto it = items.find(itemID);
    return (it != items.end()) ? &(it->second) : nullptr;
}

bool Seller::manageItemStock(const std::string& itemID, int quantityChange, bool isReplenish) {
    Item* item = getItem(itemID);
    if (!item) return false;
    if (isReplenish) item->replenishStock(quantityChange);
    else return item->decreaseStock(quantityChange);
    return true;
}

std::vector<std::pair<std::string, int>> Seller::discoverTopKItems(int k, const std::map<std::string, Transaction>& allTransactions) const {
    std::map<std::string, int> itemSales;
    for (const auto& pair : allTransactions) {
        const auto& trans = pair.second;
        if (trans.getSellerID() == userID && trans.getStatus() == TransactionStatus::COMPLETED) {
            itemSales[trans.getItemID()] += trans.getQuantity();
        }
    }
    std::vector<std::pair<std::string, int>> topK(itemSales.begin(), itemSales.end());
    std::sort(topK.begin(), topK.end(), [](const auto& a, const auto& b){
        return a.second > b.second;
    });
    if (topK.size() > k) topK.resize(k);
    return topK;
}

std::map<std::string, int> Seller::discoverLoyalCustomer(int targetMonth, int targetYear, const std::map<std::string, Transaction>& allTransactions) const {
    std::map<std::string, int> customerTransactionCount;
    for (const auto& pair : allTransactions) {
        const auto& trans = pair.second;
        if (trans.getSellerID() == userID && trans.getStatus() == TransactionStatus::COMPLETED) {
            time_t t = stringToTime(trans.getTransactionDate());
            std::tm* tm = std::localtime(&t);
            if (tm && (tm->tm_mon + 1 == targetMonth) && (tm->tm_year + 1900 == targetYear)) {
                customerTransactionCount[trans.getBuyerID()]++;
            }
        }
    }
    return customerTransactionCount;
}

void Seller::serialize(std::ostream& os) const {
    os << "Seller|" << userID << "|" << username << "|" << password << "\n";
    bankAccount.serialize(os);
    os << items.size() << "\n";
    for (const auto& pair : items) {
        pair.second.serialize(os);
    }
}
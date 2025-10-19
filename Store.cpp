#include "Store.h"
#include "Item.h" 
#include "Bank.h" 
#include <iostream>
#include <algorithm>
#include <cmath>

Store& Store::getInstance() {
    static Store instance;
    return instance;
}

bool Store::registerUser(const std::string& username, const std::string& password, const std::string& type, const std::string& accID, double initialBalance) {
    std::string userID = username + (type == "Buyer" ? "B" : "S") + std::to_string(std::time(nullptr)); 

    for (const auto& pair : buyers) { if (pair.second.getUsername() == username) return false; }
    for (const auto& pair : sellers) { if (pair.second.getUsername() == username) return false; }

    if (type == "Buyer") {
        Buyer newBuyer(userID, username, password);
        newBuyer.createBankAccount(accID, initialBalance);
        bank.addAccount(newBuyer.getBankAccount());
        buyers[userID] = newBuyer;
    } else if (type == "Seller") {
        Seller newSeller(userID, username, password);
        newSeller.createBankAccount(accID, initialBalance);
        bank.addAccount(newSeller.getBankAccount());
        sellers[userID] = newSeller;
    } else {
        return false;
    }
    std::cout << "User " << username << " registered with ID: " << userID << "\n";
    return true;
}

User* Store::login(const std::string& username, const std::string& password) {
    for (auto& pair : buyers) {
        if (pair.second.getUsername() == username && pair.second.checkPassword(password)) {
            return &pair.second;
        }
    }
    for (auto& pair : sellers) {
        if (pair.second.getUsername() == username && pair.second.checkPassword(password)) {
            return &pair.second;
        }
    }
    return nullptr;
}

bool Store::purchaseItem(Buyer& buyer, const std::string& itemID, int quantity) {
    Seller* seller = nullptr;
    Item* item = nullptr;
    std::string sellerID;

    for (auto& pair : sellers) {
        item = pair.second.getItem(itemID);
        if (item) {
            seller = &pair.second;
            sellerID = pair.first;
            break;
        }
    }

    if (!item || !seller) {
        std::cout << "Error: Item not found.\n";
        return false;
    }

    if (item->getStock() < quantity) {
        std::cout << "Error: Insufficient stock for " << item->getName() << " (Available: " << item->getStock() << ").\n";
        return false;
    }
    
    double totalAmount = item->getPrice() * quantity;
    
    if (bank.executeTransaction(buyer.getUserID(), sellerID, totalAmount, item->getName())) {
        item->decreaseStock(quantity);
        
        std::string transID = "T" + std::to_string(std::time(nullptr));
        Transaction newTrans(transID, buyer.getUserID(), sellerID, itemID, quantity, totalAmount, TransactionStatus::PAID);
        
        bank.addTransactionRecord(newTrans); 
        buyer.addOrder(transID);

        std::cout << "Purchase successful! Amount: " << totalAmount << ". Transaction ID: " << transID << "\n";
        return true;
    } else {
        std::cout << "Bank transaction failed (insufficient balance or account error).\n";
        return false;
    }
}

void Store::listAllOrders(const Buyer& buyer, const std::string& filter) const {
    std::cout << "--- " << buyer.getUsername() << "'s Orders (Filter: " << filter << ") ---\n";
    const auto& allTransactions = bank.allTransactions;
    for (const auto& transID : buyer.getOrderHistory()) {
        auto it = allTransactions.find(transID);
        if (it != allTransactions.end()) {
            const auto& trans = it->second;
            std::string statusStr = statusToString(trans.getStatus());
            if (filter == "ALL" || statusStr == filter) {
                std::cout << "ID: " << trans.getTransactionID() << " | Item: " << trans.getItemID()
                          << " | Amount: " << std::fixed << std::setprecision(2) << trans.getTotalAmount() 
                          << " | Date: " << trans.getTransactionDate()
                          << " | Status: " << statusStr << "\n";
            }
        }
    }
}

void Store::updateTransactionStatus(const std::string& transID, TransactionStatus newStatus) {
    auto it = bank.allTransactions.find(transID);
    if (it != bank.allTransactions.end()) {
        it->second.setStatus(newStatus);
        std::cout << "Transaction " << transID << " status updated to " << statusToString(newStatus) << "\n";
    } else {
        std::cout << "Error: Transaction ID not found.\n";
    }
}

void Store::listTransactions(int kDays) const {
    std::cout << "--- Store Transactions (Last " << kDays << " Days) ---\n";
    std::time_t now = std::time(nullptr);
    std::time_t cutoff = now - (kDays * 24 * 3600);

    for (const auto& pair : bank.allTransactions) {
        const auto& trans = pair.second;
        if (stringToTime(trans.getTransactionDate()) >= cutoff) {
            std::cout << "[" << trans.getTransactionDate() << "] ID: " << trans.getTransactionID() 
                      << " | Buyer: " << trans.getBuyerID() << " | Status: " << statusToString(trans.getStatus()) << "\n";
        }
    }
}

void Store::listPaidButIncomplete() const {
    std::cout << "--- Paid but Yet to be Completed Transactions ---\n";
    for (const auto& pair : bank.allTransactions) {
        const auto& trans = pair.second;
        if (trans.getStatus() == TransactionStatus::PAID) {
            std::cout << "ID: " << trans.getTransactionID() << " | Item: " << trans.getItemID() 
                      << " | Date: " << trans.getTransactionDate() << "\n";
        }
    }
}

void Store::listMostFrequentItemTransactions(int m) const {
    std::map<std::string, int> itemFrequency;
    for (const auto& pair : bank.allTransactions) {
        itemFrequency[pair.second.getItemID()]++;
    }

    std::vector<std::pair<std::string, int>> frequentItems(itemFrequency.begin(), itemFrequency.end());
    std::sort(frequentItems.begin(), frequentItems.end(), [](const auto& a, const auto& b){
        return a.second > b.second;
    });

    std::cout << "--- Top " << m << " Most Frequent Item Transactions ---\n";
    for (int i = 0; i < std::min((int)frequentItems.size(), m); ++i) {
        std::cout << i+1 << ". Item ID: " << frequentItems[i].first << " | Transactions: " << frequentItems[i].second << "\n";
    }
}

void Store::listMostActiveUsers(int k, bool isBuyer) const {
    std::map<std::string, int> userActivity;
    std::string today = getCurrentDateTime().substr(0, 10);

    for (const auto& pair : bank.allTransactions) {
        if (pair.second.getTransactionDate().substr(0, 10) == today) {
            if (isBuyer) {
                userActivity[pair.second.getBuyerID()]++;
            } else {
                userActivity[pair.second.getSellerID()]++;
            }
        }
    }

    std::vector<std::pair<std::string, int>> activeUsers(userActivity.begin(), userActivity.end());
    std::sort(activeUsers.begin(), activeUsers.end(), [](const auto& a, const auto& b){
        return a.second > b.second;
    });

    std::cout << "--- Top " << k << " Most Active " << (isBuyer ? "Buyers" : "Sellers") << " (Today) ---\n";
    for (int i = 0; i < std::min((int)activeUsers.size(), k); ++i) {
        std::cout << i+1 << ". User ID: " << activeUsers[i].first << " | Transactions: " << activeUsers[i].second << "\n";
    }
}

void Store::save(const std::string& filenameBuyers, const std::string& filenameSellers) const {
    std::ofstream ofsB(filenameBuyers);
    if (ofsB.is_open()) {
        ofsB << buyers.size() << "\n";
        for (const auto& pair : buyers) {
            pair.second.serialize(ofsB);
        }
        ofsB.close();
        std::cout << "Buyers data saved.\n";
    } else {
        std::cerr << "ERROR: Could not open " << filenameBuyers << " for saving Buyers data.\n";
    }

    std::ofstream ofsS(filenameSellers);
    if (ofsS.is_open()) {
        ofsS << sellers.size() << "\n";
        for (const auto& pair : sellers) {
            pair.second.serialize(ofsS);
        }
        ofsS.close();
        std::cout << "Sellers data saved.\n";
    } else {
        std::cerr << "ERROR: Could not open " << filenameSellers << " for saving Sellers data.\n";
    }

    Bank::getInstance().save(BANK_FILE);
}

void Store::load(const std::string& filenameBuyers, const std::string& filenameSellers) {
    Bank::getInstance().load(BANK_FILE); 

    std::ifstream ifsB(filenameBuyers);
    if (ifsB.is_open()) {
        buyers.clear();
        size_t count;
        if (ifsB >> count) {
            ifsB.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            for (size_t i = 0; i < count; ++i) {
                std::string type, userID, username, password;
                std::string line;
                if (std::getline(ifsB, line)) {
                    std::stringstream ss(line);
                    if (std::getline(ss, type, '|') &&
                        std::getline(ss, userID, '|') &&
                        std::getline(ss, username, '|') &&
                        std::getline(ss, password)) {
                        
                        Buyer buyer(userID, username, password);
                        buyer.getBankAccountRef() = BankAccount::deserialize(ifsB);
                        
                        size_t orderCount;
                        if (ifsB >> orderCount) {
                            ifsB.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            for (size_t j = 0; j < orderCount; ++j) {
                                std::string transID;
                                if (std::getline(ifsB, transID)) {
                                    buyer.addOrder(transID);
                                }
                            }
                        }
                        buyers[userID] = buyer;
                    }
                }
            }
        }
        ifsB.close();
        std::cout << "Buyers data loaded.\n";
    } else {
        std::cerr << "WARNING: Could not open " << filenameBuyers << " for loading Buyers data. Starting fresh.\n";
    }

    std::ifstream ifsS(filenameSellers);
    if (ifsS.is_open()) {
        sellers.clear();
        size_t count;
        if (ifsS >> count) {
            ifsS.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            for (size_t i = 0; i < count; ++i) {
                std::string type, userID, username, password;
                std::string line;
                if (std::getline(ifsS, line)) {
                    std::stringstream ss(line);
                    if (std::getline(ss, type, '|') &&
                        std::getline(ss, userID, '|') &&
                        std::getline(ss, username, '|') &&
                        std::getline(ss, password)) {
                        
                        Seller seller(userID, username, password);
                        seller.getBankAccountRef() = BankAccount::deserialize(ifsS);
                        
                        size_t itemCount;
                        if (ifsS >> itemCount) {
                            ifsS.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            for (size_t j = 0; j < itemCount; ++j) {
                                Item item = Item::deserialize(ifsS);
                                if (!item.getItemID().empty()) {
                                    seller.registerNewItem(item);
                                }
                            }
                        }
                        sellers[userID] = seller;
                    }
                }
            }
        }
        ifsS.close();
        std::cout << "Sellers data loaded.\n";
    } else {
        std::cerr << "WARNING: Could not open " << filenameSellers << " for loading Sellers data. Starting fresh.\n";
    }
}
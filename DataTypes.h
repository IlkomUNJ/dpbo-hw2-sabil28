#ifndef DATATYPES_H
#define DATATYPES_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <time.h>   
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>

const std::string BANK_FILE = "bank_data.txt";
const std::string BUYERS_FILE = "buyers.txt";
const std::string SELLERS_FILE = "sellers.txt";

enum class TransactionStatus {
    PAID,
    COMPLETED,
    CANCELLED
};

std::string statusToString(TransactionStatus status);
TransactionStatus stringToStatus(const std::string& statusStr);
std::string getCurrentDateTime();
time_t stringToTime(const std::string& timeStr); 
void clearInputBuffer();

class BankAccount;
class Item;
class Transaction;
class User;
class Buyer;
class Seller;
class Bank;
class Store;

#endif // DATATYPES_H
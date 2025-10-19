#include "Item.h"

Item::Item() : price(0.0), stock(0) {}
Item::Item(const std::string& id, const std::string& name, double price, int stock, const std::string& sellerId)
    : itemID(id), name(name), price(price), stock(stock), sellerID(sellerId) {}

void Item::setPrice(double newPrice) { 
    if (newPrice > 0) price = newPrice; 
}
void Item::replenishStock(int quantity) { 
    if (quantity > 0) stock += quantity; 
}
bool Item::decreaseStock(int quantity) { 
    if (quantity > 0 && stock >= quantity) { 
        stock -= quantity; 
        return true; 
    } 
    return false; 
}

void Item::serialize(std::ostream& os) const {
    os << itemID << "|" << name << "|" << std::fixed << std::setprecision(2) << price << "|" << stock << "|" << sellerID << "\n";
}

Item Item::deserialize(std::istream& is) {
    Item item;
    std::string line;
    if (std::getline(is, line)) {
        std::stringstream ss(line);
        std::string sPrice, sStock;
        
        if (std::getline(ss, item.itemID, '|') &&
            std::getline(ss, item.name, '|') &&
            std::getline(ss, sPrice, '|') &&
            std::getline(ss, sStock, '|') &&
            std::getline(ss, item.sellerID)) 
        {
            try {
                item.price = std::stod(sPrice);
                item.stock = std::stoi(sStock);
            } catch (...) {
                return Item();
            }
        }
    }
    return item;
}
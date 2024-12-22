/**
 * @file Q3-mm23b009.cpp
 * @author Akshit V V | MM23B009
 * @brief Enterprise market data publishing system that handles equity and bond market data
 *        with support for paid and free subscription models.
 * 
 * This system implements a publisher-subscriber pattern for financial market data,
 * supporting two types of instruments (equities and bonds) and two types of
 * subscribers (paid and free).
 */

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <stdexcept>

/**
 * @brief Container for instrument-specific market data
 * 
 * Stores the last traded price and either yield (for bonds) or
 * volume (for equities) along with other relevant market data.
 */


struct InstrumentData {
    double lastTradedPrice_{0.0};
    double bondYield_{0.0};
    uint64_t lastDayVolume_{0};
    
    InstrumentData(double price = 0.0, double yield = 0.0, uint64_t volume = 0)
        : lastTradedPrice_(price), bondYield_(yield), lastDayVolume_(volume) {}
};

/**
 * @brief Abstract base class for market data publishers
 * 
 * Defines the interface for publishing market data and managing subscriptions.
 * Publishers are responsible for maintaining instrument data and subscriber lists.
 */
class Publisher {
public:
    virtual bool update_data(uint64_t instrumentId, double lastTradedPrice, double extraValue) = 0;
    virtual bool subscribe(const std::string& subscriberId, uint64_t instrumentId) = 0;
    virtual bool get_data(const std::string& subscriberId, uint64_t instrumentId, InstrumentData& data) const = 0;
    virtual ~Publisher() = default;

protected:
    std::unordered_map<uint64_t, InstrumentData> instrumentData_;
    std::unordered_map<uint64_t, std::unordered_set<std::string>> subscribers_;
};

/**
 * @brief Equity market data publisher
 * 
 * Handles equity instruments (instrumentId < 1000) and their associated
 * market data including last traded price and daily volume.
 */
class EquityPublisher : public Publisher {
public:
    bool update_data(uint64_t instrumentId, double lastTradedPrice, double lastDayVolume) override {
        if (instrumentId >= 1000) return false;
        instrumentData_[instrumentId] = InstrumentData(lastTradedPrice, 0.0, static_cast<uint64_t>(lastDayVolume));
        return true;
    }

    bool subscribe(const std::string& subscriberId, uint64_t instrumentId) override {
        if (instrumentId >= 1000) return false;
        subscribers_[instrumentId].insert(subscriberId);
        return true;
    }

    bool get_data(const std::string& subscriberId, uint64_t instrumentId, InstrumentData& data) const override {
        if (instrumentId >= 1000) return false;
        
        auto instrumentIt = instrumentData_.find(instrumentId);
        if (instrumentIt == instrumentData_.end()) return false;
        
        auto subscriberIt = subscribers_.find(instrumentId);
        if (subscriberIt == subscribers_.end() || 
            subscriberIt->second.find(subscriberId) == subscriberIt->second.end()) {
            return false;
        }
        
        data = instrumentIt->second;
        return true;
    }
};

/**
 * @brief Bond market data publisher
 * 
 * Handles bond instruments (1000 <= instrumentId < 2000) and their associated
 * market data including last traded price and yield.
 */
class BondPublisher : public Publisher {
public:
    bool update_data(uint64_t instrumentId, double lastTradedPrice, double bondYield) override {
        if (instrumentId < 1000 || instrumentId >= 2000) return false;
        instrumentData_[instrumentId] = InstrumentData(lastTradedPrice, bondYield, 0);
        return true;
    }

    bool subscribe(const std::string& subscriberId, uint64_t instrumentId) override {
        if (instrumentId < 1000 || instrumentId >= 2000) return false;
        subscribers_[instrumentId].insert(subscriberId);
        return true;
    }

    bool get_data(const std::string& subscriberId, uint64_t instrumentId, InstrumentData& data) const override {
        if (instrumentId < 1000 || instrumentId >= 2000) return false;
        
        auto instrumentIt = instrumentData_.find(instrumentId);
        if (instrumentIt == instrumentData_.end()) return false;
        
        auto subscriberIt = subscribers_.find(instrumentId);
        if (subscriberIt == subscribers_.end() || 
            subscriberIt->second.find(subscriberId) == subscriberIt->second.end()) {
            return false;
        }
        
        data = instrumentIt->second;
        return true;
    }
};

/**
 * @brief Abstract base class for market data subscribers
 * 
 * Defines the interface for subscribing to and retrieving market data.
 */
class Subscriber {
public:
    virtual bool subscribe(std::shared_ptr<Publisher> publisher, uint64_t instrumentId) = 0;
    virtual void get_data(std::shared_ptr<Publisher> publisher, uint64_t instrumentId) = 0;
    virtual ~Subscriber() = default;
    virtual char get_type() const = 0;

protected:
    std::string subscriberId_;
    void print_result(bool success, uint64_t instrumentId, const InstrumentData& data) const {
        if (success) {
            std::cout << get_type() << "," << subscriberId_ << "," << instrumentId << ","
                     << std::fixed << std::setprecision(6)
                     << data.lastTradedPrice_ << ","
                     << (instrumentId < 1000 ? data.lastDayVolume_ : data.bondYield_)
                     << std::endl;
        } else {
            std::cout << get_type() << "," << subscriberId_ << "," << instrumentId << ",invalid_request" << std::endl;
        }
    }
};

/**
 * @brief Paid subscriber with unlimited get_data calls
 */
class PaidSubscriber : public Subscriber {
public:
    explicit PaidSubscriber(const std::string& id) {
        subscriberId_ = id;
    }

    bool subscribe(std::shared_ptr<Publisher> publisher, uint64_t instrumentId) override {
        return publisher->subscribe(subscriberId_, instrumentId);
    }

    void get_data(std::shared_ptr<Publisher> publisher, uint64_t instrumentId) override {
        InstrumentData data;
        bool success = publisher->get_data(subscriberId_, instrumentId, data);
        print_result(success, instrumentId, data);
    }

    char get_type() const override { return 'P'; }
};

/**
 * @brief Free subscriber with limited get_data calls (100 requests)
 */
class FreeSubscriber : public Subscriber {
private:
    int remainingRequests_{100};

public:
    explicit FreeSubscriber(const std::string& id) {
        subscriberId_ = id;
    }

    bool subscribe(std::shared_ptr<Publisher> publisher, uint64_t instrumentId) override {
        return publisher->subscribe(subscriberId_, instrumentId);
    }

    void get_data(std::shared_ptr<Publisher> publisher, uint64_t instrumentId) override {
        if (remainingRequests_ <= 0) {
            print_result(false, instrumentId, InstrumentData());
            return;
        }
        
        InstrumentData data;
        bool success = publisher->get_data(subscriberId_, instrumentId, data);
        if (success) remainingRequests_--;
        print_result(success, instrumentId, data);
    }

    char get_type() const override { return 'F'; }
};

int main() {
    auto equityPublisher = std::make_shared<EquityPublisher>();
    auto bondPublisher = std::make_shared<BondPublisher>();
    std::unordered_map<std::string, std::shared_ptr<Subscriber>> subscribers;

    int numLines;
    std::cin >> numLines;
    std::cin.ignore(); 

    std::string line;
    for (int i = 0; i < numLines; ++i) {
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "P") {
            uint64_t instrumentId;
            double lastTradedPrice, extraValue;
            iss >> instrumentId >> lastTradedPrice >> extraValue;

            if (instrumentId < 1000) {
                equityPublisher->update_data(instrumentId, lastTradedPrice, extraValue);
            } else {
                bondPublisher->update_data(instrumentId, lastTradedPrice, extraValue);
            }
        } else if (command == "S") {
            std::string type, subscriberId, action;
            uint64_t instrumentId;
            iss >> type >> subscriberId >> action >> instrumentId;

            auto publisher = (instrumentId < 1000) ? 
                std::static_pointer_cast<Publisher>(equityPublisher) : 
                std::static_pointer_cast<Publisher>(bondPublisher);

            bool validSubscriber = true;
            auto existingSubscriber = subscribers.find(subscriberId);
            if (existingSubscriber != subscribers.end()) {
                char existingType = existingSubscriber->second->get_type();
                if (existingType != type[0]) {
                    validSubscriber = false;  
                }
            } else if (type == "P") {
                subscribers[subscriberId] = std::make_shared<PaidSubscriber>(subscriberId);
            } else if (type == "F") {
                subscribers[subscriberId] = std::make_shared<FreeSubscriber>(subscriberId);
            }

            if (action == "get_data") {
                if (validSubscriber && subscribers.count(subscriberId)) {
                    subscribers[subscriberId]->get_data(publisher, instrumentId);
                } else {
                    std::cout << type << "," << subscriberId << "," << instrumentId 
                             << ",invalid_request" << std::endl;
                }
            } else if (action == "subscribe" && validSubscriber && subscribers.count(subscriberId)) {
                subscribers[subscriberId]->subscribe(publisher, instrumentId);
            }
        }
    }

    return 0;
}
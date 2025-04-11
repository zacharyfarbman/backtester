#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>

namespace backtester {
struct Tick {
  std::chrono::system_clock::time_point timestamp;
  double price;
  double volume;
  // Add more fields later (e.g. bid/ask, exchange ID, instrument ID)
};

enum class OrderType { MARKET, LIMIT, STOP, STOP_LIMIT };

enum class OrderSide { BUY, SELL };

enum class OrderStatus { PENDING, OPEN, FILLED, CANCELED, REJECTED };

struct Order {
  std::string order_id;
  std::string instrument;
  OrderType type;
  OrderSide side;
  double quantity;
  double price;
  std::chrono::system_clock::time_point timestamp;
  OrderStatus status;

  Order(OrderSide side_, double qty_, double price_,
        const std::string& instrument_ = "BTCUSD")
      : instrument(instrument_),
        type(OrderType::LIMIT),
        side(side_),
        quantity(qty_),
        price(price_),
        timestamp(std::chrono::system_clock::now()),
        status(OrderStatus::PENDING) {
    static std::atomic<int> id_counter{0};
    order_id = "order_" + std::to_string(id_counter++);
  }
};

struct Execution {
  std::string order_id;
  std::string execution_id;
  double price;
  double quantity;
  std::chrono::system_clock::time_point timestamp;
};

struct Position {
  std::string instrument;
  double quantity;
  double avg_entry_price;
  double realized_pnl;
  double unrealized_pnl;
};

}  // namespace backtester

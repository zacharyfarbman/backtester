#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataTypes.hpp"

namespace backtester {

class OrderManager {
 public:
  // Typedef for callbacks on order status changes
  using OrderCallback = std::function<void(const Order&)>;
  using ExecutionCallback = std::function<void(const Execution&)>;

  OrderManager();

  std::string submitOrder(const Order& order);
  std::optional<Order> getOrder(const std::string& order_id) const;
  bool updateOrderStatus(const std::string& order_id, OrderStatus status);
  bool recordExecution(const Execution& execution);

  void registerOrderCallback(OrderCallback callback);
  void registerExecutionCallback(ExecutionCallback callback);

  // JSON-RPC Serialization (placeholder implementation)
  std::string serializeOrderToJson(const Order& order) const;

  const std::unordered_map<std::string, Order>& getAllOrders() const {
    return orders_;
  }

 private:
  mutable std::mutex mutex_;

  std::unordered_map<std::string, Order> orders_;
  std::vector<Execution> executions_;

  std::vector<OrderCallback> orderCallbacks_;
  std::vector<ExecutionCallback> executionCallbacks_;

  void notifyOrderCallbacks(const Order& order);
  void notifyExecutionCallbacks(const Execution& execution);
};
}  // namespace backtester

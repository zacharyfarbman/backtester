#include "OrderManager.hpp"

#include <iostream>
#include <optional>

namespace backtester {

OrderManager::OrderManager() = default;

std::string OrderManager::submitOrder(const Order& order) {
  std::lock_guard<std::mutex> lock(mutex_);

  // Store the order
  orders_.insert_or_assign(order.order_id, order);

  // Notify callbacks
  notifyOrderCallbacks(order);

  // sent the order to the exchange via JSON-RPC or other protocol
  std::cout << "Order submitted: " << order.order_id
            << " Side: " << (order.side == OrderSide::BUY ? "BUY" : "SELL")
            << " QTY: " << order.quantity << " Price: " << order.price
            << std::endl;

  return order.order_id;
}

std::optional<Order> OrderManager::getOrder(const std::string& order_id) const {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = orders_.find(order_id);
  if (it != orders_.end()) {
    return it->second;
  }
  return std::nullopt;
}

bool OrderManager::updateOrderStatus(const std::string& order_id,
                                     OrderStatus status) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = orders_.find(order_id);
  if (it != orders_.end()) {
    it->second.status = status;
    notifyOrderCallbacks(it->second);
    return true;
  }
  return false;
}

bool OrderManager::recordExecution(const Execution& execution) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = orders_.find(execution.order_id);
  if (it != orders_.end()) {
    // Store the execution
    executions_.push_back(execution);

    // Update order status if needed
    // (In reality, more logic would be here to handle partial fills)
    it->second.status = OrderStatus::FILLED;

    // Notify callbacks
    notifyOrderCallbacks(it->second);
    notifyExecutionCallbacks(execution);

    return true;
  }
  return false;
}

void OrderManager::registerOrderCallback(OrderCallback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  orderCallbacks_.push_back(std::move(callback));
}

void OrderManager::registerExecutionCallback(ExecutionCallback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  executionCallbacks_.push_back(std::move(callback));
}

void OrderManager::notifyOrderCallbacks(const Order& order) {
  for (const auto& callback : orderCallbacks_) {
    callback(order);
  }
}

void OrderManager::notifyExecutionCallbacks(const Execution& execution) {
  for (const auto& callback : executionCallbacks_) {
    callback(execution);
  }
}

// Placeholder implementation for JSON serialization
// In a real system, this would use Boost.JSON
std::string OrderManager::serializeOrderToJson(const Order& order) const {
  // Simple manual JSON construction for demonstration
  std::string json = "{\n";
  json += "  \"order_id\": \"" + order.order_id + "\",\n";
  json += "  \"instrument\": \"" + order.instrument + "\",\n";
  json += "  \"side\": \"" +
          std::string(order.side == OrderSide::BUY ? "buy" : "sell") + "\",\n";
  json += "  \"quantity\": " + std::to_string(order.quantity) + ",\n";
  json += "  \"price\": " + std::to_string(order.price) + ",\n";
  json += "  \"type\": ";
  switch (order.type) {
    case OrderType::MARKET:
      json += "\"market\"";
      break;
    case OrderType::LIMIT:
      json += "\"limit\"";
      break;
    case OrderType::STOP:
      json += "\"stop\"";
      break;
    case OrderType::STOP_LIMIT:
      json += "\"stop_limit\"";
      break;
  }
  json += "\n}";

  return json;
}

}  // namespace backtester

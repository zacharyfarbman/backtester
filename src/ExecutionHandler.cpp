#include "ExecutionHandler.hpp"

#include <algorithm>
#include <iostream>

#include "DataTypes.hpp"

namespace backtester {
ExecutionHandler::ExecutionHandler(std::shared_ptr<OrderManager> order_manager)
    : order_manager_(std::move(order_manager)) {}

void ExecutionHandler::processTick(const Tick& tick) {
  // Iterate through all open orders and check for executions
  for (const auto& [order_id, order] : order_manager_->getAllOrders()) {
    if (order.status != OrderStatus::OPEN) {
      continue;  // Skip orders that are not open
    }

    if (shouldExecute(order, tick)) {
      // Create an execution
      Execution exec;
      exec.order_id = order.order_id;
      exec.execution_id = "exec_" + order.order_id;
      exec.timestamp = tick.timestamp;
      exec.quantity = order.quantity;
      exec.price = calculateExecutionPrice(order, tick);

      order_manager_->recordExecution(exec);

      std::cout << "Order executed: " << order.order_id
                << " at price: " << exec.price << std::endl;
    }
  }
}

void ExecutionHandler::setSlippageModel(double fixed_slippage) {
  fixed_slippage_ = fixed_slippage;
}

bool ExecutionHandler::shouldExecute(const Order& order,
                                     const Tick& tick) const {
  // Basic implementation for limit orders
  if (order.type == OrderType::LIMIT) {
    if (order.side == OrderSide::BUY) {
      return tick.price <= order.price;
    } else {
      return tick.price >= order.price;
    }
  }
  // Market orders are executed immediately
  else if (order.type == OrderType::MARKET) {
    return true;
  }
  // Other order types would have more complex logic
  return false;
}

double ExecutionHandler::calculateExecutionPrice(const Order& order,
                                                 const Tick& tick) const {
  // Apply slippage based on order side
  if (order.type == OrderType::MARKET) {
    if (order.side == OrderSide::BUY) {
      return tick.price + fixed_slippage_;
    } else {
      return tick.price - fixed_slippage_;
    }
  }
  // For limit orders, use the limit price (plus some slippage within limit)
  else if (order.type == OrderType::LIMIT) {
    if (order.side == OrderSide::BUY) {
      return std::min(order.price, tick.price + fixed_slippage_);
    } else {
      return std::max(order.price, tick.price - fixed_slippage_);
    }
  }
  // Default fallback
  return tick.price;
}

}  // namespace backtester

#pragma once

#include <memory>

#include "DataTypes.hpp"
#include "OrderManager.hpp"

namespace backtester {
class ExecutionHandler {
 public:
  ExecutionHandler(std::shared_ptr<OrderManager> order_mandager);

  // Process new tick, potentially generationg executions
  void processTick(const Tick& tick);
  void setSlippageModel(double fixed_slippage);

 private:
  std::shared_ptr<OrderManager> order_manager_;
  double fixed_slippage_ = 0.0;  // Fixed slippage in price points

  // Determin if an order should be filled at current price/time
  bool shouldExecute(const Order& order, const Tick& tick) const;

  // Calculate execution price with slippage
  double calculateExecutionPrice(const Order& order, const Tick& tick) const;
};
}  // namespace backtester

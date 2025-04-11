#pragma once

#include <memory>
#include <string>

#include "DataTypes.hpp"

namespace backtester {
class Strategy {
 public:
  virtual ~Strategy() = default;
  // Called once at strategy initialization
  virtual void initialize() = 0;
  // Process market data tick - return true if order generated
  virtual bool onTick(const Tick& tick) = 0;
  // Optional callback for executions/fills
  virtual void onExecution(const Execution& execution [[maybe_unused]]) {};
  // Return strategy name/identifier
  virtual std::string getName() const = 0;
};

// Factory function to create strategy instances
using StrategyPtr = std::unique_ptr<Strategy>;
using StrategyFactory = StrategyPtr (*)(const std::string& config);
}  // namespace backtester

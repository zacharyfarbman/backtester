#pragma once

#include <deque>
#include <memory>
#include <string>

#include "Strategy.hpp"

namespace backtester {
namespace strategies {

class MovingAverageCrossover : public Strategy {
 public:
  MovingAverageCrossover(int fast_period, int slow_period,
                         double position_size);

  void initialize() override;
  bool onTick(const Tick& tick) override;
  void onExecution(const Execution& execution) override;
  std::string getName() const override;

 private:
  int fast_period_;
  int slow_period_;
  double position_size_;

  std::deque<double> price_history_;
  double fast_ma_ = 0.0;
  double slow_ma_ = 0.0;
  bool position_open_ = false;
  OrderSide current_position_ = OrderSide::BUY;  // Default

  void updateMovingAverages(double price);
  double calculateMA(int period) const;
};

// Factory function
StrategyPtr createMovingAverageCrossover(const std::string& config);

}  // namespace strategies
}  // namespace backtester

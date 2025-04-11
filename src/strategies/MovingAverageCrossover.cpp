#include "strategies/MovingAverageCrossover.hpp"

#include <iostream>
#include <numeric>

namespace backtester {
namespace strategies {

MovingAverageCrossover::MovingAverageCrossover(int fast_period, int slow_period,
                                               double position_size)
    : fast_period_(fast_period),
      slow_period_(slow_period),
      position_size_(position_size) {
  if (fast_period_ >= slow_period_) {
    throw std::invalid_argument("Fast period must be smaller than slow period");
  }
}

void MovingAverageCrossover::initialize() {
  price_history_.clear();
  fast_ma_ = 0.0;
  slow_ma_ = 0.0;
  position_open_ = false;
  std::cout << "Initialized " << getName() << " strategy" << std::endl;
}

bool MovingAverageCrossover::onTick(const Tick& tick) {
  // Update moving averages with new price
  updateMovingAverages(tick.price);

  // Wait until we have enough data
  if (price_history_.size() < static_cast<size_t>(slow_period_)) {
    return false;
  }

  // Check for crossover signals
  bool signal_generated = false;

  // Fast MA crosses above Slow MA -> BUY signal
  if (fast_ma_ > slow_ma_ && !position_open_) {
    std::cout << "BUY Signal at price: " << tick.price
              << " (Fast MA: " << fast_ma_ << ", Slow MA: " << slow_ma_ << ")"
              << std::endl;

    // Create a BUY order
    Order order(OrderSide::BUY, position_size_, tick.price);
    // In a complete implementation, this would be passed to the framework

    position_open_ = true;
    current_position_ = OrderSide::BUY;
    signal_generated = true;
  }
  // Fast MA crosses below Slow MA -> SELL signal
  else if (fast_ma_ < slow_ma_ && !position_open_) {
    std::cout << "SELL Signal at price: " << tick.price
              << " (Fast MA: " << fast_ma_ << ", Slow MA: " << slow_ma_ << ")"
              << std::endl;

    // Create a SELL order
    Order order(OrderSide::SELL, position_size_, tick.price);
    // In a complete implementation, this would be passed to the framework

    position_open_ = true;
    current_position_ = OrderSide::SELL;
    signal_generated = true;
  }

  return signal_generated;
}

void MovingAverageCrossover::onExecution(const Execution& execution) {
  std::cout << "Execution received in strategy for order: "
            << execution.order_id << std::endl;
}

std::string MovingAverageCrossover::getName() const {
  return "MovingAverageCrossover(" + std::to_string(fast_period_) + "," +
         std::to_string(slow_period_) + ")";
}

void MovingAverageCrossover::updateMovingAverages(double price) {
  // Add new price to history
  price_history_.push_back(price);

  // Trim history if it gets too large
  if (price_history_.size() > static_cast<size_t>(slow_period_)) {
    price_history_.pop_front();
  }

  // Calculate moving averages
  fast_ma_ = calculateMA(fast_period_);
  slow_ma_ = calculateMA(slow_period_);
}

double MovingAverageCrossover::calculateMA(int period) const {
  if (price_history_.size() < static_cast<size_t>(period)) {
    return 0.0;
  }

  // For optimization in a real system, you would incrementally update MAs
  // rather than recalculating from scratch each time

  // Get the last 'period' elements
  auto start = price_history_.end() - period;
  auto end = price_history_.end();

  // Calculate sum and divide by period
  double sum = std::accumulate(start, end, 0.0);
  return sum / period;
}

// Factory function implementation
StrategyPtr createMovingAverageCrossover(const std::string& config
                                         [[maybe_unused]]) {
  // In a real system, parse config string for parameters
  // For simplicity, we're using hardcoded values here
  int fast_period = 10;
  int slow_period = 30;
  double position_size = 1.0;

  return std::make_unique<MovingAverageCrossover>(fast_period, slow_period,
                                                  position_size);
}

}  // namespace strategies
}  // namespace backtester

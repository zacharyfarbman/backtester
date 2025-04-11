#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "DataFeed.hpp"
#include "ExecutionHandler.hpp"
#include "OrderManager.hpp"
#include "Strategy.hpp"
#include "strategies/MovingAverageCrossover.hpp"

int main(int argc, char* argv[]) {
  std::cout << "--- DeFi Backtester Starting ---" << std::endl;

  // --- Configuration ---
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <data_file.csv>" << std::endl;
    return 1;
  }
  std::string dataFilePath = argv[1];
  std::cout << "Data file path: " << dataFilePath << std::endl;

  // --- Component Initialization ---
  backtester::DataFeed dataFeed(dataFilePath);
  if (!dataFeed.loadData()) {
    std::cerr << "Failed to load market data. Exiting." << std::endl;
    return 1;
  }

  // Create components
  auto orderManager = std::make_shared<backtester::OrderManager>();
  auto executionHandler =
      std::make_shared<backtester::ExecutionHandler>(orderManager);

  // Create strategy
  auto strategy = backtester::strategies::createMovingAverageCrossover("");
  strategy->initialize();

  // Configure execution handling
  executionHandler->setSlippageModel(0.01);  // Small fixed slippage

  // --- Main Event Loop ---
  std::cout << "--- Starting Simulation Loop ---" << std::endl;
  int tickCount = 0;
  while (auto tickOpt = dataFeed.getNextTick()) {
    const auto& tick = *tickOpt;
    tickCount++;

    // Process this tick for executions
    executionHandler->processTick(tick);

    // Process this tick for strategy signals
    bool signalGenerated = strategy->onTick(tick);

    if (tickCount % 1000 == 0 || signalGenerated) {
      auto tt = std::chrono::system_clock::to_time_t(tick.timestamp);
      std::tm utc_tm = *std::gmtime(&tt);
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    tick.timestamp.time_since_epoch()) %
                1000;

      std::cout << "Tick " << tickCount
                << ": Time=" << std::put_time(&utc_tm, "%Y-%m-%d %H:%M:%S")
                << '.' << std::setfill('0') << std::setw(3) << ms.count()
                << ", Price=" << tick.price << ", Volume=" << tick.volume
                << std::endl;
    }
  }

  std::cout << "--- Simulation Loop Finished ---" << std::endl;
  std::cout << "Total ticks processed: " << tickCount << std::endl;

  std::cout << "--- DeFi Backtester Shutting Down ---" << std::endl;
  return 0;
}

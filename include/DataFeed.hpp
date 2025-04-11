#pragma once

#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "DataTypes.hpp"

namespace backtester {
class DataFeed {
 public:
  // Constructor takes the path to the data file
  explicit DataFeed(const std::string& filepath);

  // Attempts to load and parse the data file
  bool loadData();

  // Gets the next tick in chronological order
  // Returns std::nullopt if no more ticks are available
  std::optional<Tick> getNextTick();

 private:
  std::string dataFilepath;
  std::vector<Tick> ticks;  // Stores all ticks after loading
  size_t currentTickIndex = 0;
};

}  // namespace backtester

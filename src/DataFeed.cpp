#include "DataFeed.hpp"

#include <charconv>
#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <system_error>

namespace backtester {

DataFeed::DataFeed(const std::string& filepath) : dataFilepath(filepath) {}

bool DataFeed::loadData() {
  std::ifstream file(dataFilepath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open data file: " << dataFilepath
              << std::endl;
    return false;
  }

  std::string line;
  int lineNumber = 0;
  ticks.clear();
  currentTickIndex = 0;

  while (std::getline(file, line)) {
    lineNumber++;
    if (line.empty() || line[0] == '#') {  // Skip empty lines or comments
      continue;
    }

    std::stringstream ss(line);
    std::string segment;
    std::vector<std::string> parts;

    while (std::getline(ss, segment, ',')) {  // Split by comma
      parts.push_back(segment);
    }

    if (parts.size() != 3) {
      std::cerr << "Warning: Skipping malformed line " << lineNumber << " in "
                << dataFilepath << " (Expected 3 parts, got " << parts.size()
                << ")" << std::endl;
      continue;
    }

    try {
      Tick tick;
      // Timestamp (milliseconds since epoch)
      long long timestamp_ms{};
      std::string_view sv_ts(parts[0]);
      auto res_ts = std::from_chars(sv_ts.data(), sv_ts.data() + sv_ts.size(),
                                    timestamp_ms);
      if (res_ts.ec != std::errc() ||
          res_ts.ptr != sv_ts.data() + sv_ts.size()) {
        throw std::runtime_error(
            "Invalid timestamp format or incomplete parse");
      }

      tick.timestamp = std::chrono::system_clock::time_point(
          std::chrono::milliseconds(timestamp_ms));

      // Price
      size_t price_chars_processed = 0;
      tick.price = std::stod(parts[1], &price_chars_processed);
      if (price_chars_processed != parts[1].length()) {
        throw std::runtime_error("Incomplete parse for price");
      }

      // Volume
      size_t vol_chars_processed = 0;
      tick.volume = std::stod(parts[2], &vol_chars_processed);
      if (vol_chars_processed != parts[2].length()) {
        throw std::runtime_error("Incomplete parse for volume");
      }

      ticks.push_back(tick);
    } catch (const std::invalid_argument& e) {
      std::cerr << "Warning: Invalid number format on line " << lineNumber
                << " in " << dataFilepath << ": " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
      std::cerr << "Warning: Number out of range on line " << lineNumber
                << " in " << dataFilepath << ": " << e.what() << std::endl;
    } catch (const std::exception& e) {  // Catch other general errors
      std::cerr << "Warning: Error parsing line " << lineNumber << " in "
                << dataFilepath << ": " << e.what() << std::endl;
    }
  }

  if (ticks.empty()) {
    std::cerr << "Warning: No valid ticks loaded from " << dataFilepath
              << std::endl;
    return false;
  }

  std::cerr << "Info: Successfully loaded " << ticks.size() << " ticks from "
            << dataFilepath << std::endl;
  // Optionally sort ticks by timestamp if file isn't guaranteed sorted
  // std::sort(ticks.begin(), ticks.end(), [](const Tick& a, const Tick& b) {
  // return a.timestamp < b.timestamp; });
  return true;
}

std::optional<Tick> DataFeed::getNextTick() {
  if (currentTickIndex < ticks.size()) {
    return ticks[currentTickIndex++];
  }
  return std::nullopt;  // No more ticks
}

}  // namespace backtester

#include <algorithm>
#include <string>
#include <thread>
#include <vector>

thread_local std::vector<std::vector<size_t>> dp;

size_t EditDistance(const std::string& s1, const std::string& s2) {
  const size_t m = s1.size();
  const size_t n = s2.size();

  // Resize the dp table if necessary
  if (dp.size() < m + 1) {
    dp.resize(m + 1);
  }
  for (size_t i = 0; i <= m; ++i) {
    if (dp[i].size() < n + 1) {
      dp[i].resize(n + 1);
    }
  }

  for (size_t i = 0; i <= m; ++i) {
    dp[i][0] = i;
  }
  for (size_t j = 0; j <= n; ++j) {
    dp[0][j] = j;
  }

  for (size_t i = 1; i <= m; ++i) {
    for (size_t j = 1; j <= n; ++j) {
      if (s1[i - 1] == s2[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1];
      } else {
        dp[i][j] = std::min(
            {dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + 1});
      }
    }
  }
  return dp[m][n];
}
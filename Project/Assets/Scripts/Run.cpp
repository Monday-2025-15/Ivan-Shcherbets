//
// Created by vanya on 12/27/25.
//

#include "Run.h"
#include <random>
uint64_t Run::seed = 0;
std::map<uint64_t, std::mt19937_64> Run::rngs;
void Run::GenerateRandomSeed()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<uint32_t> dist(100000000, 999999999);
    seed = dist(rng);
}

uint64_t GetSubSeed(uint64_t seed, Tag tag, int index)
{
    seed ^= static_cast<uint64_t>(tag);
    seed ^= static_cast<uint64_t>(static_cast<uint32_t>(index));
    seed ^= seed >> 33;
    seed *= 0xff51afd7ed558ccdULL;
    seed ^= seed >> 33;
    seed *= 0xc4ceb9fe1a85ec53ULL;
    seed ^= seed >> 33;
    return seed;
}

bool Run::GenerateDecision(int percentage, Tag tag, int index)
{
    if (percentage <= 0) return false;
    if (percentage >= 100) return true;
    uint64_t subseed = GetSubSeed(Run::GetSeed(), tag, index);
    auto it = rngs.find(subseed);
    if (it == rngs.end()) it = rngs.emplace(subseed, mt19937_64{subseed}).first;
    std::uniform_int_distribution<int> dist(1, 100);
    int result = dist(it->second);
    return result <= percentage;
}
int Run::GenerateinRange(int from, int to, Tag tag, int index)
{
    uint64_t subseed = GetSubSeed(Run::GetSeed(), tag, index);
    auto it = rngs.find(subseed);
    if (it == rngs.end()) it = rngs.emplace(subseed, mt19937_64{subseed}).first;
    std::uniform_int_distribution<int> dist(from, to);
    int result = dist(it->second);
    return result;
}

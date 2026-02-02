//
// Created by vanya on 12/27/25.
//

#ifndef ROGUELIKE_RUN_H
#define ROGUELIKE_RUN_H
#include <cstdint>
#include <Asset.h>
#include <map>
#include <random>
class Run
{
public:
    static void SetSeed(uint64_t s){seed = s;}
    static uint64_t GetSeed(){return seed;}
    static void GenerateRandomSeed();
    static bool GenerateDecision(int percentage, Tag tag, int index);
    static int GenerateinRange(int from, int to, Tag tag, int index);
private:
    static uint64_t seed;
    static map<uint64_t, mt19937_64> rngs;
};


#endif //ROGUELIKE_RUN_H
#include "Asset.h"

#include <unordered_map>
#include <filesystem>
#include <algorithm>

using namespace std;

static unordered_map<string, vector<Texture2D>> g_enemySpriteCache;

void Asset::LoadEnemySprites(const std::string& folder, std::vector<Texture2D>*& out)
{
    out = nullptr;

    auto it = g_enemySpriteCache.find(folder);
    if (it != g_enemySpriteCache.end())
    {
        out = &it->second;
        return;
    }

    vector<filesystem::path> files;
    try
    {
        for (const auto& entry : filesystem::directory_iterator(folder))
        {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension().string() == ".png")
                files.push_back(entry.path());
        }
    }
    catch (...)
    {
        out = nullptr;
        return;
    }

    sort(files.begin(), files.end());

    vector<Texture2D> loaded;
    loaded.reserve(files.size());
    for (auto& p : files)
        loaded.push_back(LoadTexture(p.string().c_str()));

    auto res = g_enemySpriteCache.emplace(folder, std::move(loaded));
    out = &res.first->second;
}

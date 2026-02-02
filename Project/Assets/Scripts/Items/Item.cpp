//
// Created by vanya on 1/26/26.
//

#include "Item.h"
#include "Player.h"
#include "Inventory.h"
#include "WeaponItem.h"

static bool loaded = false;
static std::unordered_map<int, json> ItemsRegister;
static std::unordered_map<int, Texture2D> TexRegister;

static void LoadAllItems()
{
    char* text = LoadFileText("Assets/Items.json");
    if (!text) return;

    json j = json::parse(text, nullptr, false);
    UnloadFileText(text);

    if (j.is_discarded()) return;
    if (!j.contains("items")) return;

    for (auto &it : j["items"])
    {
        if (!it.contains("id")) continue;
        int id = it["id"].get<int>();
        ItemsRegister[id] = it;

        if (TexRegister.find(id)!=TexRegister.end()) continue;

        string icon = it.value("icon","");
        if (icon.size()<=0) continue;

        Texture2D tex = LoadTexture(icon.c_str());
        TexRegister[id] = tex;
    }
}

static Texture2D* GetTextureById(int id)
{
    if (!loaded)
    {
        LoadAllItems();
        loaded = true;
    }

    auto it = TexRegister.find(id);
    if (it==TexRegister.end()) return nullptr;
    return &it->second;
}


Item::Item(int id)
{
    tag=Tag::Item;
    if (!loaded)
    {
        LoadAllItems();
        loaded = true;
    }
    auto data = GetJson(id);
    this->id=id;
    if (!data) return;
    name = data->value("name","");
    description = data->value("desc","");
    auto& colliders = Collider::GetAllColliders();
    for (auto& othercollider : colliders)
    {
        if (othercollider->GetParent()->GetTag()==Tag::Player)
        {
            player = dynamic_cast<Player*>(othercollider->GetParent());
            break;
        }
    }
    texture = GetTextureById(id);
    visible = true;
}

const json* Item::GetJson(int id)
{
    if (!loaded)
    {
        LoadAllItems();
        loaded = true;
    }
    auto it = ItemsRegister.find(id);
    if (it==ItemsRegister.end()) return nullptr;
    return &it->second;
}

void Item::OnPickUp()
{
    collider.Unregister();
    visible = false;
    OnPickUpExtra();
}


void Item::OnDrop(Vector2 position)
{
    visible=true;
    this->position = position;
    collider = Collider({position.x, position.y, 100, 100}, this);
    collider.Register();
    OnDropExtra();
}

void Item::Draw()
{
    Rectangle rectangle = {position.x, position.y, 100, 100};
    if (!texture)
    {
        DrawRectangleRec(rectangle, GRAY);
        return;
    }

    Rectangle src = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
    Rectangle dest = {
        rectangle.x + rectangle.width / 2.0f,
        rectangle.y + rectangle.height / 2.0f,
        rectangle.width,
        rectangle.height
    };
    Vector2 origin = { rectangle.width / 2.0f, rectangle.height / 2.0f };

    DrawTexturePro(*texture, src, dest, origin, 0, WHITE);
    if (forSale)
    {
        string p = to_string(price) + "p.";
        DrawText(p.c_str(), (int)position.x+20, (int)position.y - 50, 48, RED);
    }
    //collider.DebugDraw();
}

unique_ptr<Item> Item::CreateItem(int id)
{
    if (id==33) return make_unique<EyeItem>(id);
    if (id==34) return make_unique<ThirdHandItem>(id);

    if (id <=34) return make_unique<PassiveItems>(id);
    else if (id > 34 && id <=41) return make_unique<OneUseItem>(id);
    else if (id > 41 && id <= 48) return make_unique<GadgetItem>(id);
    else return make_unique<WeaponItem>(id);
}

static const char* TypeName(ItemType t)
{
    switch (t)
    {
    case ItemType::Passive: return "Passive";
    case ItemType::Weapon: return "Weapon";
    case ItemType::OneUse: return "OneUse";
    case ItemType::Gadget: return "Gadget";
    }
    return "";
}
static void DrawWrapped(const std::string& text,
                        float x, float y,
                        float maxW, float maxH,
                        int fontSize, Color col)
{
    std::string line;
    std::string word;

    float yy = y;
    const float bottom = y + maxH;

    auto FlushLine = [&]() -> bool
    {
        if (line.empty()) return true;

        if (yy + fontSize > bottom) return false; // stop drawing (out of box)
        DrawText(line.c_str(), (int)x, (int)yy, fontSize, col);
        yy += fontSize + 2;
        line.clear();
        return true;
    };

    for (int i = 0; i <= (int)text.size(); i++)
    {
        char ch = (i == (int)text.size()) ? '\n' : text[i];

        if (ch == ' ' || ch == '\n')
        {
            if (!word.empty())
            {
                std::string test = line;
                if (!test.empty()) test += " ";
                test += word;

                if (MeasureText(test.c_str(), fontSize) > (int)maxW)
                {
                    // draw current line, start new one
                    if (!FlushLine()) return;
                    line = word;
                }
                else
                {
                    if (!line.empty()) line += " ";
                    line += word;
                }
                word.clear();
            }

            if (ch == '\n')
            {
                if (!FlushLine()) return;
            }
        }
        else
        {
            word += ch;
        }
    }
}

void Item::DrawPopup()
{
    Vector2 base = position;
    float underY = position.y + 110;

    if (collider.GetType() == ColliderType::CIRCLE)
    {
        Circle c = collider.GetCircle();
        base = c.center;
        underY = c.center.y + c.radius + 10;
    }
    else
    {
        Rectangle r = collider.GetRectangle();
        base = { r.x + r.width/2.0f, r.y + r.height/2.0f };
        underY = r.y + r.height + 10;
    }

    float w = 420;
    float h = 220;

    Rectangle box = { base.x - w/2.0f, underY, w, h };

    Color bg = {255,255,255,230};
    Color lineCol = {0,0,0,255};

    DrawRectangleRec(box, bg);
    DrawRectangleLinesEx(box, 2, lineCol);

    float x = box.x + 12;
    float y = box.y + 10;

    DrawText(name.c_str(), (int)x, (int)y, 30, BLACK);
    y += 34;

    string t = string("Type: ") + TypeName(itemtype);
    DrawText(t.c_str(), (int)x, (int)y, 26, DARKGRAY);
    y += 30;
    if (itemtype == ItemType::Gadget)
    {
        const json* data = GetJson(id);
        int cd = data ? data->value("cooldown", 0) : 0;

        if (cd != 0)
        {
            std::string txt;

            if (cd < 0)
            {
                if (player && player->HasClock())
                    txt = "Cooldown: 90s";
                else
                    txt = "Cooldown: " + std::to_string(-cd) + " rooms";
            }
            else
                txt = "Cooldown: " + std::to_string(cd) + "s";

            DrawText(txt.c_str(), (int)x, (int)y, 22, DARKGRAY);
            y += 26;
        }
    }

    float descW = box.width - 24;
    float descH = (box.y + box.height) - y - 12;

    DrawWrapped(description, x, y, descW, descH, 24, BLACK);
}
void Item::Get(Inventory& inv)
{
    if (forSale)
    {
        if (!player) return;

        if (player->GetPoints() < price) return;

        player->AddPoints(-price);

        forSale = false;
        price = 0;
    }

    inv.Add(this);
}

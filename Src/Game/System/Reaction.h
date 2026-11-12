#pragma once
#include"Element.h"

enum ReactionType
{
    REACTION_NONE,      // 反応なし
    REACTION_VAPORIZE,  // 蒸発(水×炎)
    REACTION_SHOCK,     // 感電(水×雷)
    REACTION_OVERLOAD,  // 過負荷(炎×雷)
};

class ReactionResult
{
public:
    ReactionType type;       // 何の反応か(演出の出し分けにも使う)
    float        damageRate; // ダメージ倍率
    bool         consumeAttached; // 付着していた属性を消すか
    bool         startShock;      // 感電を開始するか

 
};

ReactionResult calcReaction(ElementType attached, ElementType hit);

const char* GetReactionName(ReactionType type);
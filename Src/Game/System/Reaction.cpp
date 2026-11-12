#include"Reaction.h"

ReactionResult calcReaction(ElementType attached, ElementType hit)
{
    ReactionResult result;
    result.damageRate = 1.0f;
    result.startShock = false;
    result.consumeAttached = false;
    result.type = REACTION_NONE;
    if ((attached == FIRE && hit == WATER) || (attached == WATER && hit == FIRE))
    {
        result.damageRate = 2.0f;
        result.startShock = false;
        result.consumeAttached = true;
        result.type = REACTION_VAPORIZE;
    }
    else if ((attached == WATER && hit == THUNDER) || (attached == THUNDER && hit == WATER))
    {
        result.damageRate = 1.5f;
        result.startShock = true;
        result.consumeAttached = true;
        result.type = REACTION_SHOCK;
    }
    else if ((attached == FIRE && hit == THUNDER) || (attached == THUNDER && hit == FIRE))
    {
        result.damageRate = 1.0f;
        result.startShock = false;
        result.consumeAttached = true;
        result.type = REACTION_OVERLOAD;
    }
    return result;
}


const char* GetReactionName(ReactionType type)
{
    switch (type)
    {
    case REACTION_VAPORIZE: return "蒸発";
    case REACTION_SHOCK:    return "感電";
    case REACTION_OVERLOAD: return "過負荷";
    default:                return "";
    }
}


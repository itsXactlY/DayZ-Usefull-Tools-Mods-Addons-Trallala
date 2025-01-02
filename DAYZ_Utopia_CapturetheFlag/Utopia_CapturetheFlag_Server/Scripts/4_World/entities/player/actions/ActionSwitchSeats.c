modded class ActionSwitchSeats
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (player && IsPlayerHoldingFlag(player))
        {
            player.MessageStatus("You cannot switch seats while carrying the flag.");
            return false; // Prevent the action
        }
        return super.ActionCondition(player, target, item);
    }

    bool IsPlayerHoldingFlag(PlayerBase player)
    {
        // Same implementation as above
        if (player && player.GetInventory())
        {
            array<EntityAI> items = new array<EntityAI>();
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
            foreach (EntityAI item : items)
            {
                if (item && (item.GetType() == "Utopia_CaptureFlag"))
                {
                    return true;
                }
            }
        }
        return false;
    }
}

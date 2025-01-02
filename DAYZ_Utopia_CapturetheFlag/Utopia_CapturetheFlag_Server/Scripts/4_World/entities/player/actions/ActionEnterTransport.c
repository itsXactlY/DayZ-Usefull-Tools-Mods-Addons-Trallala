modded class ActionGetInTransport
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (player && IsPlayerHoldingFlag(player))
        {
            // Send a message to the player
            player.MessageStatus("You cannot enter a vehicle while carrying the flag.");
            return false; // Prevent the action
        }
        return super.ActionCondition(player, target, item);
    }

    bool IsPlayerHoldingFlag(PlayerBase player)
    {
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

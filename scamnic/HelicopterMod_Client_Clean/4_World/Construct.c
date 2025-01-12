modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
		actions.Insert(ActionHeliEnableAutopilotSIB);
		actions.Insert(ActionHeliDisableAutopilotSIB);
		actions.Insert(ActionHeliStopEngineSIB);
		actions.Insert(ActionHeliStartEngineSIB);
		actions.Insert(ActionHelicopterSwitchSeatsSIB);
		actions.Insert(ActionFillFuelSIB);
		super.RegisterActions(actions);
    }
}
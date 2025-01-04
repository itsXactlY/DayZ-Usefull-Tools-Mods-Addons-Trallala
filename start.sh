#!/bin/bash
./start_workshopupdate.sh

# Define variables
SERVER_BINARY="./DayZServer"
CONFIG_FILE="serverDZ_debug.cfg"
PROFILES_FOLDER="./profile"
MODS="@CF;@Dabs Framework;@BaseBuildingPlus;@BodyBags;@Breachingcharge;@CarCover;@CJ187-LootChest;@Code Lock;@Movement;@MuchFramework;@MuchStuffPack;@RevGuns;@VPPAdminTools;@Trader;@Paragon Gear and Armor;@SNAFU_Weapons;@Care Packages V2"

# Server startup options
OPTIONS="-config=$CONFIG_FILE -profiles=$PROFILES_FOLDER -logs -adminlog -mod=\"$MODS\" -nosound -noPause"

# Use Konsole to open a new window and start the server with Fish shell
konsole --noclose -e fish -c "$SERVER_BINARY $OPTIONS"


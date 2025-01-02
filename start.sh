#!/bin/bash
./start_workshopupdate.sh

sleep 3

./DayZServer -config=serverDZ.cfg -profiles=./profile -logs -mod="@CF;@Dabs Framework;@DayZ-Expansion-Core;@DayZ-Expansion-Chat;@DayZ-Expansion-Hardline;@DayZ-Expansion-Licensed;@DayZ-Expansion-Market;@DayZ-Expansion-Name-Tags;@Movement;@VPPAdminTools"

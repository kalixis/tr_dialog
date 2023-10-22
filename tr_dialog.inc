/*

	About: Dialog Processor v3.2
	Author: kalixis

*/

#if !defined _samp_included
	#error "Please include a_samp or a_npc before tr_dialog"
#endif

#if defined _tr_dialog_included_
	#endinput
#endif
#define _tr_dialog_included_

/*

	Define const

*/

#if !defined PROCESSOR_DIALOG_ID
	#define PROCESSOR_DIALOG_ID 32767
#endif

#if !defined MAX_FUNCTION_NAME
	#define MAX_FUNCTION_NAME 32
#endif

#if !defined TR_DIALOG_DialogForPlayer
	#define TR_DIALOG_DialogForPlayer 0
#else
	#undef TR_DIALOG_DialogForPlayer
	#define TR_DIALOG_DialogForPlayer 1
#endif

/*

	Define functions

*/

#define DialogCreate Dialog_Create
#define Dialog_Create:%0(%1) \
	forward DC_%0(%1); \
	public DC_%0(%1)

#define Dialog_Show::%0(%1) \
	DC_%0(%1)

#define DialogResponse Dialog_Response
#define Dialog_Response:%0(%1) \
	forward DR_%0(%1); \
	public DR_%0(%1)

#define Dialog: #

#if !defined isnull
    #define isnull(%1) ((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

/*

	Vars

*/

static
	bool:dlg_player_open[MAX_PLAYERS char],
	dlg_player_function[MAX_PLAYERS][MAX_FUNCTION_NAME char];

#if TR_DIALOG_DialogForPlayer
	static
		dlg_player_dialogid[MAX_PLAYERS];
#endif

/*

	Public functions

*/

stock Dialog_Open(playerid, const function[], style, caption[], info[], button1[], button2[])
{
	dlg_player_open{playerid} = true;
	dlg_player_function[playerid] = !"DR_";
	strcat(dlg_player_function[playerid], function);

	#if TR_DIALOG_DialogForPlayer
		dlg_player_dialogid[playerid] = funcidx(dlg_player_function[playerid]);

		return ShowPlayerDialog(playerid, dlg_player_dialogid[playerid], style, caption, info, button1, button2);
	#else
		return ShowPlayerDialog(playerid, PROCESSOR_DIALOG_ID, style, caption, info, button1, button2);
	#endif
}

stock Dialog_IsOpen(playerid)
{
	return _:dlg_player_open{playerid};
}

stock Dialog_Show(playerid, const function[])
{
	new
		call_func[MAX_FUNCTION_NAME char] = !"DC_";

	strcat(call_func, function);
	return CallLocalFunction(call_func, "d", playerid);
}

stock Dialog_Close(playerid, bool:showDialog = true)
{
	dlg_player_open{playerid} = false;

	if (showDialog == true) {

		static
			dlg_gsSpace[] = !" ";

		return ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, dlg_gsSpace, dlg_gsSpace, dlg_gsSpace, "");
	}

	return 1;
}

stock Dialog_Message(playerid, caption[], info[], button1[])
{
	return Dialog_Open(playerid, Dialog:Message, DIALOG_STYLE_MSGBOX, caption, info, button1, "");
}

stock Dialog_MessageEx(playerid, const function[], caption[], info[], button1[], button2[])
{
	return Dialog_Open(playerid, function, DIALOG_STYLE_MSGBOX, caption, info, button1, button2);
}

/*

	OnDialogResponse

*/

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
#if TR_DIALOG_DialogForPlayer
	if (dialogid == dlg_player_dialogid[playerid]) {
#else
	if (dialogid == PROCESSOR_DIALOG_ID) {
#endif
		if (Dialog_IsOpen(playerid)) {
			if (isnull(inputtext)) {
				Dialog_Close(playerid, false);

				if (funcidx(dlg_player_function[playerid]) != -1)
					CallLocalFunction(dlg_player_function[playerid], !"iiis", playerid, response, listitem, !"\1");
				return 1;
			}
			else {
				for (new i; inputtext[i] != '\0'; i++) {
					if (inputtext[i] == '%')
						inputtext[i] = '#';
					else if (inputtext[i] == '{' && inputtext[i + 7] == '}')
						strdel(inputtext, i, i + 8);
				}
				Dialog_Close(playerid, false);

				if (funcidx(dlg_player_function[playerid]) != -1)
					CallLocalFunction(dlg_player_function[playerid], !"iiis", playerid, response, listitem, inputtext);
				return 1;
			}
		}
		return 0;
	}

	#if defined tr_dialog_OnDialogResponse
		return tr_dialog_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	#else
		return 0;
	#endif
}

#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif
#define OnDialogResponse tr_dialog_OnDialogResponse
#if defined tr_dialog_OnDialogResponse
	forward tr_dialog_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif
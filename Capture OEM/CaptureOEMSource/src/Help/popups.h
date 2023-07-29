// This file is a sort of go-between to allow the C++ compiler and the 
// HTML Help compiler to be on the same page, so to speak. This header
// works in conjuction with the global array defined in HelpMap.cpp.
// HelpMap creates an association between the DevStudio-generated ID's
// of GUI controls and a unique Help Context ID. It is these Help Context
// ID's that are defined here. The actual numeric values are arbitrary.
// The only requirement is that for each distinct block of popup help 
// text, there is some unique number that both the C++ compiler and the
// HTML Help compiler can use, so that they know that they are talking
// about the same thing.  The C++ compiler uses these values in HelpMap.cpp,
// and the HTML Help compiler uses them in popups.txt.

// NOTE: The Microsoft HTML Help compiler is quite possibly the worst piece of
// software ever created in the history of the universe. If you breath on it
// wrong, it will crash. One thing in particular to be aware of is that this
// header file must have only a single space character between each name being
// #defined and its value - no tabs, no multiple spaces. Sometimes it seem to
// choke on comments too. If you are bold enough to try to recompile the CHM
// file, remember these steps:
// 1) Try deleting all comments from this file.
// 2) Make sure that no files that it uses are currently opened by any other app.
// 3) If the help compiler still complains or crashes, take three deep breaths
//    and think about puppies. We've all been there. It sucks, but you'll live.


#define IDH_IMPORTCONFIGFILE 1001
#define IDH_EXPORTCONFIGFILE 1002
#define IDH_APP_EXIT 1003
#define IDH_VIEW_ADVANCEDMODE 1004
#define IDH_VIEW_ALWAYSONTOP 1005
#define IDH_VIEW_RESETPREVIEW 1006
#define IDH_VIEW_RESTOREDEFAULTS 1007
#define IDH_VIEW_HISTOGRAM 1008
#define IDH_APILOG_CLEAR 1009
#define IDH_APILOG_FILTER 1010
#define IDH_CAMERA_LOADSETTINGS 1011
#define IDH_CAMERA_SAVESETTINGS 1012
#define IDH_CAMERA_SETNAME 1013
#define IDH_CAMERA_REINITIALIZE 1014
#define IDH_HELP_CONTENTS 1015
#define IDH_HELP_ABOUT 1016
#define IDH_APICALL_LOG 1017
#define IDH_CAMERA_SELECT 1018
#define IDH_PREVIEW 1019
#define IDH_PREVIEW_PAUSE 1020
#define IDH_PREVIEW_PLAY 1021
#define IDH_PREVIEW_STOP 1022
#define IDH_EXPOSURE 1023
#define IDH_EXPOSURE_EDIT 1024
#define IDH_EXPOSURE_AUTO 1025
#define IDH_GAIN 1026
#define IDH_GAIN_EDIT 1027
#define IDH_GAIN_AUTO 1028
#define IDH_STANDARD_SIZES 1029
#define IDH_DECIMATION 1030
#define IDH_IMAGE_FILE 1031
#define IDH_IMAGE_FORMAT 1032
#define IDH_IMAGE_CAPTURE 1033
#define IDH_CLIP_NUMFRAMES 1034
#define IDH_CLIP_FILE 1035
#define IDH_CLIP_FORMAT 1036
#define IDH_CLIP_CAPTURE 1037
#define IDH_BRIGHTNESS 1038
#define IDH_BRIGHTNESS_EDIT 1039
#define IDH_BRIGHTNESS_AUTO 1040
#define IDH_LOOKUP_ENABLE 1041
#define IDH_LOOKUP_AUTOUPDATE 1042
#define IDH_LOOKUP_UPDATENOW 1043
#define IDH_LOOKUP_IMPORT 1044
#define IDH_LOOKUP_LIST 1045
#define IDH_LOOKUP_GRAPH 1046
#define IDH_ROI_APPLYTO_STREAM 1047
#define IDH_ROI_APPLYTO_AUTO 1048
#define IDH_WINDOWLAYOUT 1049
#define IDH_WINDOWLAYOUT_HEIGHT 1050
#define IDH_WINDOWLAYOUT_LEFT 1051
#define IDH_WINDOWLAYOUT_TOP 1052
#define IDH_WINDOWLAYOUT_WIDTH 1053
#define IDH_ROI_CENTER 1054
#define IDH_INCREMENTFILENAME 1055
#define IDH_IMAGE_PREVIEWAFTERCAPTURE 1056
#define IDH_IMAGE_CAPTUREFULLFRAME 1057
#define IDH_TIMELAPSE_CAPTURE 1058
#define IDH_TIMELAPSE_PARAMS 1059
#define IDH_TIMELAPSE_AUTOEXPOSE 1060
#define IDH_CLIP_CAPTUREFULLFRAME 1061
#define IDH_CLIP_SAVERAW 1062
#define IDH_CLIP_RAWFILE 1063
#define IDH_CLIP_RAWINCREMENT 1064
#define IDH_CLIP_SAVEFORMATTED 1065
#define IDH_CLIP_INCREMENT 1066
#define IDH_CLIP_VIEWAFTERCAPTURE 1067
#define IDH_PIXELFORMAT 1068
#define IDH_TEMPERATURE 1069
#define IDH_IMAGEFLIP 1070
#define IDH_FEATURE_SLIDER 1071
#define IDH_FEATURE_EDIT 1072
#define IDH_FEATURE_AUTO 1073
#define IDH_CAMERA_TREE 1074
#define IDH_DESCRIPTOR_CREATE 1075
#define IDH_DESCRIPTOR_DELETE 1076
#define IDH_DESCRIPTOR_DELETEALL 1077
#define IDH_DESCRIPTOR_UPDATEMODE 1078
#define IDH_DESCRIPTOR_UPDATECAMERA 1079
#define IDH_DESCRIPTOR_UPDATEHOST 1080
#define IDH_DESCRIPTOR_UPDATENOW 1081
#define IDH_DESCRIPTOR_FEATURE 1082
#define IDH_TRIGGER_ONOFF 1083
#define IDH_TRIGGER_MODE 1084
#define IDH_TRIGGER_TYPE 1085
#define IDH_TRIGGER_POLARITY 1086
#define IDH_TRIGGER_DELAY 1087
#define IDH_TRIGGER_PARAMETER 1088
#define IDH_TRIGGER_UPDATE 1089
#define IDH_TRIGGER_DESCRIPTION 1090
#define IDH_GPO_NUMBER 1091
#define IDH_GPO_ONOFF 1092
#define IDH_GPO_MODE 1093
#define IDH_GPO_POLARITY 1094
#define IDH_GPO_PARAMETER 1095
#define IDH_GPO_UPDATE 1096
#define IDH_GPO_DESCRIPTION 1097
#define IDH_CALLBACK 1098
#define IDH_CALLBACK_FILE 1099
#define IDH_KNEE_CHECK 1100
#define IDH_KNEE 1101
#define IDH_TODO 1102













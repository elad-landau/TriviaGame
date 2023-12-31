#pragma once


//from client to server
#define SIGN_IN 200
#define SIGN_OUT 201
#define SIGN_UP 203
#define GET_EXISTING_ROOMS 205
#define GET_ROOMS_USERS 207
#define JOIN_REQUEST 209
#define LEAVE_ROOM 211
#define CREATE_ROOM 213
#define CLOSE_ROOM 215
#define START_GAME 217
#define SEND_ANSWER 219
#define LEAVE_GAME 222
#define BEST_SCORES 223
#define PERSONAL_DATA 225
#define EXIT_APP 299


//from server to client
#define SIGN_IN_RES 102
#define SIGN_UP_RES 104
#define SEND_EXISTING_ROOMS 106
#define SEND_ROOMS_USERS 108
#define JOIN_REQUEST_RES 110
#define LEAVE_ROOM_RES 1120
#define CREATE_ROOM_RES 114
#define CLOSE_ROOM_RES 116
#define SEND_QUESTIONS 118
#define IS_RIGHT_ANSWER 120
#define END_GAME 121
#define BEST_SCORES_RES 124
#define PERSONAL_DATA_RES 126
#define WRONG_DETAILS 1021
#define	ALREADY_CONNECTED 1022
#define PASSWORD_ILLEGAL 1041
#define	USERNAME_EXISTS 1042
#define	USERNAME_ILLEGAL 1043
#define SINGUP_ERRUR 1044
#define ROOM_NOT_EXIST 1102
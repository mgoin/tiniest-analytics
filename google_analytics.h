/*
Tiniest Analytics - v1.1 - MIT License (i.e. can use it for whatever purpose)

Version history:
v1.1 - 2017/12/15 - changed to C-style C++

Original authors:
Mihai Dragomir, email:dmc@pixelshard.com
Mihai Gosa, email:pintea@inthekillhouse.com  twitter: @gosamihai

*/

#pragma once

// 'uniqueClientId' should be unique for each user, e.g. the Steam UID or a randomly generated 32/64bit value
// e.g. Init("UA-12345678-1", steamClientId)
bool google_analytics_init(const char* tracking_id, const char* unique_client_id);

// call when exiting app
void google_analytics_shutdown();

// call at most once per frame
void google_analytics_update();

// read this https://support.google.com/analytics/answer/1033068?hl=en
// e.g. Event("GameStart", "linux")
// e.g. Event("WeaponClicked", "RocketLauncher")
// e.g. Event("MapStarted", "SinglePlayer", "MapName.map")
void google_analytics_event(char const* category, char const* action);
void google_analytics_event(char const* category, char const* action, char const* label);
void google_analytics_event(char const* category, char const* action, char const* label, unsigned int const value);

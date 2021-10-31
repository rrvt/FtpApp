// Utilities --


#pragma once


String remotePath(TCchar* path);      // Replace '\' with '/'
String localPath(TCchar* path);       // Replace '/' with '\'
String parseName(TCchar* path);       // Finds the name after the last '\'


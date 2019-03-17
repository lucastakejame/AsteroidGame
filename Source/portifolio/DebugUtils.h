#pragma once

#include "DrawDebugHelpers.h"

#define lin() __LINE__
#define str(t) #t
#define logFloat(flo) UE_LOG(LogTemp, Warning, TEXT("%s [%d] : %s -> %f"), TEXT(__FUNCTION__), __LINE__, TEXT(str(flo)), flo)
#define logVec(vec) UE_LOG(LogTemp, Warning, TEXT("%s [%d] : %s -> (%f,%f,%f)"), TEXT(__FUNCTION__), __LINE__, TEXT(str(vec)),vec.X, vec.Y, vec.Z)
#define logFStr(text) UE_LOG(LogTemp, Warning, TEXT("%s [%d] : %s"), TEXT(__FUNCTION__), __LINE__, *text)
#define log(text) UE_LOG(LogTemp, Warning, TEXT("%s [%d] : %s"), TEXT(__FUNCTION__), __LINE__, TEXT(text))
#define lg(text, ...) UE_LOG(LogTemp, Warning, TEXT(text), ##__VA_ARGS__)

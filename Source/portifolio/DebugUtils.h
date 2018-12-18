#pragma once

#define str(t) #t
#define logFloat(flo) UE_LOG(LogTemp, Warning, TEXT(" %s: %f"), TEXT(str(flo)), flo)
#define logVec(vec) UE_LOG(LogTemp, Warning, TEXT(" %s: (%f,%f,%f)"), TEXT(str(vec)),vec.X, vec.Y, vec.Z)
#define logFStr(text) UE_LOG(LogTemp, Warning, TEXT("%s"), *text)
#define log(text) UE_LOG(LogTemp, Warning, TEXT(text))
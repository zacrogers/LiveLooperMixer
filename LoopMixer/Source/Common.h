/*
  ==============================================================================

    Common.h
    Created: 26 Apr 2023 1:35:04pm
    Author:  Zachary Rogers

  ==============================================================================
*/

#pragma once

enum class State
{
    Playing, //0
    Recording, //1
    Stopped, // 2
    PreparingToPlay, //3
    PreparingToStop, // 4
    PreparingToRecord // 5
};

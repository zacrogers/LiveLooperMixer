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
    Playing,
    Recording,
    Stopped,
    PreparingToPlay,
    PreparingToStop,
    PreparingToRecord
};

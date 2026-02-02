//
// Created by vanya on 2/2/26.
//

#ifndef ROGUELIKE_PHYSICTEACHER_H
#define ROGUELIKE_PHYSICTEACHER_H

#include "Security.h"

class PhysicTeacher : public Security
{
public:
    PhysicTeacher(float x, float y, int id);
    ~PhysicTeacher() override = default;
};

#endif

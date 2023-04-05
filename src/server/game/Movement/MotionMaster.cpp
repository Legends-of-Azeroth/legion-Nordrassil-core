/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MotionMaster.h"
#include "CreatureAISelector.h"
#include "Creature.h"

#include "ConfusedMovementGenerator.h"
#include "FleeingMovementGenerator.h"
#include "HomeMovementGenerator.h"
#include "IdleMovementGenerator.h"
#include "PointMovementGenerator.h"
#include "TargetedMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "RandomMovementGenerator.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include <cassert>

inline bool isStatic(MovementGenerator *mv)
{
    return (mv == &si_idleMovement);
}

void MotionMaster::Initialize()
{
    // clear ALL movement generators (including default)
    while (!empty())
    {
        MovementGenerator *curr = top();
        pop();
        if (curr)
            DirectDelete(curr);
    }

    InitDefault();
}

// set new default movement generator
void MotionMaster::InitDefault()
{
    if (_owner->IsCreature())
    {
        MovementGenerator* movement = FactorySelector::selectMovementGenerator(_owner->ToCreature());
        Mutate(movement == nullptr ? &si_idleMovement : movement, MOTION_SLOT_IDLE);
    }
    else
    {
        Mutate(&si_idleMovement, MOTION_SLOT_IDLE);
    }
}

MotionMaster::~MotionMaster()
{
    // clear ALL movement generators (including default)
    while (!empty())
    {
        MovementGenerator *curr = top();
        pop();
        if (curr) DirectDelete(curr);
    }
}

void MotionMaster::UpdateMotion(uint32 diff)
{
    if (!_owner)
        return;

    if (_owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED)) // what about UNIT_STATE_DISTRACTED? Why is this not included?
    {
        if (_owner->HasUnitState(UNIT_STATE_EVADE))
            if (GetMotionSlotType(MOTION_SLOT_ACTIVE) == HOME_MOTION_TYPE)
                if (!top()->Update(*_owner, diff))
                    MovementExpired();
        return;
    }

    if (empty())
        return;

    _cleanFlag |= MMCF_UPDATE;
    if (!top()->Update(*_owner, diff))
    {
        _cleanFlag &= ~MMCF_UPDATE;
        MovementExpired();
    }
    else
        _cleanFlag &= ~MMCF_UPDATE;

    if (_expList)
    {
        for (size_t i = 0; i < _expList->size(); ++i)
        {
            MovementGenerator* mg = (*_expList)[i];
            DirectDelete(mg);
        }

        delete _expList;
        _expList = nullptr;

        if (empty())
            Initialize();
        else if (needInitTop())
            InitTop();
        else if (_cleanFlag & MMCF_RESET)
            top()->Reset(*_owner);

        _cleanFlag &= ~MMCF_RESET;
    }
}

void MotionMaster::DirectClean(bool reset)
{
    while (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        if (curr) DirectDelete(curr);
    }

    if (empty())
        return;

    if (needInitTop())
        InitTop();
    else if (reset)
        top()->Reset(*_owner);
}

void MotionMaster::DelayedClean()
{
    while (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        if (curr)
            DelayedDelete(curr);
    }
}

void MotionMaster::DirectExpire(bool reset)
{
    if (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();

        if (curr)
            DirectDelete(curr);
    }

    while (!top())
        --_top;

    if (empty())
        Initialize();
    else if (needInitTop())
        InitTop();
    else if (reset)
        top()->Reset(*_owner);
}

void MotionMaster::DelayedExpire()
{
    if (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        DelayedDelete(curr);
    }

    while (!top())
        --_top;
}

void MotionMaster::MoveIdle()
{
    //! Should be preceded by MovementExpired or Clear if there's an overlying movementgenerator active
    if (empty() || !isStatic(top()))
        Mutate(&si_idleMovement, MOTION_SLOT_IDLE);
}

void MotionMaster::MoveRandom(float spawndist)
{
    if (_owner->IsCreature())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (GUID: %u) start moving random", _owner->GetGUIDLow());
        Mutate(new RandomMovementGenerator<Creature>(spawndist), MOTION_SLOT_IDLE);
    }
}

void MotionMaster::MoveTargetedHome()
{
    Clear(false);

    if (_owner->GetTypeId() == TYPEID_UNIT && !static_cast<Creature*>(_owner)->GetCharmerOrOwnerGUID())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) targeted home", _owner->GetEntry(), _owner->GetGUIDLow());
        Mutate(new HomeMovementGenerator<Creature>(), MOTION_SLOT_ACTIVE);
    }
    else if (_owner->GetTypeId() == TYPEID_UNIT && static_cast<Creature*>(_owner)->GetCharmerOrOwnerGUID())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Pet or controlled creature (Entry: %u GUID: %u) targeting home", _owner->GetEntry(), _owner->GetGUIDLow());
        Unit *target = static_cast<Creature*>(_owner)->GetCharmerOrOwner();
        if (target)
        {
            TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Following %s (GUID: %u)", target->IsPlayer() ? "player" : "creature", target->IsPlayer() ? target->GetGUIDLow() : static_cast<Creature*>(target)->GetDBTableGUIDLow());
            if (!_owner->m_movedPlayer)
            {
                TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Following %s (GUID: %u)", target->IsPlayer() ? "player" : "creature", target->IsPlayer() ? target->GetGUIDLow() : static_cast<Creature*>(target)->GetDBTableGUIDLow());
                Mutate(new FollowMovementGenerator<Creature>(*target, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE), MOTION_SLOT_ACTIVE);
            }
        }
    }
    else
    {
        TC_LOG_ERROR(LOG_FILTER_GENERAL, "Player (GUID: %u) attempt targeted home", _owner->GetGUIDLow());
    }
}

void MotionMaster::MoveConfused()
{
    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Player (GUID: %u) move confused", _owner->GetGUIDLow());
        Mutate(new ConfusedMovementGenerator<Player>(), MOTION_SLOT_CONTROLLED);
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) move confused", _owner->GetEntry(), _owner->GetGUIDLow());
        Mutate(new ConfusedMovementGenerator<Creature>(), MOTION_SLOT_CONTROLLED);
    }
}

void MotionMaster::MoveChase(Unit* target, float dist, float angle)
{
    // ignore movement request if target not exist
    if (!target || target == _owner || _owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL))
        return;

    //_owner->ClearUnitState(UNIT_STATE_FOLLOW);
    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Player (GUID: %u) chase to %s (GUID: %u)", _owner->GetGUIDLow(), target->IsPlayer() ? "player" : "creature", target->IsPlayer() ? target->GetGUIDLow() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new ChaseMovementGenerator<Player>(*target, dist, angle), MOTION_SLOT_ACTIVE);
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) chase to %s (GUID: %u)", _owner->GetEntry(), _owner->GetGUIDLow(), target->IsPlayer() ? "player" : "creature", target->IsPlayer() ? target->GetGUIDLow() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new ChaseMovementGenerator<Creature>(*target, dist, angle), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MoveFetch(Unit* target, float dist, float angle, MovementSlot slot)
{
    // ignore movement request if target not exist
    if (!target || target == _owner || _owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL))
        return;

    //_owner->AddUnitState(UNIT_STATE_FOLLOW);
    if (!_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) follow to %s (GUID: %u)", _owner->GetEntry(), _owner->GetGUIDLow(), target->IsPlayer() ? "player" : "creature", target->IsPlayer() ? target->GetGUIDLow() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FetchMovementGenerator<Creature>(*target, dist, angle), slot);
    }
}

void MotionMaster::MoveFollow(Unit* target, float dist, float angle, MovementSlot slot)
{
    // ignore movement request if target not exist
    if (!target || target == _owner || _owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL))
        return;

    //_owner->AddUnitState(UNIT_STATE_FOLLOW);
    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Player (GUID: %u) follow to %s (GUID: %u)", _owner->GetGUIDLow(), target->IsPlayer() ? "player" : "creature", target->IsPlayer() ? target->GetGUIDLow() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FollowMovementGenerator<Player>(*target, dist, angle), slot);
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) follow to %s (GUID: %u)", _owner->GetEntry(), _owner->GetGUIDLow(), target->IsPlayer() ? "player" : "creature", target->IsPlayer() ? target->GetGUIDLow() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FollowMovementGenerator<Creature>(*target, dist, angle), slot);
    }
}

void MotionMaster::MovePoint(uint32 id, float x, float y, float z, bool generatePath, float speed)
{
    if (!_owner)
        return;

    if (_owner->IsFlying())
        generatePath = false;

    if (Transport* transport = _owner->GetTransport())
        transport->CalculatePassengerPosition(x, y, z);

    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Player (GUID: %u) targeted point (Id: %u X: %f Y: %f Z: %f)", _owner->GetGUIDLow(), id, x, y, z);
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath, speed), MOTION_SLOT_ACTIVE);
    }
    else
    {
        //TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) targeted point (ID: %u X: %f Y: %f Z: %f)",
        //    _owner->GetEntry(), _owner->GetGUIDLow(), id, x, y, z);
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath, speed), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MovePointWithRot(uint32 id, float x, float y, float z, float orientation, bool generatePath)
{
    if (!_owner)
        return;

    if (_owner->IsFlying())
        generatePath = false;

    if (_owner->IsPlayer())
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath, 0.0f, orientation), MOTION_SLOT_ACTIVE);
    else
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath, 0.0f, orientation), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveLand(uint32 id, Position const& pos)
{
    float x, y, z;
    pos.GetPosition(x, y, z);

    TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u) landing point (ID: %u X: %f Y: %f Z: %f)", _owner->GetEntry(), id, x, y, z);

    Movement::MoveSplineInit init(*_owner);
    init.MoveTo(x, y, z);
    init.SetAnimation(Movement::ToGround);
    init.Launch();
    Mutate(new EffectMovementGenerator(id, x, y, z), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveTakeoff(uint32 id, float x, float y, float z)
{
    TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u) landing point (ID: %u X: %f Y: %f Z: %f)", _owner->GetEntry(), id, x, y, z);

    Movement::MoveSplineInit init(*_owner);
    init.MoveTo(x, y, z);
    init.SetAnimation(Movement::ToFly);
    init.Launch();
    Mutate(new EffectMovementGenerator(id, x, y, z), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveKnockbackFrom(float srcX, float srcY, float speedXY, float speedZ, Movement::SpellEffectExtraData const* spellEffectExtraData /*= nullptr*/)
{
    //this function may make players fall below map
    if (_owner->IsPlayer())
        return;

    float x, y, z;
    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    for (uint8 i = 0; i < 5; ++i)
    {
        switch (i)
        {
            case 0:
                _owner->GetNearPoint(_owner, x, y, z, _owner->GetObjectSize(), dist, _owner->GetAngle(srcX, srcY) + M_PI);
                break;
            case 1:
                _owner->GetNearPoint(_owner, x, y, z, _owner->GetObjectSize(), dist*0.8, _owner->GetAngle(srcX, srcY) + M_PI);
                break;
            case 2:
                _owner->GetNearPoint(_owner, x, y, z, _owner->GetObjectSize(), dist*0.5, _owner->GetAngle(srcX, srcY) + M_PI);
                break;
            case 3:
                _owner->GetNearPoint(_owner, x, y, z, _owner->GetObjectSize(), dist*0.25, _owner->GetAngle(srcX, srcY) + M_PI);
                break;
            case 4:
                _owner->GetNearPoint(_owner, x, y, z, _owner->GetObjectSize(), dist*0.1, _owner->GetAngle(srcX, srcY) + M_PI);
                break;
        }

        if (_owner->IsWithinLOS(x, y, z))
        {
            if (Transport* transport = _owner->GetTransport())
                transport->CalculatePassengerOffset(x, y, z);

            Movement::MoveSplineInit init(*_owner);
            init.MoveTo(x, y, z);
            init.SetParabolic(max_height, 0);
            init.SetOrientationFixed(true);
            init.SetVelocity(speedXY);
            if (spellEffectExtraData)
                init.SetSpellEffectExtraData(*spellEffectExtraData);
            init.Launch();
            Mutate(new EffectMovementGenerator(0, x, y, z), MOTION_SLOT_CONTROLLED);
            break;
        }
    }
}

void MotionMaster::MoveJumpTo(float angle, float speedXY, float speedZ)
{
    //this function may make players fall below map
    if (_owner->IsPlayer())
        return;

    float x, y, z;

    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;
    _owner->GetClosePoint(x, y, z, _owner->GetObjectSize(), dist, angle);
    MoveJump(x, y, z, speedXY, speedZ);
}

void MotionMaster::MoveJump(Position const pos, float speedXY, float speedZ, uint32 id /*= 0*/, float o /*= 0.0f*/, DelayCastEvent* e /*= nullptr*/, Unit* target /*= nullptr*/, Movement::SpellEffectExtraData const* spellEffectExtraData /*= nullptr*/)
{
    MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), speedXY, speedZ, id, o, e, target, spellEffectExtraData);
}

void MotionMaster::MoveJump(float x, float y, float z, float speedXY, float speedZ, uint32 id /*= 0*/, float o /*= 0.0f*/, DelayCastEvent* e /*= nullptr*/, Unit* target /*= nullptr*/, Movement::SpellEffectExtraData const* spellEffectExtraData /*= nullptr*/)
{
    float moveTimeHalf = speedZ / Movement::gravity;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    _owner->AddUnitState(UNIT_STATE_LONG_JUMP);

    if ((id == 49575 || id == 102401) && _owner->IsPlayer())
    {
        speedXY *= 50.0f;
        max_height /= 2000.0f;
    }

    //TC_LOG_DEBUG(LOG_FILTER_SPELLS_AURAS, "MoveJump (GUID: %u) jump to (X: %f Y: %f Z: %f, max_height: %f speedXY %f)", _owner->GetGUIDLow(), x, y, z, max_height, speedXY);

    //if (!o)
        //o = _owner->GetOrientation();

    if (Transport* transport = _owner->GetTransport())
        transport->CalculatePassengerOffset(x, y, z);

    Movement::MoveSplineInit init(*_owner);
    init.MoveTo(x, y, z);
    if (speedZ)
        init.SetParabolic(max_height, 0);
    init.SetVelocity(speedXY);
    if (target)
        init.SetFacing(target);
    else if (o)
        init.SetFacing(o);
    if (spellEffectExtraData)
        init.SetSpellEffectExtraData(*spellEffectExtraData);

    init.Launch();
    Mutate(new EffectMovementGenerator(id, x, y, z, e), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveFall(uint32 id/*=0*/)
{
    // use larger distance for vmap height search than in most other cases
    float tz = _owner->GetHeight(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ(), true, MAX_FALL_DISTANCE);

    if (tz <= INVALID_HEIGHT)
    {
        // try fall down to ground
        tz = _owner->GetHeight(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ(), false, MAX_FALL_DISTANCE);
    }

    if (tz <= INVALID_HEIGHT)
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "MotionMaster::MoveFall: unable retrive a proper height at map %u (x: %f, y: %f, z: %f).",
            _owner->GetMap()->GetId(), _owner->GetPositionX(), _owner->GetPositionX(), _owner->GetPositionZ());
        return;
    }

    // Abort too if the ground is very near
    if (fabs(_owner->GetPositionZ() - tz) < 0.1f)
        return;

    if (_owner->IsPlayer())
    {
        _owner->AddUnitMovementFlag(MOVEMENTFLAG_FALLING);
        _owner->m_movementInfo.fall.SetFallTime(0);
        return;
    }

    Movement::MoveSplineInit init(*_owner);
    init.MoveTo(_owner->GetPositionX(), _owner->GetPositionY(), tz, false);
    init.SetFall();
    init.Launch();
    Mutate(new EffectMovementGenerator(id, _owner->GetPositionX(), _owner->GetPositionY(), tz), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveCirclePath(float x, float y, float z, float radius, bool clockwise, uint8 stepCount)
{
    float step = 2 * float(M_PI) / stepCount * (clockwise ? -1.0f : 1.0f);
    Position const& pos = { x, y, z, 0.0f };
    float angle = pos.GetAngle(_owner->GetPositionX(), _owner->GetPositionY());

    Movement::MoveSplineInit init(*_owner);

    for (uint8 i = 0; i < stepCount; angle += step, ++i)
    {
        G3D::Vector3 point;
        point.x = x + radius * cosf(angle);
        point.y = y + radius * sinf(angle);

        if (_owner->IsFlying())
            point.z = z;
        else
            point.z = _owner->GetHeight(point.x, point.y, z);

        init.Path().push_back(point);
    }

    if (_owner->IsFlying())
    {
        init.SetFly();    
        init.SetAnimation(Movement::ToFly);
    }
    else
        init.SetWalk(true);

    init.SetCyclic();

    init.Launch();
}

void MotionMaster::MoveSmoothPath(uint32 pointId, G3D::Vector3 pathPoint, bool walk)
{
    G3D::Vector3 const* p = nullptr;
    p = &pathPoint;
    MoveSmoothPath(pointId, p, 1, walk);
}

void MotionMaster::MoveSmoothPath(uint32 pointId, G3D::Vector3 const* pathPoints, size_t pathSize, bool walk)
{
    Movement::PointsArray path(pathPoints, pathPoints + pathSize);

    Movement::MoveSplineInit init(*_owner);
    init.MovebyPath(path);
    init.SetSmooth();
    init.SetWalk(walk);
    init.Launch();

    // This code is not correct
    // EffectMovementGenerator does not affect UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE
    // need to call PointMovementGenerator with various pointIds
    Mutate(new EffectMovementGenerator(pointId, pathPoints->x, pathPoints->y, pathPoints->z), MOTION_SLOT_ACTIVE);
    //Position pos(pathPoints[pathSize - 1].x, pathPoints[pathSize - 1].y, pathPoints[pathSize - 1].z);
    //MovePoint(EVENT_CHARGE_PREPATH, pos, false);
}

void MotionMaster::MoveSmoothFlyPath(uint32 pointID, G3D::Vector3 const* path, size_t size)
{
    Movement::MoveSplineInit init(*_owner);
    init.SetSmooth();
    init.SetFly();
    init.SetUncompressed();

    for (uint32 count = 0; count < uint32(size); ++count)
        init.Path().push_back(*path++);

    init.Launch();

    Mutate(new EffectMovementGenerator(pointID, path->x, path->y, path->z), MovementSlot::MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveSmoothFlyPath(uint32 pointID, Position const position, float flightSpeed/* = 0.0f*/)
{
    Movement::MoveSplineInit init(*_owner);
    init.SetSmooth();
    init.SetFly();
    init.SetUncompressed();
    init.MoveTo(position.m_positionX, position.m_positionY, position.m_positionZ, false, false);
    if (flightSpeed > 0.0f)
        init.SetVelocity(flightSpeed);
    init.Launch();

    Mutate(new EffectMovementGenerator(pointID, position.GetPositionX(), position.GetPositionY(), position.GetPositionZ()), MovementSlot::MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveCharge(Position const pos, float speed /*= SPEED_CHARGE*/, uint32 id /*= EVENT_CHARGE*/, bool generatePath /*= true*/)
{
    MoveCharge(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), speed, id, generatePath);
}

void MotionMaster::MoveCharge(float x, float y, float z, float speed /*= SPEED_CHARGE*/, uint32 id /*= EVENT_CHARGE*/, bool generatePath /*= true*/)
{
    if (Impl[MOTION_SLOT_CONTROLLED] && Impl[MOTION_SLOT_CONTROLLED]->GetMovementGeneratorType() != DISTRACT_MOTION_TYPE)
        return;

    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Player (GUID: %u) charge point (X: %f Y: %f Z: %f)", _owner->GetGUIDLow(), x, y, z);
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath, speed), MOTION_SLOT_CONTROLLED);
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) charge point (X: %f Y: %f Z: %f)",
            _owner->GetEntry(), _owner->GetGUIDLow(), x, y, z);
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath, speed), MOTION_SLOT_CONTROLLED);
    }
}

bool MotionMaster::SpellMoveCharge(float x, float y, float z, float speed, uint32 id, uint32 triggerspellId)
{
    if (Impl[MOTION_SLOT_CONTROLLED] && Impl[MOTION_SLOT_CONTROLLED]->GetMovementGeneratorType() != DISTRACT_MOTION_TYPE)
        return false;

    TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Unit (GUID: %u) charge point (X: %f Y: %f Z: %f)", _owner->GetGUIDLow(), x, y, z);

    Mutate(new ChargeMovementGenerator(id, x, y, z, speed, triggerspellId), MOTION_SLOT_CONTROLLED);

    return true;
}

void MotionMaster::MoveSeekAssistance(float x, float y, float z)
{
    if (_owner->IsPlayer())
    {
        TC_LOG_ERROR(LOG_FILTER_GENERAL, "Player (GUID: %u) attempt to seek assistance", _owner->GetGUIDLow());
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) seek assistance (X: %f Y: %f Z: %f)",
            _owner->GetEntry(), _owner->GetGUIDLow(), x, y, z);
        _owner->AttackStop();
        _owner->ToCreature()->SetReactState(REACT_PASSIVE);
        Mutate(new AssistanceMovementGenerator(x, y, z), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MoveSeekAssistanceDistract(uint32 time)
{
    if (_owner->IsPlayer())
    {
        TC_LOG_ERROR(LOG_FILTER_GENERAL, "Player (GUID: %u) attempt to call distract after assistance", _owner->GetGUIDLow());
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) is distracted after assistance call (Time: %u)",
            _owner->GetEntry(), _owner->GetGUIDLow(), time);
        Mutate(new AssistanceDistractMovementGenerator(time), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MoveFleeing(Unit* enemy, uint32 time)
{
    if (!enemy)
        return;

    if (_owner->HasAuraType(SPELL_AURA_PREVENTS_FLEEING))
        return;

    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Player (GUID: %u) flee from %s (GUID: %u)", _owner->GetGUIDLow(),
            enemy->IsPlayer() ? "player" : "creature",
            enemy->IsPlayer() ? enemy->GetGUIDLow() : enemy->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FleeingMovementGenerator<Player>(enemy->GetGUID()), MOTION_SLOT_CONTROLLED);
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) flee from %s (GUID: %u)%s",
            _owner->GetEntry(), _owner->GetGUIDLow(),
            enemy->IsPlayer() ? "player" : "creature",
            enemy->IsPlayer() ? enemy->GetGUIDLow() : enemy->ToCreature()->GetDBTableGUIDLow(),
            time ? " for a limited time" : "");
        if (time)
            Mutate(new TimedFleeingMovementGenerator(enemy->GetGUID(), time), MOTION_SLOT_CONTROLLED);
        else
            Mutate(new FleeingMovementGenerator<Creature>(enemy->GetGUID()), MOTION_SLOT_CONTROLLED);
    }
}

void MotionMaster::MoveTaxiFlight(uint32 path, uint32 pathnode)
{
    if (_owner->IsPlayer())
    {
        if (path < sTaxiPathNodesByPath.size())
        {
            TC_LOG_DEBUG(LOG_FILTER_GENERAL, "%s taxi to (Path %u node %u)", _owner->GetName(), path, pathnode);
            FlightPathMovementGenerator* mgen = new FlightPathMovementGenerator();
            mgen->LoadPath(*_owner->ToPlayer(), pathnode);
            Mutate(mgen, MOTION_SLOT_CONTROLLED);
        }
        else
        {
            TC_LOG_ERROR(LOG_FILTER_GENERAL, "%s attempt taxi to (not existed Path %u node %u)",
                _owner->GetName(), path, pathnode);
        }
    }
    else
    {
        TC_LOG_ERROR(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) attempt taxi to (Path %u node %u)",
            _owner->GetEntry(), _owner->GetGUIDLow(), path, pathnode);
    }
}

void MotionMaster::MoveDistract(uint32 timer)
{
    if (Impl[MOTION_SLOT_CONTROLLED])
        return;

    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Player (GUID: %u) distracted (timer: %u)", _owner->GetGUIDLow(), timer);
    }
    else
    {
        TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Creature (Entry: %u GUID: %u) (timer: %u)",
            _owner->GetEntry(), _owner->GetGUIDLow(), timer);
    }

    DistractMovementGenerator* mgen = new DistractMovementGenerator(timer);
    Mutate(mgen, MOTION_SLOT_CONTROLLED);
}

void MotionMaster::Mutate(MovementGenerator *m, MovementSlot slot)
{
    if (MovementGenerator *curr = Impl[slot])
    {
        Impl[slot] = nullptr; // in case a new one is generated in this slot during directdelete
        if (_top == slot && (_cleanFlag & MMCF_UPDATE))
            DelayedDelete(curr);
        else
            DirectDelete(curr);
    }
    else if (_top < slot)
    {
        _top = slot;
    }

    Impl[slot] = m;
    if (_top > slot)
        _needInit[slot] = true;
    else
    {
        _needInit[slot] = false;
        m->Initialize(*_owner);
    }
}

void MotionMaster::MovePath(uint32 path_id, bool repeatable, float randomMoveX, float randomMoveY)
{
    if (!path_id)
        return;

    if (_owner->IsPlayer())
        Mutate(new WaypointMovementGenerator<Player>(path_id), MOTION_SLOT_IDLE);
    else
        Mutate(new WaypointMovementGenerator<Creature>(path_id, repeatable, randomMoveX, randomMoveY), MOTION_SLOT_IDLE);

    TC_LOG_DEBUG(LOG_FILTER_GENERAL, "%s (GUID: %u) start moving over path(Id:%u, repeatable: %s)",
        _owner->IsPlayer() ? "Player" : "Creature",
        _owner->GetGUIDLow(), path_id, repeatable ? "YES" : "NO");
}

void MotionMaster::MoveRotate(uint32 time, RotateDirection direction, bool repeat /* = false*/)
{
    if (!time)
        return;

    Mutate(new RotateMovementGenerator(time, direction, repeat), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveBackward(uint32 id, float x, float y, float z, float speed)
{
    if (_owner->IsPlayer())
        _owner->AddUnitMovementFlag(MOVEMENTFLAG_BACKWARD);

    Movement::MoveSplineInit init(*_owner);
    init.MoveTo(x, y, z);
    init.SetBackward();
    init.Launch();
    if (speed > 0.0f)
        init.SetVelocity(speed);
    Mutate(new EffectMovementGenerator(id, x, y, z), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::propagateSpeedChange()
{
    /*Impl::container_type::iterator it = Impl::c.begin();
    for (; it != end(); ++it)
    {
        (*it)->unitSpeedChanged();
    }*/
    for (int i = 0; i <= _top; ++i)
    {
        if (Impl[i])
            Impl[i]->unitSpeedChanged();
    }
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType() const
{
    if (empty() || !top())
        return IDLE_MOTION_TYPE;

    return top()->GetMovementGeneratorType();
}

MovementGeneratorType MotionMaster::GetMotionSlotType(int slot) const
{
    if (!Impl[slot])
        return NULL_MOTION_TYPE;
    return Impl[slot]->GetMovementGeneratorType();
}

void MotionMaster::InitTop()
{
    top()->Initialize(*_owner);
    _needInit[_top] = false;
}

void MotionMaster::DirectDelete(_Ty curr)
{
    if (!curr)
        return;
    if (isStatic(curr))
        return;
    if (curr)
        curr->Finalize(*_owner);
    delete curr;
}

void MotionMaster::DelayedDelete(_Ty curr)
{
    if (!curr)
        return;
    TC_LOG_DEBUG(LOG_FILTER_GENERAL, "Unit (Entry %u) is trying to delete its updating MG (Type %u)!", _owner->GetEntry(), curr->GetMovementGeneratorType());
    if (isStatic(curr))
        return;
    if (!_expList)
        _expList = new ExpireList();
    _expList->push_back(curr);
}

bool MotionMaster::GetDestination(float &x, float &y, float &z)
{
    if (_owner->movespline->Finalized())
        return false;

    const G3D::Vector3& dest = _owner->movespline->FinalDestination();
    x = dest.x;
    y = dest.y;
    z = dest.z;
    return true;
}

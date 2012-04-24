/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2010-2011 ScriptDev0 <http://github.com/mangos-zero/scriptdev0>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Gyth
SD%Complete: 100
SDComment: Whole Event needs some rewrite
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"


#define    SPELL_CORROSIVEACID		16359						//20667, spell id is from old.wowhead.com
#define    SPELL_FREEZE             16350                       // ID was wrong!
#define    SPELL_FLAMEBREATH		16390						 //20712, spell id is from old.wowhead.com
#define    SPELL_ROOT_SELF          33356
#define	   SPELL_KNOCK_AWAY			10101						// this spell use this boss, information from old.wowhead.com
#define	   SPELL_SUMMONREND			16328						// summon Boss Rend blackhand

#define    MODEL_ID_INVISIBLE		11686
#define    MODEL_ID_GYTH_MOUNTED	9723
#define    MODEL_ID_GYTH			9806

#define    NPC_FIRE_TONGUE          10372
#define    NPC_CHROMATIC_WHELP      10442
#define    NPC_CHROMATIC_DRAGON     10447
#define    NPC_BLACKHAND_ELITE      10317
#define    NPC_REND_BLACKHAND       10429

//static Position Spawn = { 201.129f, -420.035f, 110.894f };
float fX1 = 200.049f;
float fY1 = -420.301f;
float fZ1 = 110.894f;
float fO = 3.070f;

float fX2 = 200.036f;
float fY2 = -417.669f;
float fZ2 = 110.894f;

float fX3 = 200.022f;
float fY3 = 415.051f;
float fZ3 = 110.890f;

float fX4 = 200.067f;
float fY4 = -423.786f;
float fZ4 = 110.891f;

float fX5 = 205.525f;
float fY5 = -425.451f;
float fZ5 = 110.913f;

float fX6 = 160.501f;
float fY6 = -420.693f;
float fZ6 = 110.472f;

struct MANGOS_DLL_DECL boss_gythAI : public ScriptedAI
{
    boss_gythAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*) pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;
    uint64 m_uiCombatDoorGUID;
    uint32 uiAggroTimer;
    uint32 uiDragonsTimer;
    uint32 uiOrcTimer;
    uint32 uiCorrosiveAcidTimer;
    uint32 uiFreezeTimer;
    uint32 uiFlamebreathTimer;
	uint32 uiKnockAwayTimer;
    uint32 uiLine1Count;
    uint32 uiLine2Count;

    bool m_bSummonedRend;
    bool m_bAggro;
    bool m_bRootSelf;

    void Reset()
    {
        uiDragonsTimer = 3000;
        uiOrcTimer = 60000;
        uiAggroTimer = 60000;
        uiCorrosiveAcidTimer = 8000;
        uiFreezeTimer = 11000;
        uiFlamebreathTimer = 4000;
		uiKnockAwayTimer = 20000;
        m_bSummonedRend = false;
        m_bAggro = false;
        m_bRootSelf = false;

        // how many times should the two lines of summoned creatures be spawned
        // min 2 x 2, max 7 lines of attack in total
        uiLine1Count = urand(2, 5);
        uiLine2Count = urand(2, 7 - uiLine1Count);

        // Invisible for event start
        m_creature->SetDisplayId(MODEL_ID_INVISIBLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_GYTH, IN_PROGRESS);
            m_uiCombatDoorGUID = m_pInstance->GetData64(GO_GYTH_COMBAT_DOOR);
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GYTH, DONE);
			DoCastSpellIfCan(m_creature, SPELL_SUMMONREND);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GYTH, FAIL);
    }

 /*   void SummonCreatureWithRandomTarget(uint32 uiCreatureId)
    {
        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 2*INTERACTION_DISTANCE, fX, fY, fZ);
        fX = std::min(m_creature->GetPositionX(), fX);      // Halfcircle - suits better the rectangular form
        if (Creature* pSummoned = m_creature->SummonCreature(uiCreatureId, fX, fY, fZ, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240000))
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
    } */

    void JustSummoned(Creature* summon)
    {
        switch (summon->GetEntry())
        {
            Unit* Target;
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                pTarget = Target;
            }
        case NPC_FIRE_TONGUE:
            summon->SetInCombatWithZone();
            if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            summon->AI()->AttackStart(Target);
            summon->Attack(Target, true);
            break;
        case NPC_CHROMATIC_WHELP:
            summon->SetInCombatWithZone();
            if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            summon->AI()->AttackStart(Target);
            summon->Attack(Target, true);
            break;
        case NPC_CHROMATIC_DRAGON:
            summon->SetInCombatWithZone();
            if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            summon->AI()->AttackStart(Target);
            summon->Attack(Target, true);
            break;
        case NPC_BLACKHAND_ELITE:
            summon->SetInCombatWithZone();
            if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            summon->AI()->AttackStart(Target);
            summon->Attack(Target, true);
            break;
        default:
            break;
        }
    }


    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bRootSelf)
        {
            DoCastSpellIfCan(m_creature, SPELL_ROOT_SELF);
            m_bRootSelf = true;
        }

        if (!m_bAggro && uiLine1Count == 0 && uiLine2Count == 0)
        {
            if (uiAggroTimer < uiDiff)
            {
                m_bAggro = true;
                // Visible now!
                m_creature->SetDisplayId(MODEL_ID_GYTH_MOUNTED);
                m_creature->setFaction(14);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveAurasDueToSpell(SPELL_ROOT_SELF);
                if (m_pInstance)
                    m_pInstance->DoUseDoorOrButton(m_uiCombatDoorGUID);

            }
            else
                uiAggroTimer -= uiDiff;
        }

        // Summon Dragon pack. 2 Dragons and 3 Whelps
        if (!m_bAggro && !m_bSummonedRend && uiLine1Count > 0)
        {
            if (uiDragonsTimer < uiDiff)
            {
                Unit* Target;
                Target = NULL;
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    pTarget = Target;
                }
                m_creature->SummonCreature(NPC_FIRE_TONGUE, fX1, fY1, fZ1, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_FIRE_TONGUE, fX2, fY2, fZ2, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_CHROMATIC_WHELP, fX3, fY3, fZ3, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_CHROMATIC_WHELP, fX4, fY4, fZ4, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_CHROMATIC_WHELP, fX5, fY5, fZ5, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                --uiLine1Count;
                if (m_pInstance)
                    m_pInstance->DoUseDoorOrButton(m_uiCombatDoorGUID);
                uiDragonsTimer = 60000;
            }
            else
                uiDragonsTimer -= uiDiff;
        }

        //Summon Orc pack. 1 Orc Handler 1 Elite Dragonkin and 3 Whelps
        if (!m_bAggro && !m_bSummonedRend && uiLine1Count == 0 && uiLine2Count > 0)
        {
            if (uiOrcTimer < uiDiff)
            {
                m_creature->SummonCreature(NPC_CHROMATIC_DRAGON, fX1, fY1, fZ1, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_BLACKHAND_ELITE, fX2, fY2, fZ2, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_CHROMATIC_WHELP, fX3, fY3, fZ3, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_CHROMATIC_WHELP, fX4, fY4, fZ4, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                m_creature->SummonCreature(NPC_CHROMATIC_WHELP, fX5, fY5, fZ5, fO, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS)->GetMotionMaster()->MovePoint(0, fX6, fY6, fZ6);
                if (m_pInstance)
                    m_pInstance->DoUseDoorOrButton(m_uiCombatDoorGUID);
                --uiLine2Count;
                uiOrcTimer = 60000;
            }
            else
                uiOrcTimer -= uiDiff;
        }

        // we take part in the fight
        if (m_bAggro)
        {
             // CorrosiveAcid_Timer
            if (uiCorrosiveAcidTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_CORROSIVEACID);
                uiCorrosiveAcidTimer = 7000;
            }
            else
                uiCorrosiveAcidTimer -= uiDiff;

            // Freeze_Timer
            if (uiFreezeTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FREEZE) == CAST_OK)
                    uiFreezeTimer = 16000;
            }
            else
                uiFreezeTimer -= uiDiff;

            // Flamebreath_Timer
            if (uiFlamebreathTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_FLAMEBREATH);
                uiFlamebreathTimer = 10500;
            }
            else
                uiFlamebreathTimer -= uiDiff;
			
			// KNockAway_Timer
			if (uiKnockAwayTimer < uiDiff)
			{
				DoCastSpellIfCan(m_creature, SPELL_KNOCK_AWAY);
				uiKnockAwayTimer = 25000;
			}
			else
				uiKnockAwayTimer -= uiDiff;


            //Summon Rend
            if (!m_bSummonedRend && m_creature->GetHealthPercent() < 11.0f)
            {
                // summon Rend and Change model to normal Gyth
                // Inturrupt any spell casting
                m_creature->InterruptNonMeleeSpells(false);
                // Gyth model
                m_creature->SetDisplayId(MODEL_ID_GYTH);
                m_creature->SummonCreature(NPC_REND_BLACKHAND, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 900000);
                m_bSummonedRend = true;
            }

            DoMeleeAttackIfReady();
        }                                                   // end if Aggro
    }
};
// test
CreatureAI* GetAI_boss_gyth(Creature* pCreature)
{
    return new boss_gythAI(pCreature);
}

void AddSC_boss_gyth()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_gyth";
    pNewScript->GetAI = &GetAI_boss_gyth;
    pNewScript->RegisterSelf();
}

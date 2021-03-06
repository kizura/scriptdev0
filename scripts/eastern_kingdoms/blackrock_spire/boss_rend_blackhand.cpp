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
SDName: Boss_Rend_Blackhand
SD%Complete: 100
SDComment: Intro event NYI
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"


#define    SPELL_WHIRLWIND		15589	  							//26038
#define    SPELL_CLEAVE			15284								//20691
//#define    SPELL_THUNDERCLAP  23931                               //Not sure if he cast this spell. ****this spell use this boss not.
#define	   SPELL_FRENZY			8269								// boss use this spell, information from old.wowhead.com
#define	   SPELL_MORTAL_STRIKE	16856								// boss use this spell, information from old.wwohead.com


struct MANGOS_DLL_DECL boss_rend_blackhandAI : public ScriptedAI
{
    boss_rend_blackhandAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiWhirlWindTimer;
    uint32 m_uiCleaveTimer;
   // uint32 m_uiThunderclapTimer;
	uint32 m_uiFrenzyTimer;
	uint32 m_uiMortalStrikeTimer;

    void Reset()
    {
        m_uiWhirlWindTimer		= 20000;
        m_uiCleaveTimer			= 5000;
       // m_uiThunderclapTimer = 9000;,
		m_uiFrenzyTimer			= 9000;
		m_uiMortalStrikeTimer	= 15000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // WhirlWind
        if (m_uiWhirlWindTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND);
            m_uiWhirlWindTimer = 18000;
        }
        else
            m_uiWhirlWindTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = 10000;
        }
        else
            m_uiCleaveTimer -= uiDiff;
		/*
        // Thunderclap
        if (m_uiThunderclapTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_THUNDERCLAP);
            m_uiThunderclapTimer = 16000;
        }
        else
            m_uiThunderclapTimer -= uiDiff; */

		// Frenzy
		if (m_uiFrenzyTimer < uiDiff)
		{
			DoCastSpellIfCan(m_creature, SPELL_FRENZY);
			m_uiFrenzyTimer = 16000;
		}
		else
			m_uiFrenzyTimer -= uiDiff;

		// MortalStrike
		if (m_uiMortalStrikeTimer < uiDiff)
		{
			if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			DoCastSpellIfCan(pTarget, SPELL_MORTAL_STRIKE);
				m_uiMortalStrikeTimer = 12500;
		}
		else
			m_uiMortalStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rend_blackhand(Creature* pCreature)
{
    return new boss_rend_blackhandAI(pCreature);
}

void AddSC_boss_rend_blackhand()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_rend_blackhand";
    newscript->GetAI = &GetAI_boss_rend_blackhand;
    newscript->RegisterSelf();
}

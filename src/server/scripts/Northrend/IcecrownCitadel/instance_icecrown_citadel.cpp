/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AccountMgr.h"
#include "AreaDefines.h"
#include "CreatureTextMgr.h"
#include "Group.h"
#include "InstanceMapScript.h"
#include "InstanceScript.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "Transport.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "WorldStateDefines.h"
#include "icecrown_citadel.h"

enum EventIds
{
    EVENT_PLAYERS_GUNSHIP_SPAWN     = 22663,
    EVENT_PLAYERS_GUNSHIP_COMBAT    = 22664,
    EVENT_PLAYERS_GUNSHIP_SAURFANG  = 22665,
    EVENT_ENEMY_GUNSHIP_COMBAT      = 22860,
    EVENT_ENEMY_GUNSHIP_DESPAWN     = 22861,

    EVENT_QUAKE                     = 23437,
    EVENT_SECOND_REMORSELESS_WINTER = 23507,
    EVENT_TELEPORT_TO_FROSMOURNE    = 23617,
    EVENT_FESTERGUT_VALVE_USED      = 23438,
    EVENT_ROTFACE_VALVE_USED        = 23426,
};

enum TimedEvents
{
    EVENT_UPDATE_EXECUTION_TIME = 1,
    EVENT_QUAKE_SHATTER         = 2,
    EVENT_REBUILD_PLATFORM      = 3,
    EVENT_RESPAWN_GUNSHIP       = 4,
    EVENT_RESPAWN_SINDRAGOSA    = 5
};

enum Spells
{
    SPELL_GAS_VARIABLE      = 74119,
    SPELL_OOZE_VARIABLE     = 74118,
    BLOOD_BEAM_VISUAL_RHAND = 72304,
    BLOOD_BEAM_VISUAL_LHAND = 72303,
    BLOOD_BEAM_VISUAL_LLEG  = 72302,
    BLOOD_BEAM_VISUAL_RLEG  = 72301,
    VOID_ZONE_VISUAL        = 69422
};

enum Say
{
    SAY_SOULS_LICH_KING_RAND_WHISPER = 5
};

BossBoundaryData const boundaries =
{
    { DATA_LORD_MARROWGAR, new CircleBoundary(Position(-428.0f,2211.0f), 95.0) },
    { DATA_LORD_MARROWGAR, new RectangleBoundary(-430.0f, -330.0f, 2110.0f, 2310.0f) },
    { DATA_LADY_DEATHWHISPER, new RectangleBoundary(-670.0f, -520.0f, 2145.0f, 2280.0f) },
    { DATA_DEATHBRINGER_SAURFANG, new RectangleBoundary(-565.0f, -465.0f, 2160.0f, 2260.0f) },

    { DATA_ROTFACE, new RectangleBoundary(4385.0f, 4505.0f, 3082.0f, 3195.0f) },
    { DATA_FESTERGUT, new RectangleBoundary(4205.0f, 4325.0f, 3082.0f, 3195.0f) },
    { DATA_PROFESSOR_PUTRICIDE, new ParallelogramBoundary(Position(4356.0f, 3290.0f), Position(4435.0f, 3194.0f), Position(4280.0f, 3194.0f)) },
    { DATA_PROFESSOR_PUTRICIDE, new RectangleBoundary(4280.0f, 4435.0f, 3150.0f, 4360.0f) },

    { DATA_BLOOD_PRINCE_COUNCIL, new EllipseBoundary(Position(4660.95f, 2769.194f), 85.0, 60.0) },
    { DATA_BLOOD_QUEEN_LANA_THEL, new CircleBoundary(Position(4595.93f, 2769.365f), 64.0) },

    { DATA_SISTER_SVALNA, new RectangleBoundary(4291.0f, 4423.0f, 2438.0f, 2653.0f) },
    { DATA_VALITHRIA_DREAMWALKER, new RectangleBoundary(4112.5f, 4293.5f, 2385.0f, 2585.0f) },
    { DATA_SINDRAGOSA, new EllipseBoundary(Position(4418.6f, 2484.0f), 110.0, 75.0) }
};

DoorData const doorData[] =
{
    {GO_LORD_MARROWGAR_S_ENTRANCE,           DATA_LORD_MARROWGAR,        DOOR_TYPE_ROOM        },
    {GO_SCOURGE_TRANSPORTER_FIRST,           DATA_LORD_MARROWGAR,        DOOR_TYPE_PASSAGE     },
    {GO_ICEWALL,                             DATA_LORD_MARROWGAR,        DOOR_TYPE_PASSAGE     },
    {GO_DOODAD_ICECROWN_ICEWALL02,           DATA_LORD_MARROWGAR,        DOOR_TYPE_PASSAGE     },
    {GO_ORATORY_OF_THE_DAMNED_ENTRANCE,      DATA_LADY_DEATHWHISPER,     DOOR_TYPE_ROOM        },
    {GO_SAURFANG_S_DOOR,                     DATA_DEATHBRINGER_SAURFANG, DOOR_TYPE_PASSAGE     },
    {GO_ORANGE_PLAGUE_MONSTER_ENTRANCE,      DATA_FESTERGUT,             DOOR_TYPE_ROOM        },
    {GO_GREEN_PLAGUE_MONSTER_ENTRANCE,       DATA_ROTFACE,               DOOR_TYPE_ROOM        },
    {GO_CRIMSON_HALL_DOOR,                   DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_ROOM        },
    {GO_CRIMSON_HALL_DOOR,                   DATA_BLOOD_PRINCE_TRASH,    DOOR_TYPE_PASSAGE     },
    {GO_BLOOD_ELF_COUNCIL_DOOR,              DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_PASSAGE    },
    {GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT,        DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_PASSAGE    },
    {GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01, DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_ROOM       },
    {GO_DOODAD_ICECROWN_GRATE_01,            DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_PASSAGE    },
    {GO_GREEN_DRAGON_BOSS_ENTRANCE,          DATA_SISTER_SVALNA,         DOOR_TYPE_PASSAGE    },
    {GO_GREEN_DRAGON_BOSS_ENTRANCE,          DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_ROOM       },
    {GO_GREEN_DRAGON_BOSS_EXIT,              DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_PASSAGE    },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE },
    {GO_SINDRAGOSA_ENTRANCE_DOOR,            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM       },
    {GO_SINDRAGOSA_ENTRANCE_DOOR,            DATA_SINDRAGOSA_GAUNTLET,   DOOR_TYPE_PASSAGE    },
    {GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR,   DATA_SINDRAGOSA,            DOOR_TYPE_PASSAGE    },
    {GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR,       DATA_SINDRAGOSA,            DOOR_TYPE_PASSAGE    },
    {GO_ICE_WALL,                            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM       },
    {GO_ICE_WALL,                            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM       },
    {0,                                      0,                          DOOR_TYPE_ROOM       }
};

ObjectData const creatureData[] =
{
    { NPC_SINDRAGOSA,     DATA_SINDRAGOSA     },
    { NPC_THE_SKYBREAKER, DATA_THE_SKYBREAKER },
    { NPC_ORGRIMS_HAMMER, DATA_ORGRIMS_HAMMER },
    { 0,                  0                   }
};

// this doesnt have to only store questgivers, also can be used for related quest spawns
struct WeeklyQuest
{
    uint32 creatureEntry;
    uint32 questId[2];  // 10 and 25 man versions
};

// when changing the content, remember to update SetData, DATA_BLOOD_QUICKENING_STATE case for NPC_ALRIN_THE_AGILE index
WeeklyQuest const WeeklyQuestData[WeeklyNPCs] =
{
    {NPC_INFILTRATOR_MINCHAR,         {QUEST_DEPROGRAMMING_10,                 QUEST_DEPROGRAMMING_25                }}, // Deprogramming
    {NPC_KOR_KRON_LIEUTENANT,         {QUEST_SECURING_THE_RAMPARTS_10,         QUEST_SECURING_THE_RAMPARTS_25        }}, // Securing the Ramparts
    {NPC_ROTTING_FROST_GIANT_10,      {QUEST_SECURING_THE_RAMPARTS_10,         QUEST_SECURING_THE_RAMPARTS_25        }}, // Securing the Ramparts
    {NPC_ROTTING_FROST_GIANT_25,      {QUEST_SECURING_THE_RAMPARTS_10,         QUEST_SECURING_THE_RAMPARTS_25        }}, // Securing the Ramparts
    {NPC_ALCHEMIST_ADRIANNA,          {QUEST_RESIDUE_RENDEZVOUS_10,            QUEST_RESIDUE_RENDEZVOUS_25           }}, // Residue Rendezvous
    {NPC_ALRIN_THE_AGILE,             {QUEST_BLOOD_QUICKENING_10,              QUEST_BLOOD_QUICKENING_25             }}, // Blood Quickening
    {NPC_INFILTRATOR_MINCHAR_BQ,      {QUEST_BLOOD_QUICKENING_10,              QUEST_BLOOD_QUICKENING_25             }}, // Blood Quickening
    {NPC_MINCHAR_BEAM_STALKER,        {QUEST_BLOOD_QUICKENING_10,              QUEST_BLOOD_QUICKENING_25             }}, // Blood Quickening
    {NPC_VALITHRIA_DREAMWALKER_QUEST, {QUEST_RESPITE_FOR_A_TORMENTED_SOUL_10,  QUEST_RESPITE_FOR_A_TORMENTED_SOUL_25 }}, // Respite for a Tormented Soul
};

Position const JainaSpawnPos    = { -48.65278f, 2211.026f, 27.98586f, 3.124139f };
Position const MuradinSpawnPos  = { -47.34549f, 2208.087f, 27.98586f, 3.106686f };
Position const UtherSpawnPos    = { -26.58507f, 2211.524f, 30.19898f, 3.124139f };
Position const SylvanasSpawnPos = { -41.45833f, 2222.891f, 27.98586f, 3.647738f };
Position const SindragosaSpawnPos = { 4818.6997f, 2483.7102f, 287.06497f, 3.286661f };

// Set position traps Spirit Alarm
std::vector<Position> GoSpiritAlarm_1 = { { -160.96f, 2210.46f, 35.24f, 0.0f }, { -176.27f, 2201.93f, 35.24f, 0.0f}, { -207.83f, 2207.38f, 35.24f, 0.0f } };
std::vector<Position> GoSpiritAlarm_2 = { { -178.41f, 2225.11f, 35.24f, 0.0f }, { -195.23f, 2221.55f, 35.24f, 0.0f}, { -209.94f, 2250.34f, 37.99f, 0.0f } };
std::vector<Position> GoSpiritAlarm_3 = { { -289.80f, 2216.60f, 42.39f, 0.0f }, { -317.76f, 2216.11f, 42.57f, 0.0f}, { -301.07f, 2216.62f, 42.0f, 0.0f } };
std::vector<Position> GoSpiritAlarm_4 = { { -276.07f, 2206.76f, 42.57f, 0.0f }, { -304.44f, 2199.11f, 41.99f, 0.0f}, { -292.82f, 2204.61f, 42.02f, 0.0f } };

class RespawnEvent : public BasicEvent
{
public:
    RespawnEvent(Creature& owner) : _owner(owner) { }

    bool Execute(uint64 /*eventTime*/, uint32 /*updateTime*/) override
    {
        _owner.RemoveCorpse(false);
        _owner.Respawn();
        return true;
    }

private:
    Creature& _owner;
};

class DelayedCastMincharEvent : public BasicEvent
{
public:
    DelayedCastMincharEvent(Creature* trigger, uint32 spellId) : _trigger(trigger), _spellId(spellId) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        if (Creature* minchar = _trigger->FindNearestCreature(NPC_INFILTRATOR_MINCHAR_BQ, 50.0f, true))
            _trigger->CastSpell(minchar, _spellId, true);
        return true;
    }

private:
    Creature* _trigger;
    uint32 _spellId;
};

class instance_icecrown_citadel : public InstanceMapScript
{
public:
    instance_icecrown_citadel() : InstanceMapScript(ICCScriptName, MAP_ICECROWN_CITADEL) { }

    struct instance_icecrown_citadel_InstanceMapScript : public InstanceScript
    {
        instance_icecrown_citadel_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            // pussywizard:
            IsBuffAvailable = true;
            WeeklyQuestId10 = 0;
            PutricideEventProgress = 0;
            LichKingHeroicAvailable = true;
            LichKingRandomWhisperTimer = 120 * IN_MILLISECONDS;
            DarkwhisperElevatorTimer = 3000;

            SetHeaders(DataHeader);
            SetBossNumber(MAX_ENCOUNTERS);
            SetPersistentDataCount(MAX_DATA_INDEXES);
            LoadBossBoundaries(boundaries);
            LoadObjectData(creatureData, nullptr);
            LoadDoorData(doorData);
            HeroicAttempts = MaxHeroicAttempts;
            IsBonedEligible = true;
            IsOozeDanceEligible = true;
            IsNauseaEligible = true;
            IsOrbWhispererEligible = true;
            ColdflameJetsState = NOT_STARTED;
            BloodQuickeningState = NOT_STARTED;
            BloodQuickeningMinutes = 0;
            BloodPrinceTrashCount = 0;
            IsSindragosaIntroDone = false;
        }

        void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override
        {
            packet.Worldstates.reserve(5);
            packet.Worldstates.emplace_back(WORLD_STATE_ICECROWN_CITADEL_SHOW_TIMER, BloodQuickeningState == IN_PROGRESS ? 1 : 0);
            packet.Worldstates.emplace_back(WORLD_STATE_ICECROWN_CITADEL_EXECUTION_TIME, BloodQuickeningMinutes);
            packet.Worldstates.emplace_back(WORLD_STATE_ICECROWN_CITADEL_SHOW_ATTEMPTS, 1); // instance->IsHeroic() ? 1 : 0
            packet.Worldstates.emplace_back(WORLD_STATE_ICECROWN_CITADEL_ATTEMPTS_REMAINING, HeroicAttempts);
            packet.Worldstates.emplace_back(WORLD_STATE_ICECROWN_CITADEL_ATTEMPTS_MAX, MaxHeroicAttempts);
        }

        void OnPlayerAreaUpdate(Player* player, uint32  /*oldArea*/, uint32 newArea) override
        {
            if (newArea == AREA_PUTRICIDES_LABORATORY_OF_ALCHEMICAL_HORRORS_AND_FUN ||
                    newArea == AREA_THE_SANCTUM_OF_BLOOD ||
                    newArea == AREA_THE_FROST_QUEENS_LAIR ||
                    newArea == AREA_THE_FROZEN_THRONE ||
                    newArea == AREA_FROSTMOURNE)
            {
                player->SendInitWorldStates(player->GetZoneId(), player->GetAreaId());
            }
            else
            {
                player->SendUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_SHOW_ATTEMPTS, 0);
            }
        }

        void OnPlayerEnter(Player* player) override
        {
            InstanceScript::OnPlayerEnter(player);
            // for professor putricide hc
            DoRemoveAurasDueToSpellOnPlayers(SPELL_GAS_VARIABLE);
            DoRemoveAurasDueToSpellOnPlayers(SPELL_OOZE_VARIABLE);

            if (GetBossState(DATA_LADY_DEATHWHISPER) == DONE && GetBossState(DATA_ICECROWN_GUNSHIP_BATTLE) != DONE)
                SpawnGunship();

            if (GetBossState(DATA_SINDRAGOSA) != DONE && IsSindragosaIntroDone && !GetCreature(DATA_SINDRAGOSA) && Events.GetTimeUntilEvent(EVENT_RESPAWN_SINDRAGOSA) == Milliseconds::max())
            {
                Events.ScheduleEvent(EVENT_RESPAWN_SINDRAGOSA, 30s);
            }

            if (IsBuffAvailable)
            {
                SpellAreaForAreaMapBounds saBounds = sSpellMgr->GetSpellAreaForAreaMapBounds(4812);
                for (SpellAreaForAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
                    if ((itr->second->raceMask & player->getRaceMask()) && !player->HasAura(itr->second->spellId))
                    {
                        if (SpellInfo const* si = sSpellMgr->GetSpellInfo(itr->second->spellId))
                        {
                            if (si->HasAura(SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT))
                            {
                                DoCastSpellOnPlayer(player, itr->second->spellId, false, false);
                            }
                        }
                    }
            }
        }

        void OnCreatureCreate(Creature* creature) override
        {
            // apply ICC buff to pets/summons
            if (GetData(DATA_BUFF_AVAILABLE) && creature->GetOwnerGUID().IsPlayer() && creature->HasUnitTypeMask(UNIT_MASK_MINION | UNIT_MASK_GUARDIAN | UNIT_MASK_CONTROLLABLE_GUARDIAN) && creature->CanHaveThreatList())
                if (Unit* owner = creature->GetOwner())
                    if (Player* plr = owner->ToPlayer())
                    {
                        SpellAreaForAreaMapBounds saBounds = sSpellMgr->GetSpellAreaForAreaMapBounds(4812);
                        for (SpellAreaForAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
                            if ((itr->second->raceMask & plr->getRaceMask()) && !creature->HasAura(itr->second->spellId))
                                if (SpellInfo const* si = sSpellMgr->GetSpellInfo(itr->second->spellId))
                                    if (si->HasAura(SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT))
                                        creature->AddAura(itr->second->spellId, creature);
                    }

            // fighting npcs in Rampart of Skulls
            std::string name1("Skybreaker ");
            std::string name2("Kor'kron ");
            if (!creature->GetTransport() && creature->GetPositionZ() <= 205.0f && creature->GetExactDist2d(-439.0f, 2210.0f) <= 150.0f && (creature->GetEntry() == 37544 || creature->GetEntry() == 37545 || creature->GetName().compare(0, name1.length(), name1) == 0 || creature->GetName().compare(0, name2.length(), name2) == 0))
                creature->AddToNotify(NOTIFY_AI_RELOCATION);

            // pussywizard: check weekly here, before possible UpdateEntry
            // allow creating all of them, because after killing Marrowgar some have to appear, so just hide them
            switch (creature->GetEntry())
            {
                case NPC_INFILTRATOR_MINCHAR:
                case NPC_KOR_KRON_LIEUTENANT:
                case NPC_ALCHEMIST_ADRIANNA:
                case NPC_ALRIN_THE_AGILE:
                case NPC_INFILTRATOR_MINCHAR_BQ:
                case NPC_MINCHAR_BEAM_STALKER:
                case NPC_VALITHRIA_DREAMWALKER_QUEST:
                    for (uint8 i = 0; i < WeeklyNPCs; ++i)
                        if (WeeklyQuestData[i].creatureEntry == creature->GetEntry())
                        {
                            WeeklyQuestNpcGUID[i] = creature->GetGUID();
                            if (WeeklyQuestId10 != WeeklyQuestData[i].questId[0])
                                creature->SetVisible(false);
                            else if (WeeklyQuestData[i].creatureEntry == NPC_VALITHRIA_DREAMWALKER_QUEST && GetBossState(DATA_VALITHRIA_DREAMWALKER) != DONE)
                                creature->SetVisible(false);
                        }
                    break;
            }

            switch (creature->GetEntry())
            {
                case NPC_KOR_KRON_GENERAL:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_ALLIANCE_COMMANDER);
                    break;
                case NPC_KOR_KRON_LIEUTENANT:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_SKYBREAKER_LIEUTENANT);
                    break;
                case NPC_TORTUNOK:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_ALANA_MOONSTRIKE);
                    break;
                case NPC_GERARDO_THE_SUAVE:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_TALAN_MOONSTRIKE);
                    break;
                case NPC_UVLUS_BANEFIRE:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_MALFUS_GRIMFROST);
                    break;
                case NPC_IKFIRUS_THE_VILE:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_YILI);
                    break;
                case NPC_VOL_GUK:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_JEDEBIA);
                    break;
                case NPC_HARAGG_THE_UNSEEN:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_NIBY_THE_ALMIGHTY);
                    break;
                case NPC_GARROSH_HELLSCREAM:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_KING_VARIAN_WRYNN);

                    // Xinef: summon in case of instance unload
                    if (GetBossState(DATA_THE_LICH_KING) == DONE)
                    {
                        instance->SummonCreature(NPC_LADY_JAINA_PROUDMOORE_QUEST, JainaSpawnPos);
                        instance->SummonCreature(NPC_MURADIN_BRONZEBEARD_QUEST, MuradinSpawnPos);
                        instance->SummonCreature(NPC_UTHER_THE_LIGHTBRINGER_QUEST, UtherSpawnPos);
                        instance->SummonCreature(NPC_LADY_SYLVANAS_WINDRUNNER_QUEST, SylvanasSpawnPos);
                    }
                    break;
                case NPC_LADY_DEATHWHISPER:
                    LadyDeathwhisperGUID = creature->GetGUID();
                    break;
                case NPC_DEATHBRINGER_SAURFANG:
                    DeathbringerSaurfangGUID = creature->GetGUID();
                    break;
                case NPC_SE_HIGH_OVERLORD_SAURFANG:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                    {
                        creature->UpdateEntry(NPC_SE_MURADIN_BRONZEBEARD, true);
                        creature->LoadEquipment();
                    }
                    DeathbringerSaurfangEventGUID = creature->GetGUID();
                    creature->LastUsedScriptID = creature->GetScriptId();
                    break;
                case NPC_SE_MURADIN_BRONZEBEARD:
                    DeathbringerSaurfangEventGUID = creature->GetGUID();
                    break;
                case NPC_HIGH_OVERLORD_SAURFANG_DUMMY:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                    {
                        creature->UpdateEntry(NPC_MURADIN_BRONZEBEARD_DUMMY, creature->GetCreatureData());
                        creature->LoadEquipment();
                    }
                    break;
                case NPC_SE_KOR_KRON_REAVER:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_SE_SKYBREAKER_MARINE);
                    break;
                case NPC_FESTERGUT:
                    FestergutGUID = creature->GetGUID();
                    break;
                case NPC_ROTFACE:
                    RotfaceGUID = creature->GetGUID();
                    break;
                case NPC_PROFESSOR_PUTRICIDE:
                    ProfessorPutricideGUID = creature->GetGUID();
                    if (GetBossState(DATA_ROTFACE) == DONE && GetBossState(DATA_FESTERGUT) == DONE && !HeroicAttempts && GetData(DATA_HAS_LIMITED_ATTEMPTS) && creature->IsAlive())
                        creature->SetVisible(false);
                    break;
                case NPC_PRINCE_KELESETH:
                    BloodCouncilGUIDs[0] = creature->GetGUID();
                    break;
                case NPC_PRINCE_TALDARAM:
                    BloodCouncilGUIDs[1] = creature->GetGUID();
                    break;
                case NPC_PRINCE_VALANAR:
                    BloodCouncilGUIDs[2] = creature->GetGUID();
                    break;
                case NPC_BLOOD_ORB_CONTROLLER:
                    BloodCouncilControllerGUID = creature->GetGUID();
                    break;
                case NPC_BLOOD_QUEEN_LANA_THEL:
                    BloodQueenLanaThelGUID = creature->GetGUID();
                    if (!HeroicAttempts && GetData(DATA_HAS_LIMITED_ATTEMPTS) && creature->IsAlive())
                        creature->SetVisible(false);
                    break;
                case NPC_CROK_SCOURGEBANE:
                    CrokScourgebaneGUID = creature->GetGUID();
                    break;
                // we can only do this because there are no gaps in their entries
                case NPC_CAPTAIN_ARNATH:
                case NPC_CAPTAIN_BRANDON:
                case NPC_CAPTAIN_GRONDEL:
                case NPC_CAPTAIN_RUPERT:
                    CrokCaptainGUIDs[creature->GetEntry() - NPC_CAPTAIN_ARNATH] = creature->GetGUID();
                    break;
                case NPC_SISTER_SVALNA:
                    SisterSvalnaGUID = creature->GetGUID();
                    break;
                case NPC_VALITHRIA_DREAMWALKER:
                    ValithriaDreamwalkerGUID = creature->GetGUID();
                    break;
                case NPC_THE_LICH_KING_VALITHRIA:
                    ValithriaLichKingGUID = creature->GetGUID();
                    break;
                case NPC_THE_LICH_KING_LH:
                    TheLichKingLhGUID = creature->GetGUID();
                    break;
                case NPC_GREEN_DRAGON_COMBAT_TRIGGER:
                    ValithriaTriggerGUID = creature->GetGUID();
                    break;
                case NPC_PUTRICADES_TRAP:
                    PutricadeTrapGUID = creature->GetGUID();
                    break;
                case NPC_SINDRAGOSA_GAUNTLET:
                    SindragosaGauntletGUID = creature->GetGUID();
                    break;
                case NPC_SINDRAGOSA:
                    SindragosaGUID = creature->GetGUID();
                    if (!HeroicAttempts && GetData(DATA_HAS_LIMITED_ATTEMPTS) && creature->IsAlive())
                        creature->SetVisible(false);
                    break;
                case NPC_SPINESTALKER:
                    SpinestalkerGUID = creature->GetGUID();
                    break;
                case NPC_RIMEFANG:
                    RimefangGUID = creature->GetGUID();
                    break;
                case NPC_INVISIBLE_STALKER:
                    // Teleporter visual at center
                    if (creature->GetExactDist2d(4357.052f, 2769.421f) < 10.0f && GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && GetBossState(DATA_SINDRAGOSA) == DONE)
                        creature->CastSpell(creature, SPELL_ARTHAS_TELEPORTER_CEREMONY, false);
                    break;
                case NPC_THE_LICH_KING:
                    TheLichKingGUID = creature->GetGUID();
                    if (!HeroicAttempts && GetData(DATA_HAS_LIMITED_ATTEMPTS) && creature->IsAlive())
                        creature->SetVisible(false);
                    break;
                case NPC_HIGHLORD_TIRION_FORDRING_LK:
                    HighlordTirionFordringGUID = creature->GetGUID();
                    break;
                case NPC_TERENAS_MENETHIL_FROSTMOURNE:
                case NPC_TERENAS_MENETHIL_FROSTMOURNE_H:
                    TerenasMenethilGUID = creature->GetGUID();
                    break;
                case NPC_INFILTRATOR_MINCHAR_BQ:
                    if (BloodQuickeningState == DONE)
                        creature->DespawnOrUnsummon(1);
                    break;
                case NPC_MINCHAR_BEAM_STALKER:
                    if (BloodQuickeningState != DONE)
                    {
                        uint32 spellId = 0;
                        if (creature->GetPositionY() > 2790.0f && creature->GetPositionZ() > 420.0f)
                            spellId = BLOOD_BEAM_VISUAL_RHAND;
                        else if (creature->GetPositionY() < 2790.0f && creature->GetPositionZ() > 420.0f)
                            spellId = BLOOD_BEAM_VISUAL_LHAND;
                        else if (creature->GetPositionY() < 2790.0f && creature->GetPositionZ() < 420.0f)
                            spellId = BLOOD_BEAM_VISUAL_LLEG;
                        else
                            spellId = BLOOD_BEAM_VISUAL_RLEG;
                        creature->m_Events.AddEvent(new DelayedCastMincharEvent(creature, spellId), creature->m_Events.CalculateTime(1000));
                    }
                    break;
                case NPC_SKYBREAKER_DECKHAND:
                case NPC_ORGRIMS_HAMMER_CREW:
                    if (!creature->IsAlive())
                        creature->Respawn();
                    break;
                default:
                    break;
            }

            InstanceScript::OnCreatureCreate(creature);

        }

        void OnCreatureRemove(Creature* creature) override
        {
            if (creature->GetEntry() == NPC_SINDRAGOSA)
                SindragosaGUID.Clear();

            InstanceScript::OnCreatureRemove(creature);
        }

        uint32 GetCreatureEntry(ObjectGuid::LowType /*guidLow*/, CreatureData const* data) override
        {
            uint32 entry = data->id1;
            switch (entry)
            {
                case NPC_HORDE_GUNSHIP_CANNON:
                case NPC_ORGRIMS_HAMMER_CREW:
                case NPC_SKY_REAVER_KORM_BLACKSCAR:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        return 0;
                    break;
                case NPC_ALLIANCE_GUNSHIP_CANNON:
                case NPC_SKYBREAKER_DECKHAND:
                case NPC_HIGH_CAPTAIN_JUSTIN_BARTLETT:
                    if (GetTeamIdInInstance() == TEAM_HORDE)
                        return 0;
                    break;
                case NPC_ZAFOD_BOOMBOX:
                    if (GameObjectTemplate const* go = sObjectMgr->GetGameObjectTemplate(GO_THE_SKYBREAKER_A))
                        if ((GetTeamIdInInstance() == TEAM_ALLIANCE && data->mapid == go->moTransport.mapID) ||
                                (GetTeamIdInInstance() == TEAM_HORDE && data->mapid != go->moTransport.mapID))
                            return entry;
                    return 0;
                case NPC_IGB_MURADIN_BRONZEBEARD:
                    if ((GetTeamIdInInstance() == TEAM_ALLIANCE && data->posX > 10.0f) ||
                            (GetTeamIdInInstance() == TEAM_HORDE && data->posX < 10.0f))
                        return entry;
                    return 0;
                case NPC_SPIRE_FROSTWYRM:
                    if ((GetTeamIdInInstance() == TEAM_ALLIANCE && data->posY < 2200.0f) || (GetTeamIdInInstance() == TEAM_HORDE && data->posY > 2200.0f))
                        return 0;
                    break;
            }

            return entry;
        }

        uint32 GetGameObjectEntry(ObjectGuid::LowType /*guidLow*/, uint32 entry) override
        {
            switch (entry)
            {
                case GO_GUNSHIP_ARMORY_H_10N:
                case GO_GUNSHIP_ARMORY_H_25N:
                case GO_GUNSHIP_ARMORY_H_10H:
                case GO_GUNSHIP_ARMORY_H_25H:
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        return 0;
                    break;
                case GO_GUNSHIP_ARMORY_A_10N:
                case GO_GUNSHIP_ARMORY_A_25N:
                case GO_GUNSHIP_ARMORY_A_10H:
                case GO_GUNSHIP_ARMORY_A_25H:
                    if (GetTeamIdInInstance() == TEAM_HORDE)
                        return 0;
                    break;
            }

            return entry;
        }

        void OnUnitDeath(Unit* unit) override
        {
            Creature* creature = unit->ToCreature();
            if (!creature)
                return;

            // fighting npcs in Rampart of Skulls
            std::string name1("Skybreaker ");
            std::string name2("Kor'kron ");
            if (!creature->GetTransport() && creature->GetPositionZ() <= 205.0f && creature->GetExactDist2d(-439.0f, 2210.0f) <= 150.0f && (creature->GetEntry() == 37544 || creature->GetEntry() == 37545 || creature->GetName().compare(0, name1.length(), name1) == 0 || creature->GetName().compare(0, name2.length(), name2) == 0))
                if (!creature->GetLootRecipient())
                    creature->m_Events.AddEvent(new RespawnEvent(*creature), creature->m_Events.CalculateTime(3000));

            switch (creature->GetEntry())
            {
                case NPC_YMIRJAR_BATTLE_MAIDEN:
                case NPC_YMIRJAR_DEATHBRINGER:
                case NPC_YMIRJAR_FROSTBINDER:
                case NPC_YMIRJAR_HUNTRESS:
                case NPC_YMIRJAR_WARLORD:
                    if (Creature* crok = instance->GetCreature(CrokScourgebaneGUID))
                        crok->AI()->SetGUID(creature->GetGUID(), ACTION_VRYKUL_DEATH);
                    break;
                case NPC_FROSTWING_WHELP:
                    if (FrostwyrmGUIDs.empty())
                        return;

                    if (creature->AI()->GetData(1/*DATA_FROSTWYRM_OWNER*/) == DATA_SPINESTALKER)
                    {
                        SpinestalkerTrash.erase(creature->GetSpawnId());
                        if (SpinestalkerTrash.empty())
                            if (Creature* spinestalk = instance->GetCreature(SpinestalkerGUID))
                                spinestalk->AI()->DoAction(ACTION_START_FROSTWYRM);
                    }
                    else
                    {
                        RimefangTrash.erase(creature->GetSpawnId());
                        if (RimefangTrash.empty())
                            if (Creature* spinestalk = instance->GetCreature(RimefangGUID))
                                spinestalk->AI()->DoAction(ACTION_START_FROSTWYRM);
                    }
                    break;
                case NPC_RIMEFANG:
                case NPC_SPINESTALKER:
                    {
                        if (GetData(DATA_HAS_LIMITED_ATTEMPTS) && !HeroicAttempts)
                            return;

                        if (GetBossState(DATA_SINDRAGOSA) == DONE)
                            return;

                        FrostwyrmGUIDs.erase(creature->GetSpawnId());
                        if (FrostwyrmGUIDs.empty())
                        {
                            if (Creature* boss = instance->SummonCreature(NPC_SINDRAGOSA, SindragosaSpawnPos))
                                boss->AI()->DoAction(ACTION_START_FROSTWYRM);
                        }
                        break;
                    }
                case NPC_DEATHSPEAKER_SERVANT:
                    if (Creature* c = unit->SummonCreature(WORLD_TRIGGER, *unit, TEMPSUMMON_TIMED_DESPAWN, 10000))
                    {
                        c->CastSpell(c, VOID_ZONE_VISUAL, true);
                        unit->SummonCreature(NPC_RISEN_DEATHSPEAKER_SERVANT, *unit, TEMPSUMMON_MANUAL_DESPAWN);
                        unit->ToCreature()->DespawnOrUnsummon(3000);
                    }
                    break;
                default:
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_SPIRIT_ALARM_1:
                case GO_SPIRIT_ALARM_2:
                case GO_SPIRIT_ALARM_3:
                case GO_SPIRIT_ALARM_4:
                    SetPositionTraps(go);
                    break;
                case GO_GEIST_ALARM_1:
                case GO_GEIST_ALARM_2:
                    go->SetPosition(go->GetPositionX() + urand(0, 2) * 20.0f * (go->GetEntry() == GO_GEIST_ALARM_1 ? -1.0f : 1.0f), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation());
                    break;
                case GO_DOODAD_ICECROWN_ICEWALL02:
                case GO_ICEWALL:
                case GO_LORD_MARROWGAR_S_ENTRANCE:
                case GO_ORATORY_OF_THE_DAMNED_ENTRANCE:
                case GO_ORANGE_PLAGUE_MONSTER_ENTRANCE:
                case GO_GREEN_PLAGUE_MONSTER_ENTRANCE:
                case GO_CRIMSON_HALL_DOOR:
                case GO_BLOOD_ELF_COUNCIL_DOOR:
                case GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT:
                case GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01:
                case GO_DOODAD_ICECROWN_GRATE_01:
                case GO_GREEN_DRAGON_BOSS_ENTRANCE:
                case GO_GREEN_DRAGON_BOSS_EXIT:
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02:
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03:
                case GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR:
                case GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR:
                case GO_ICE_WALL:
                case GO_SINDRAGOSA_ENTRANCE_DOOR:
                    AddDoor(go);
                    break;
                case GO_SCIENTIST_ENTRANCE:
                    PutricideEnteranceDoorGUID = go->GetGUID();
                    HandleGameObject(PutricideEnteranceDoorGUID, PutricideEventProgress & PUTRICIDE_EVENT_FLAG_TRAP_FINISHED, go);
                    break;
                // these 2 gates are functional only on 25man modes
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01:
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04:
                    if (instance->Is25ManRaid())
                        AddDoor(go);
                    break;
                case GO_LADY_DEATHWHISPER_ELEVATOR:
                    LadyDeathwisperElevatorGUID = go->GetGUID();
                    break;
                case GO_THE_SKYBREAKER_H:
                case GO_ORGRIMS_HAMMER_A:
                    EnemyGunshipGUID = go->GetGUID();
                    break;
                case GO_GUNSHIP_ARMORY_H_10N:
                case GO_GUNSHIP_ARMORY_H_25N:
                case GO_GUNSHIP_ARMORY_H_10H:
                case GO_GUNSHIP_ARMORY_H_25H:
                case GO_GUNSHIP_ARMORY_A_10N:
                case GO_GUNSHIP_ARMORY_A_25N:
                case GO_GUNSHIP_ARMORY_A_10H:
                case GO_GUNSHIP_ARMORY_A_25H:
                    GunshipArmoryGUID = go->GetGUID();
                    break;
                case GO_SAURFANG_S_DOOR:
                    DeathbringerSaurfangDoorGUID = go->GetGUID();
                    AddDoor(go);
                    break;
                case GO_DEATHBRINGER_S_CACHE_10N:
                case GO_DEATHBRINGER_S_CACHE_25N:
                case GO_DEATHBRINGER_S_CACHE_10H:
                case GO_DEATHBRINGER_S_CACHE_25H:
                    DeathbringersCacheGUID = go->GetGUID();
                    break;
                case GO_SCOURGE_TRANSPORTER_SAURFANG:
                    SaurfangTeleportGUID = go->GetGUID();
                    break;
                case GO_PLAGUE_SIGIL:
                    PlagueSigilGUID = go->GetGUID();
                    if (GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE)
                        HandleGameObject(PlagueSigilGUID, false, go);
                    break;
                case GO_BLOODWING_SIGIL:
                    BloodwingSigilGUID = go->GetGUID();
                    if (GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE)
                        HandleGameObject(BloodwingSigilGUID, false, go);
                    break;
                case GO_SIGIL_OF_THE_FROSTWING:
                    FrostwingSigilGUID = go->GetGUID();
                    if (GetBossState(DATA_SINDRAGOSA) == DONE)
                        HandleGameObject(FrostwingSigilGUID, false, go);
                    break;
                case GO_SCIENTIST_AIRLOCK_DOOR_COLLISION:
                    PutricideCollisionGUID = go->GetGUID();
                    HandleGameObject(PutricideCollisionGUID, ((PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE) && (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE)), go);
                    break;
                case GO_SCIENTIST_AIRLOCK_DOOR_ORANGE:
                    PutricideGateGUIDs[0] = go->GetGUID();
                    if ((PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE) && (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE))
                        go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    else
                        HandleGameObject(PutricideGateGUIDs[0], !(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE), go);
                    break;
                case GO_SCIENTIST_AIRLOCK_DOOR_GREEN:
                    PutricideGateGUIDs[1] = go->GetGUID();
                    if ((PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE) && (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE))
                        go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    else
                        HandleGameObject(PutricideGateGUIDs[1], !(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE), go);
                    break;
                case GO_DOODAD_ICECROWN_ORANGETUBES02:
                    PutricidePipeGUIDs[0] = go->GetGUID();
                    if (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE)
                        HandleGameObject(PutricidePipeGUIDs[0], true, go);
                    break;
                case GO_DOODAD_ICECROWN_GREENTUBES02:
                    PutricidePipeGUIDs[1] = go->GetGUID();
                    if (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE)
                        HandleGameObject(PutricidePipeGUIDs[1], true, go);
                    break;
                case GO_GAS_RELEASE_VALVE:
                    GasReleaseValveGUID = go->GetGUID();
                    if (GetBossState(DATA_FESTERGUT) != DONE)
                        go->SetGameObjectFlag(GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                    break;
                case GO_OOZE_RELEASE_VALVE:
                    OozeReleaseValveGUID = go->GetGUID();
                    if (GetBossState(DATA_ROTFACE) != DONE)
                        go->SetGameObjectFlag(GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                    break;
                case GO_DRINK_ME:
                    PutricideTableGUID = go->GetGUID();
                    break;
                case GO_CACHE_OF_THE_DREAMWALKER_10N:
                case GO_CACHE_OF_THE_DREAMWALKER_25N:
                case GO_CACHE_OF_THE_DREAMWALKER_10H:
                case GO_CACHE_OF_THE_DREAMWALKER_25H:
                    if (Creature* valithria = instance->GetCreature(ValithriaDreamwalkerGUID))
                        go->SetLootRecipient(valithria);
                    go->RemoveGameObjectFlag(GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                    break;
                case GO_SCOURGE_TRANSPORTER_LK:
                    TheLichKingTeleportGUID = go->GetGUID();
                    if (GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && GetBossState(DATA_SINDRAGOSA) == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_ARTHAS_PLATFORM:
                    // this enables movement at The Frozen Throne, when printed this value is 0.000000f
                    // however, when represented as integer client will accept only this value
                    go->SetUInt32Value(GAMEOBJECT_PARENTROTATION, 5535469);
                    ArthasPlatformGUID = go->GetGUID();
                    break;
                case GO_ARTHAS_PRECIPICE:
                    go->SetUInt32Value(GAMEOBJECT_PARENTROTATION, 4178312);
                    ArthasPrecipiceGUID = go->GetGUID();
                    break;
                case GO_DOODAD_ICECROWN_THRONEFROSTYEDGE01:
                    FrozenThroneEdgeGUID = go->GetGUID();
                    break;
                case GO_DOODAD_ICECROWN_THRONEFROSTYWIND01:
                    FrozenThroneWindGUID = go->GetGUID();
                    break;
                case GO_DOODAD_ICECROWN_SNOWEDGEWARNING01:
                    FrozenThroneWarningGUID = go->GetGUID();
                    break;
                case GO_FROZEN_LAVAMAN:
                    FrozenBolvarGUID = go->GetGUID();
                    if (GetBossState(DATA_THE_LICH_KING) == DONE)
                        go->SetRespawnTime(7 * DAY);
                    break;
                case GO_LAVAMAN_PILLARS_CHAINED:
                    PillarsChainedGUID = go->GetGUID();
                    if (GetBossState(DATA_THE_LICH_KING) == DONE)
                        go->SetRespawnTime(7 * DAY);
                    break;
                case GO_LAVAMAN_PILLARS_UNCHAINED:
                    PillarsUnchainedGUID = go->GetGUID();
                    if (GetBossState(DATA_THE_LICH_KING) == DONE)
                        go->SetRespawnTime(7 * DAY);
                    break;
                case GO_SCOURGE_TRANSPORTER_FIRST:
                    AddDoor(go);
                    ScourgeTransporterFirstGUID = go->GetGUID();
                    if (GetBossState(DATA_LORD_MARROWGAR) == DONE)
                        go->RemoveGameObjectFlag(GO_FLAG_NOT_SELECTABLE);
                    break;
                default:
                    break;
            }
        }

        void OnGameObjectRemove(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_DOODAD_ICECROWN_ICEWALL02:
                case GO_ICEWALL:
                case GO_LORD_MARROWGAR_S_ENTRANCE:
                case GO_ORATORY_OF_THE_DAMNED_ENTRANCE:
                case GO_SAURFANG_S_DOOR:
                case GO_ORANGE_PLAGUE_MONSTER_ENTRANCE:
                case GO_GREEN_PLAGUE_MONSTER_ENTRANCE:
                case GO_SCIENTIST_ENTRANCE:
                case GO_CRIMSON_HALL_DOOR:
                case GO_BLOOD_ELF_COUNCIL_DOOR:
                case GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT:
                case GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01:
                case GO_DOODAD_ICECROWN_GRATE_01:
                case GO_GREEN_DRAGON_BOSS_ENTRANCE:
                case GO_GREEN_DRAGON_BOSS_EXIT:
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01:
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02:
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03:
                case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04:
                //case GO_SINDRAGOSA_ENTRANCE_DOOR:
                case GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR:
                case GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR:
                case GO_ICE_WALL:
                case GO_SCOURGE_TRANSPORTER_FIRST:
                    RemoveDoor(go);
                    break;
                case GO_THE_SKYBREAKER_A:
                case GO_ORGRIMS_HAMMER_H:
                    GunshipGUID.Clear();
                    break;
                default:
                    break;
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_BUFF_AVAILABLE:
                    return (IsBuffAvailable ? 1 : 0);
                case DATA_WEEKLY_QUEST_ID:
                    return WeeklyQuestId10;
                case DATA_PUTRICIDE_TRAP_STATE:
                    if (!(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE) || !(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE))
                        return TO_BE_DECIDED;
                    if (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_TRAP_INPROGRESS)
                        return IN_PROGRESS;
                    if (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_TRAP_FINISHED)
                        return DONE;
                    return NOT_STARTED;
                case DATA_HAS_LIMITED_ATTEMPTS:
                    return (instance->IsHeroic() ? 1 : 0);
                case DATA_LK_HC_AVAILABLE:
                    return (LichKingHeroicAvailable ? 1 : 0);
                case DATA_SINDRAGOSA_FROSTWYRMS:
                    return FrostwyrmGUIDs.size();
                case DATA_SPINESTALKER:
                    return SpinestalkerTrash.size();
                case DATA_RIMEFANG:
                    return RimefangTrash.size();
                case DATA_COLDFLAME_JETS:
                    return ColdflameJetsState;
                case DATA_TEAMID_IN_INSTANCE:
                    return GetTeamIdInInstance();
                case DATA_BLOOD_QUICKENING_STATE:
                    return BloodQuickeningState;
                case DATA_HEROIC_ATTEMPTS:
                    return HeroicAttempts;
                case DATA_SINDRAGOSA_INTRO:
                    return (IsSindragosaIntroDone ? 1 : 0);
                default:
                    break;
            }

            return 0;
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_LADY_DEATHWHISPER:
                    return LadyDeathwhisperGUID;
                case DATA_ICECROWN_GUNSHIP_BATTLE:
                    return GunshipGUID;
                case DATA_ENEMY_GUNSHIP:
                    return EnemyGunshipGUID;
                case DATA_DEATHBRINGER_SAURFANG:
                    return DeathbringerSaurfangGUID;
                case DATA_SAURFANG_EVENT_NPC:
                    return DeathbringerSaurfangEventGUID;
                case GO_SAURFANG_S_DOOR:
                    return DeathbringerSaurfangDoorGUID;
                case GO_SCOURGE_TRANSPORTER_SAURFANG:
                    return SaurfangTeleportGUID;
                case DATA_FESTERGUT:
                    return FestergutGUID;
                case DATA_ROTFACE:
                    return RotfaceGUID;
                case DATA_PROFESSOR_PUTRICIDE:
                    return ProfessorPutricideGUID;
                case DATA_PUTRICIDE_TABLE:
                    return PutricideTableGUID;
                case DATA_PRINCE_KELESETH_GUID:
                    return BloodCouncilGUIDs[0];
                case DATA_PRINCE_TALDARAM_GUID:
                    return BloodCouncilGUIDs[1];
                case DATA_PRINCE_VALANAR_GUID:
                    return BloodCouncilGUIDs[2];
                case DATA_BLOOD_PRINCES_CONTROL:
                    return BloodCouncilControllerGUID;
                case DATA_BLOOD_QUEEN_LANA_THEL:
                    return BloodQueenLanaThelGUID;
                case DATA_CROK_SCOURGEBANE:
                    return CrokScourgebaneGUID;
                case DATA_CAPTAIN_ARNATH:
                case DATA_CAPTAIN_BRANDON:
                case DATA_CAPTAIN_GRONDEL:
                case DATA_CAPTAIN_RUPERT:
                    return CrokCaptainGUIDs[type - DATA_CAPTAIN_ARNATH];
                case DATA_SISTER_SVALNA:
                    return SisterSvalnaGUID;
                case DATA_VALITHRIA_DREAMWALKER:
                    return ValithriaDreamwalkerGUID;
                case DATA_VALITHRIA_LICH_KING:
                    return ValithriaLichKingGUID;
                case DATA_VALITHRIA_TRIGGER:
                    return ValithriaTriggerGUID;
                case NPC_SINDRAGOSA_GAUNTLET:
                    return SindragosaGauntletGUID;
                case NPC_PUTRICADES_TRAP:
                    return PutricadeTrapGUID;
                case DATA_SINDRAGOSA:
                    return SindragosaGUID;
                case DATA_SPINESTALKER:
                    return SpinestalkerGUID;
                case DATA_RIMEFANG:
                    return RimefangGUID;
                case DATA_THE_LICH_KING:
                    return TheLichKingGUID;
                case DATA_HIGHLORD_TIRION_FORDRING:
                    return HighlordTirionFordringGUID;
                case DATA_ARTHAS_PLATFORM:
                    return ArthasPlatformGUID;
                case DATA_TERENAS_MENETHIL:
                    return TerenasMenethilGUID;
                default:
                    break;
            }

            return ObjectGuid::Empty;
        }

        void HandleDropAttempt(bool drop = true)
        {
            if (!GetData(DATA_HAS_LIMITED_ATTEMPTS))
                return;
            if (drop && HeroicAttempts)
            {
                --HeroicAttempts;
                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_ATTEMPTS_REMAINING, HeroicAttempts);
                SaveToDB();
            }
            if (HeroicAttempts)
                return;
            if (GetBossState(DATA_ROTFACE) == DONE && GetBossState(DATA_FESTERGUT) == DONE)
                if (Creature* professor = instance->GetCreature(ProfessorPutricideGUID))
                    if (professor->IsAlive())
                        professor->SetVisible(false);
            if (Creature* bq = instance->GetCreature(BloodQueenLanaThelGUID))
                if (bq->IsAlive())
                    bq->SetVisible(false);
            if (Creature* sindra = instance->GetCreature(SindragosaGUID))
                if (sindra->IsAlive())
                    sindra->SetVisible(false);
            if (Creature* theLichKing = instance->GetCreature(TheLichKingGUID))
                if (theLichKing->IsAlive())
                    theLichKing->SetVisible(false);
        }

        void RemoveBackPack()
        {
            for (auto const& itr : instance->GetPlayers())
                if (Player* _player = itr.GetSource())
                    _player->DestroyItemCount(ITEM_GOBLIN_ROCKET_PACK, _player->GetItemCount(ITEM_GOBLIN_ROCKET_PACK), true);
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (type)
            {
                case DATA_LORD_MARROWGAR:
                    if (state == DONE)
                    {
                        WeeklyQuestId10 = RAND(QUEST_BLOOD_QUICKENING_10, QUEST_RESIDUE_RENDEZVOUS_10, QUEST_RESPITE_FOR_A_TORMENTED_SOUL_10, QUEST_DEPROGRAMMING_10, QUEST_SECURING_THE_RAMPARTS_10);
                        SetData(DATA_WEEKLY_QUEST_ID, 0); // show required hidden npcs
                        if (GameObject* transporter = instance->GetGameObject(ScourgeTransporterFirstGUID))
                            transporter->RemoveGameObjectFlag(GO_FLAG_NOT_SELECTABLE);
                        SaveToDB();
                    }
                    break;
                case DATA_LADY_DEATHWHISPER:
                    if (state == DONE)
                        SpawnGunship();
                    break;
                case DATA_ICECROWN_GUNSHIP_BATTLE:
                    if (state == DONE)
                    {
                        if (GameObject* loot = instance->GetGameObject(GunshipArmoryGUID))
                        {
                            loot->SetLootRecipient(instance);
                            loot->RemoveGameObjectFlag(GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                        }
                    }
                    else if (state == FAIL)
                        Events.ScheduleEvent(EVENT_RESPAWN_GUNSHIP, 30s);
                    break;
                case DATA_DEATHBRINGER_SAURFANG:
                    switch (state)
                    {
                        case DONE:
                            if (GameObject* loot = instance->GetGameObject(DeathbringersCacheGUID))
                            {
                                if (Creature* deathbringer = instance->GetCreature(DeathbringerSaurfangGUID))
                                    loot->SetLootRecipient(deathbringer);
                                loot->RemoveGameObjectFlag(GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                            }
                            [[fallthrough]];
                        case NOT_STARTED:
                            if (GameObject* teleporter = instance->GetGameObject(SaurfangTeleportGUID))
                            {
                                HandleGameObject(SaurfangTeleportGUID, true, teleporter);
                                teleporter->RemoveGameObjectFlag(GO_FLAG_IN_USE);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case DATA_FESTERGUT:
                    if (state == DONE)
                    {
                        if (GameObject* go = instance->GetGameObject(GasReleaseValveGUID))
                            go->RemoveGameObjectFlag(GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                        if (GetBossState(DATA_ROTFACE) == DONE)
                            HandleDropAttempt(false);
                    }
                    break;
                case DATA_ROTFACE:
                    if (state == DONE)
                    {
                        if (GameObject* go = instance->GetGameObject(OozeReleaseValveGUID))
                            go->RemoveGameObjectFlag(GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                        if (GetBossState(DATA_FESTERGUT) == DONE)
                            HandleDropAttempt(false);
                    }
                    break;
                case DATA_PROFESSOR_PUTRICIDE:
                    HandleGameObject(PutricideEnteranceDoorGUID, (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_TRAP_FINISHED) && state != IN_PROGRESS);
                    HandleGameObject(PlagueSigilGUID, state != DONE);
                    if (state == DONE)
                        CheckLichKingAvailability();
                    else if (state == FAIL)
                        HandleDropAttempt();
                    if (state == DONE && !instance->IsHeroic() && LichKingHeroicAvailable)
                    {
                        LichKingHeroicAvailable = false;
                        SaveToDB();
                    }
                    break;
                case DATA_BLOOD_QUEEN_LANA_THEL:
                    HandleGameObject(BloodwingSigilGUID, state != DONE);
                    if (state == DONE)
                        CheckLichKingAvailability();
                    else if (state == FAIL)
                        HandleDropAttempt();
                    if (state == DONE && !instance->IsHeroic() && LichKingHeroicAvailable)
                    {
                        LichKingHeroicAvailable = false;
                        SaveToDB();
                    }
                    break;
                case DATA_VALITHRIA_DREAMWALKER:
                    if (state == DONE)
                        SetData(DATA_WEEKLY_QUEST_ID, GetData(DATA_WEEKLY_QUEST_ID)); // will show weekly quest npc if necessary
                    break;
                case DATA_SINDRAGOSA:
                    HandleGameObject(FrostwingSigilGUID, state != DONE);
                    if (state == DONE)
                        CheckLichKingAvailability();
                    else if (state == FAIL)
                    {
                        IsSindragosaIntroDone = true;
                        HandleDropAttempt();
                    }
                    if (state == DONE && !instance->IsHeroic() && LichKingHeroicAvailable)
                    {
                        LichKingHeroicAvailable = false;
                        SaveToDB();
                    }
                    break;
                case DATA_THE_LICH_KING:
                    {
                        // dramatically increase visibility range during fight to seeing frostmourne room
                        instance->SetVisibilityRange(state == IN_PROGRESS ? 500.0f : 200.0f);

                        if (state == FAIL)
                        {
                            Events.CancelEvent(EVENT_QUAKE_SHATTER);
                            Events.CancelEvent(EVENT_REBUILD_PLATFORM);

                            HandleDropAttempt();
                        }

                        if (state == DONE)
                        {
                            if (GameObject* bolvar = instance->GetGameObject(FrozenBolvarGUID))
                                bolvar->SetRespawnTime(7 * DAY);
                            if (GameObject* pillars = instance->GetGameObject(PillarsChainedGUID))
                                pillars->SetRespawnTime(7 * DAY);
                            if (GameObject* pillars = instance->GetGameObject(PillarsUnchainedGUID))
                                pillars->SetRespawnTime(7 * DAY);

                            instance->SummonCreature(NPC_LADY_JAINA_PROUDMOORE_QUEST, JainaSpawnPos);
                            instance->SummonCreature(NPC_MURADIN_BRONZEBEARD_QUEST, MuradinSpawnPos);
                            instance->SummonCreature(NPC_UTHER_THE_LIGHTBRINGER_QUEST, UtherSpawnPos);
                            instance->SummonCreature(NPC_LADY_SYLVANAS_WINDRUNNER_QUEST, SylvanasSpawnPos);
                        }
                        break;
                    }
                default:
                    break;
            }

            return true;
        }

        void SpawnGunship()
        {
            if (!GunshipGUID && instance->HavePlayers())
            {
                SetBossState(DATA_ICECROWN_GUNSHIP_BATTLE, NOT_STARTED);
                uint32 gunshipEntry = GetTeamIdInInstance() == TEAM_HORDE ? GO_ORGRIMS_HAMMER_H : GO_THE_SKYBREAKER_A;
                if (MotionTransport* gunship = sTransportMgr->CreateTransport(gunshipEntry, 0, instance))
                {
                    GunshipGUID = gunship->GetGUID();
                    gunship->setActive(false);
                }
            }
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_BUFF_AVAILABLE:
                    IsBuffAvailable = !!data;
                    if (!IsBuffAvailable)
                    {
                        instance->DoForAllPlayers([&](Player* player)
                        {
                            player->UpdateAreaDependentAuras(player->GetAreaId());
                            for (Unit::ControlSet::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                            {
                                Unit::AuraMap& am = (*itr)->GetOwnedAuras();
                                for (Unit::AuraMap::iterator itra = am.begin(); itra != am.end();)
                                    switch (itra->second->GetId())
                                    {
                                        // Hellscream's Warsong
                                        case 73816:
                                        case 73818:
                                        case 73819:
                                        case 73820:
                                        case 73821:
                                        case 73822:
                                            // Strength of Wrynn
                                        case 73762:
                                        case 73824:
                                        case 73825:
                                        case 73826:
                                        case 73827:
                                        case 73828:
                                            (*itr)->RemoveOwnedAura(itra);
                                            break;
                                        default:
                                            ++itra;
                                            break;
                                    }
                            }
                        });
                    }
                    SaveToDB();
                    break;
                case DATA_WEEKLY_QUEST_ID:
                    for (uint8 i = 0; i < WeeklyNPCs; ++i)
                        if (WeeklyQuestData[i].questId[0] == WeeklyQuestId10 && (WeeklyQuestData[i].creatureEntry != NPC_VALITHRIA_DREAMWALKER_QUEST || GetBossState(DATA_VALITHRIA_DREAMWALKER) == DONE) /*appears after killing valithria*/)
                            if (WeeklyQuestNpcGUID[i])
                                if (Creature* c = instance->GetCreature(WeeklyQuestNpcGUID[i]))
                                    c->SetVisible(true);
                    break;
                case DATA_PUTRICIDE_TRAP_STATE:
                    if (data == NOT_STARTED)
                    {
                        PutricideEventProgress &= ~PUTRICIDE_EVENT_FLAG_TRAP_INPROGRESS;
                        HandleGameObject(PutricideCollisionGUID, ((PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE) && (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE)));
                        if ((PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE) && (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE))
                        {
                            for (uint8 i = 0; i < 2; ++i)
                                if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[i]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        }
                        else
                        {
                            HandleGameObject(PutricideGateGUIDs[0], !(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE));
                            HandleGameObject(PutricideGateGUIDs[1], !(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE));
                        }
                        SaveToDB();
                    }
                    else if (data == IN_PROGRESS)
                    {
                        PutricideEventProgress |= PUTRICIDE_EVENT_FLAG_TRAP_INPROGRESS;
                        HandleGameObject(PutricideCollisionGUID, false);
                        HandleGameObject(PutricideGateGUIDs[0], false);
                        HandleGameObject(PutricideGateGUIDs[1], false);
                        SaveToDB();
                    }
                    else if (data == DONE)
                    {
                        PutricideEventProgress &= ~PUTRICIDE_EVENT_FLAG_TRAP_INPROGRESS;
                        PutricideEventProgress |= PUTRICIDE_EVENT_FLAG_TRAP_FINISHED;
                        HandleGameObject(PutricideEnteranceDoorGUID, true);
                        HandleGameObject(PutricideCollisionGUID, ((PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE) && (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE)));
                        if ((PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE) && (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE))
                        {
                            for (uint8 i = 0; i < 2; ++i)
                                if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[i]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        }
                        else
                        {
                            HandleGameObject(PutricideGateGUIDs[0], !(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE));
                            HandleGameObject(PutricideGateGUIDs[1], !(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE));
                        }
                        SaveToDB();
                    }
                    return;
                case DATA_BONED_ACHIEVEMENT:
                    IsBonedEligible = !!data;
                    break;
                case DATA_OOZE_DANCE_ACHIEVEMENT:
                    IsOozeDanceEligible = !!data;
                    break;
                case DATA_NAUSEA_ACHIEVEMENT:
                    IsNauseaEligible = !!data;
                    break;
                case DATA_ORB_WHISPERER_ACHIEVEMENT:
                    IsOrbWhispererEligible = !!data;
                    break;
                case DATA_SINDRAGOSA_FROSTWYRMS:
                    FrostwyrmGUIDs.insert(data);
                    break;
                case DATA_SPINESTALKER:
                    SpinestalkerTrash.insert(data);
                    break;
                case DATA_RIMEFANG:
                    RimefangTrash.insert(data);
                    break;
                case DATA_COLDFLAME_JETS:
                    ColdflameJetsState = data;
                    if (ColdflameJetsState == DONE)
                        SaveToDB();
                    break;
                case DATA_BLOOD_QUICKENING_STATE:
                    {
                        if (data == IN_PROGRESS && BloodQuickeningState != NOT_STARTED)
                            break;
                        if (BloodQuickeningState == data)
                            break;
                        if (WeeklyQuestId10 != QUEST_BLOOD_QUICKENING_10)
                            break;

                        switch (data)
                        {
                            case IN_PROGRESS:
                                Events.ScheduleEvent(EVENT_UPDATE_EXECUTION_TIME, 1min);
                                BloodQuickeningMinutes = 30;
                                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_SHOW_TIMER, 1);
                                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_EXECUTION_TIME, BloodQuickeningMinutes);
                                break;
                            case DONE:
                                Events.CancelEvent(EVENT_UPDATE_EXECUTION_TIME);
                                BloodQuickeningMinutes = 0;
                                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_SHOW_TIMER, 0);
                                break;
                            default:
                                break;
                        }

                        BloodQuickeningState = data;
                        SaveToDB();
                        break;
                    }
                case DATA_BPC_TRASH_DIED:
                    {
                        if (++BloodPrinceTrashCount >= 4)
                        {
                            SetBossState(DATA_BLOOD_PRINCE_TRASH, NOT_STARTED);
                            SetBossState(DATA_BLOOD_PRINCE_TRASH, DONE);
                        }
                        SaveToDB();
                        break;
                    }
                default:
                    break;
            }
        }

        bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/, uint32 /*miscvalue1*/) override
        {
            switch (criteria_id)
            {
                case CRITERIA_BONED_10N:
                case CRITERIA_BONED_25N:
                case CRITERIA_BONED_10H:
                case CRITERIA_BONED_25H:
                    return IsBonedEligible;
                case CRITERIA_DANCES_WITH_OOZES_10N:
                case CRITERIA_DANCES_WITH_OOZES_25N:
                case CRITERIA_DANCES_WITH_OOZES_10H:
                case CRITERIA_DANCES_WITH_OOZES_25H:
                    return IsOozeDanceEligible;
                case CRITERIA_NAUSEA_10N:
                case CRITERIA_NAUSEA_25N:
                case CRITERIA_NAUSEA_10H:
                case CRITERIA_NAUSEA_25H:
                    return IsNauseaEligible;
                case CRITERIA_ORB_WHISPERER_10N:
                case CRITERIA_ORB_WHISPERER_25N:
                case CRITERIA_ORB_WHISPERER_10H:
                case CRITERIA_ORB_WHISPERER_25H:
                    return IsOrbWhispererEligible;
                // Only one criteria for both modes, need to do it like this
                case CRITERIA_KILL_LANA_THEL_10M:
                    return instance->ToInstanceMap()->GetMaxPlayers() == 10;
                case CRITERIA_KILL_LANA_THEL_25M:
                    return instance->ToInstanceMap()->GetMaxPlayers() == 25;
                default:
                    break;
            }

            return false;
        }

        bool CheckRequiredBosses(uint32 bossId, Player const* player) const override
        {
            if (player && player->GetSession() && player->GetSession()->GetSecurity() >= SEC_MODERATOR)
            {
                return true;
            }

            switch (bossId)
            {
                case DATA_THE_LICH_KING:
                    if (!CheckPlagueworks(bossId))
                        return false;
                    if (!CheckCrimsonHalls(bossId))
                        return false;
                    if (!CheckFrostwingHalls(bossId))
                        return false;
                    break;
                case DATA_SINDRAGOSA:
                case DATA_VALITHRIA_DREAMWALKER:
                    if (!CheckFrostwingHalls(bossId))
                        return false;
                    break;
                case DATA_BLOOD_QUEEN_LANA_THEL:
                case DATA_BLOOD_PRINCE_COUNCIL:
                    if (!CheckCrimsonHalls(bossId))
                        return false;
                    break;
                case DATA_FESTERGUT:
                case DATA_ROTFACE:
                case DATA_PROFESSOR_PUTRICIDE:
                    if (!CheckPlagueworks(bossId))
                        return false;
                    break;
                default:
                    break;
            }

            if (!CheckLowerSpire(bossId))
                return false;

            return true;
        }

        bool CheckPlagueworks(uint32 bossId) const
        {
            switch (bossId)
            {
                case DATA_THE_LICH_KING:
                    if (GetBossState(DATA_PROFESSOR_PUTRICIDE) != DONE)
                    {
                        return false;
                    }
                    [[fallthrough]];
                case DATA_PROFESSOR_PUTRICIDE:
                    if (GetBossState(DATA_FESTERGUT) != DONE || GetBossState(DATA_ROTFACE) != DONE)
                        return false;
                    break;
                default:
                    break;
            }

            return true;
        }

        bool CheckCrimsonHalls(uint32 bossId) const
        {
            switch (bossId)
            {
                case DATA_THE_LICH_KING:
                    if (GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) != DONE)
                    {
                        return false;
                    }
                    [[fallthrough]];
                case DATA_BLOOD_QUEEN_LANA_THEL:
                    if (GetBossState(DATA_BLOOD_PRINCE_COUNCIL) != DONE)
                        return false;
                    break;
                default:
                    break;
            }

            return true;
        }

        bool CheckFrostwingHalls(uint32 bossId) const
        {
            switch (bossId)
            {
                case DATA_THE_LICH_KING:
                    if (GetBossState(DATA_SINDRAGOSA) != DONE)
                    {
                        return false;
                    }
                    [[fallthrough]];
                case DATA_SINDRAGOSA:
                    if (GetBossState(DATA_VALITHRIA_DREAMWALKER) != DONE)
                    {
                        return false;
                    }
                    if (GetBossState(DATA_SINDRAGOSA_GAUNTLET) != DONE)
                    {
                        return false;
                    }
                    break;
                default:
                    break;
            }

            return true;
        }

        bool CheckLowerSpire(uint32 bossId) const
        {
            switch (bossId)
            {
                case DATA_THE_LICH_KING:
                case DATA_SINDRAGOSA:
                case DATA_BLOOD_QUEEN_LANA_THEL:
                case DATA_PROFESSOR_PUTRICIDE:
                case DATA_VALITHRIA_DREAMWALKER:
                case DATA_BLOOD_PRINCE_COUNCIL:
                case DATA_ROTFACE:
                case DATA_FESTERGUT:
                    if (GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
                    {
                        return false;
                    }
                    [[fallthrough]];
                case DATA_DEATHBRINGER_SAURFANG:
                    if (GetBossState(DATA_ICECROWN_GUNSHIP_BATTLE) != DONE)
                    {
                        return false;
                    }
                    [[fallthrough]];
                case DATA_ICECROWN_GUNSHIP_BATTLE:
                    if (GetBossState(DATA_LADY_DEATHWHISPER) != DONE)
                    {
                        return false;
                    }
                    [[fallthrough]];
                case DATA_LADY_DEATHWHISPER:
                    if (GetBossState(DATA_LORD_MARROWGAR) != DONE)
                    {
                        return false;
                    }
                    [[fallthrough]];
                case DATA_LORD_MARROWGAR:
                default:
                    break;
            }

            return true;
        }

        void CheckLichKingAvailability()
        {
            if (GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && GetBossState(DATA_SINDRAGOSA) == DONE)
            {
                if (GameObject* teleporter = instance->GetGameObject(TheLichKingTeleportGUID))
                {
                    teleporter->SetGoState(GO_STATE_ACTIVE);

                    std::list<Creature*> stalkers;
                    GetCreatureListWithEntryInGrid(stalkers, teleporter, NPC_INVISIBLE_STALKER, 100.0f);
                    if (stalkers.empty())
                        return;

                    stalkers.sort(Acore::ObjectDistanceOrderPred(teleporter));
                    stalkers.front()->CastSpell((Unit*)nullptr, SPELL_ARTHAS_TELEPORTER_CEREMONY, false);
                    stalkers.pop_front();
                    for (std::list<Creature*>::iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                        (*itr)->AI()->Reset();
                }
            }
        }

        void ReadSaveDataMore(std::istringstream& data) override
        {
            data >> HeroicAttempts;

            uint32 temp = 0;
            data >> temp;

            if (temp == IN_PROGRESS)
            {
                ColdflameJetsState = NOT_STARTED;
            }
            else
            {
                ColdflameJetsState = temp ? DONE : NOT_STARTED;
            }

            data >> BloodQuickeningState;
            data >> BloodQuickeningMinutes;

            if (BloodQuickeningState == IN_PROGRESS)
            {
                Events.ScheduleEvent(EVENT_UPDATE_EXECUTION_TIME, 1min);
                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_SHOW_TIMER, 1);
                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_EXECUTION_TIME, BloodQuickeningMinutes);
            }

            data >> WeeklyQuestId10;
            data >> PutricideEventProgress;
            PutricideEventProgress &= ~PUTRICIDE_EVENT_FLAG_TRAP_INPROGRESS;
            data >> LichKingHeroicAvailable;
            data >> BloodPrinceTrashCount;
            data >> IsBuffAvailable;
            data >> IsSindragosaIntroDone;
            SetData(DATA_BUFF_AVAILABLE, IsBuffAvailable);
        }

        void WriteSaveDataMore(std::ostringstream& data) override
        {
            data << HeroicAttempts << ' '
                << ColdflameJetsState << ' '
                << BloodQuickeningState << ' '
                << BloodQuickeningMinutes << ' '
                << WeeklyQuestId10 << ' '
                << PutricideEventProgress << ' '
                << uint32(LichKingHeroicAvailable ? 1 : 0) << ' '
                << BloodPrinceTrashCount << ' '
                << uint32(IsBuffAvailable ? 1 : 0) << ' '
                << uint32(IsSindragosaIntroDone ? 1 : 0);
        }

        void Update(uint32 diff) override
        {
            // Xinef: A Feast of Souls (24547) whispers
            if (LichKingRandomWhisperTimer <= diff)
            {
                LichKingRandomWhisperTimer = urand(100, 300) * IN_MILLISECONDS;
                Map::PlayerList const& players = instance->GetPlayers();
                if (!players.IsEmpty())
                    if (Player* player = players.begin()->GetSource())
                        if (player->GetQuestStatus(QUEST_A_FEAST_OF_SOULS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (Creature* theLichKing = instance->GetCreature(TheLichKingLhGUID))
                            {
                                theLichKing->AI()->Talk(SAY_SOULS_LICH_KING_RAND_WHISPER, player);
                            }
                        }
            }
            else
                LichKingRandomWhisperTimer -= diff;

            if (DarkwhisperElevatorTimer <= diff)
            {
                DarkwhisperElevatorTimer = 3000;
                if (GetBossState(DATA_LADY_DEATHWHISPER) == DONE)
                    if (GameObject* elevator = instance->GetGameObject(LadyDeathwisperElevatorGUID))
                        if (StaticTransport* trans = elevator->ToStaticTransport())
                        {
                            if (trans->GetGoState() == GO_STATE_READY && trans->GetPathProgress() == 0)
                                trans->SetGoState(GO_STATE_ACTIVE);
                            else if (trans->GetGoState() == GO_STATE_ACTIVE && trans->GetPathProgress() == trans->GetPauseTime())
                                trans->SetGoState(GO_STATE_READY);
                        }
            }
            else
                DarkwhisperElevatorTimer -= diff;

            if (Events.Empty())
                return;

            Events.Update(diff);

            while (uint32 eventId = Events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_UPDATE_EXECUTION_TIME:
                        {
                            --BloodQuickeningMinutes;
                            if (BloodQuickeningMinutes)
                            {
                                Events.ScheduleEvent(EVENT_UPDATE_EXECUTION_TIME, 1min);
                                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_SHOW_TIMER, 1);
                                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_EXECUTION_TIME, BloodQuickeningMinutes);
                            }
                            else
                            {
                                BloodQuickeningState = DONE;
                                DoUpdateWorldState(WORLD_STATE_ICECROWN_CITADEL_SHOW_TIMER, 0);
                                if (Creature* bq = instance->GetCreature(BloodQueenLanaThelGUID))
                                    bq->AI()->DoAction(ACTION_KILL_MINCHAR);
                            }
                            SaveToDB();
                            break;
                        }
                    case EVENT_QUAKE_SHATTER:
                        {
                            if (GameObject* platform = instance->GetGameObject(ArthasPlatformGUID))
                                platform->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
                            if (GameObject* edge = instance->GetGameObject(FrozenThroneEdgeGUID))
                                edge->SetGoState(GO_STATE_ACTIVE);
                            if (GameObject* wind = instance->GetGameObject(FrozenThroneWindGUID))
                                wind->SetGoState(GO_STATE_READY);
                            if (GameObject* warning = instance->GetGameObject(FrozenThroneWarningGUID))
                                warning->SetGoState(GO_STATE_READY);
                            if (Creature* theLichKing = instance->GetCreature(TheLichKingGUID))
                                theLichKing->AI()->DoAction(ACTION_RESTORE_LIGHT);
                            break;
                        }
                    case EVENT_REBUILD_PLATFORM:
                        if (GameObject* platform = instance->GetGameObject(ArthasPlatformGUID))
                            platform->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING, nullptr, true);
                        if (GameObject* edge = instance->GetGameObject(FrozenThroneEdgeGUID))
                            edge->SetGoState(GO_STATE_READY);
                        if (GameObject* wind = instance->GetGameObject(FrozenThroneWindGUID))
                            wind->SetGoState(GO_STATE_ACTIVE);
                        break;
                    case EVENT_RESPAWN_GUNSHIP:
                        SpawnGunship();
                        break;
                    case EVENT_RESPAWN_SINDRAGOSA:
                        if (!GetCreature(DATA_SINDRAGOSA))
                        {
                            if (Creature* sindragosa = instance->SummonCreature(NPC_SINDRAGOSA, SindragosaSpawnPos))
                            {
                                sindragosa->setActive(true);
                                sindragosa->SetDisableGravity(true);
                                sindragosa->GetMotionMaster()->MovePath(NPC_SINDRAGOSA * 10, true);

                                if (TempSummon* summon = sindragosa->ToTempSummon())
                                {
                                    summon->SetTempSummonType(TEMPSUMMON_DEAD_DESPAWN);
                                }
                            }
                        }
                        // Could happen more than once if more than one player enters before she respawns.
                        Events.CancelEvent(EVENT_RESPAWN_SINDRAGOSA);
                        break;
                    default:
                        break;
                }
            }
        }

        void ProcessEvent(WorldObject* source, uint32 eventId) override
        {
            switch (eventId)
            {
                case EVENT_ENEMY_GUNSHIP_DESPAWN:
                    if (GetBossState(DATA_ICECROWN_GUNSHIP_BATTLE) == DONE)
                    {
                        if (GameObject* go = source->ToGameObject())
                            if (MotionTransport* transport = go->ToMotionTransport())
                                transport->UnloadNonStaticPassengers();
                        source->AddObjectToRemoveList();
                    }
                    break;
                case EVENT_ENEMY_GUNSHIP_COMBAT:
                    if (Creature* captain = source->FindNearestCreature(GetTeamIdInInstance() == TEAM_HORDE ? NPC_IGB_HIGH_OVERLORD_SAURFANG : NPC_IGB_MURADIN_BRONZEBEARD, 200.0f))
                    {
                        captain->AI()->DoAction(ACTION_ENEMY_GUNSHIP_TALK);
                    }
                    [[fallthrough]];
                case EVENT_PLAYERS_GUNSHIP_SPAWN:
                case EVENT_PLAYERS_GUNSHIP_COMBAT:
                    if (GameObject* go = source->ToGameObject())
                        if (MotionTransport* transport = go->ToMotionTransport())
                            transport->EnableMovement(false);
                    break;
                case EVENT_PLAYERS_GUNSHIP_SAURFANG:
                    if (GameObject* go = source->ToGameObject())
                        if (MotionTransport* transport = go->ToMotionTransport())
                        {
                            transport->setActive(false);
                            transport->EnableMovement(false);
                            //After movement is stopped remove the backpack
                            RemoveBackPack();
                        }
                    if (Creature* captain = source->FindNearestCreature(GetTeamIdInInstance() == TEAM_HORDE ? NPC_IGB_HIGH_OVERLORD_SAURFANG : NPC_IGB_MURADIN_BRONZEBEARD, 200.0f))
                        captain->AI()->DoAction(ACTION_EXIT_SHIP);
                    break;

                case EVENT_QUAKE:
                    if (GameObject* warning = instance->GetGameObject(FrozenThroneWarningGUID))
                        warning->SetGoState(GO_STATE_ACTIVE);
                    Events.ScheduleEvent(EVENT_QUAKE_SHATTER, 5s);
                    break;
                case EVENT_SECOND_REMORSELESS_WINTER:
                    if (GameObject* platform = instance->GetGameObject(ArthasPlatformGUID))
                    {
                        platform->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        Events.ScheduleEvent(EVENT_REBUILD_PLATFORM, 1500ms);
                    }
                    break;
                case EVENT_TELEPORT_TO_FROSMOURNE: // Harvest Soul (normal mode)
                    if (Creature* terenas = instance->SummonCreature(NPC_TERENAS_MENETHIL_FROSTMOURNE, TerenasSpawn, nullptr, 65000))
                    {
                        terenas->AI()->DoAction(ACTION_FROSTMOURNE_INTRO);
                        std::list<Creature*> triggers;
                        GetCreatureListWithEntryInGrid(triggers, terenas, NPC_WORLD_TRIGGER_INFINITE_AOI, 100.0f);
                        if (!triggers.empty())
                        {
                            triggers.sort(Acore::ObjectDistanceOrderPred(terenas, false));
                            Unit* visual = triggers.front();
                            visual->CastSpell(visual, SPELL_FROSTMOURNE_TELEPORT_VISUAL, true);
                        }

                        if (Creature* warden = instance->SummonCreature(NPC_SPIRIT_WARDEN, SpiritWardenSpawn, nullptr, 65000))
                        {
                            terenas->AI()->AttackStart(warden);
                            warden->AddThreat(terenas, 300000.0f);
                        }
                    }
                    break;
                case EVENT_FESTERGUT_VALVE_USED:
                    if (!(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE))
                    {
                        if (GameObject* goGas = instance->GetGameObject(GasReleaseValveGUID))
                            goGas->SetGameObjectFlag(GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                        PutricideEventProgress |= PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE;
                        if (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE)
                        {
                            HandleGameObject(PutricideCollisionGUID, true);
                            for (uint8 i = 0; i < 2; ++i)
                                if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[i]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        }
                        else
                            HandleGameObject(PutricideGateGUIDs[0], false);
                        HandleGameObject(PutricidePipeGUIDs[0], true);
                        SaveToDB();
                    }
                    break;
                case EVENT_ROTFACE_VALVE_USED:
                    if (!(PutricideEventProgress & PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE))
                    {
                        if (GameObject* goOoze = instance->GetGameObject(OozeReleaseValveGUID))
                            goOoze->SetGameObjectFlag(GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                        PutricideEventProgress |= PUTRICIDE_EVENT_FLAG_ROTFACE_VALVE;
                        if (PutricideEventProgress & PUTRICIDE_EVENT_FLAG_FESTERGUT_VALVE)
                        {
                            HandleGameObject(PutricideCollisionGUID, true);
                            for (uint8 i = 0; i < 2; ++i)
                                if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[i]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        }
                        else
                            HandleGameObject(PutricideGateGUIDs[1], false);
                        HandleGameObject(PutricidePipeGUIDs[1], true);
                        SaveToDB();
                    }
                    break;
            }
        }

        void SetPositionTraps(GameObject* go)
        {
            std::vector<Position> trapPositions;

            switch (go->GetEntry())
            {
                case GO_SPIRIT_ALARM_1:
                    trapPositions = GoSpiritAlarm_1;
                    break;
                case GO_SPIRIT_ALARM_2:
                    trapPositions = GoSpiritAlarm_2;
                    break;
                case GO_SPIRIT_ALARM_3:
                    trapPositions = GoSpiritAlarm_3;
                    break;
                case GO_SPIRIT_ALARM_4:
                    trapPositions = GoSpiritAlarm_4;
                    break;
                default:
                    return;
            }

            go->Relocate(Acore::Containers::SelectRandomContainerElement(trapPositions));
        }

    protected:
        // pussywizard:
        bool IsBuffAvailable;
        uint32 WeeklyQuestId10; // contains id from 10man for any difficulty (for simplicity)
        ObjectGuid WeeklyQuestNpcGUID[WeeklyNPCs];
        ObjectGuid PutricideEnteranceDoorGUID;
        uint32 PutricideEventProgress;
        ObjectGuid GasReleaseValveGUID;
        ObjectGuid OozeReleaseValveGUID;
        bool LichKingHeroicAvailable;
        uint32 LichKingRandomWhisperTimer;
        uint32 DarkwhisperElevatorTimer;
        ObjectGuid ScourgeTransporterFirstGUID;

        EventMap Events;
        ObjectGuid LadyDeathwhisperGUID;
        ObjectGuid LadyDeathwisperElevatorGUID;
        ObjectGuid GunshipGUID;
        ObjectGuid EnemyGunshipGUID;
        ObjectGuid GunshipArmoryGUID;
        ObjectGuid DeathbringerSaurfangGUID;
        ObjectGuid DeathbringerSaurfangDoorGUID;
        ObjectGuid DeathbringerSaurfangEventGUID;   // Muradin Bronzebeard or High Overlord Saurfang
        ObjectGuid DeathbringersCacheGUID;
        ObjectGuid SaurfangTeleportGUID;
        ObjectGuid PlagueSigilGUID;
        ObjectGuid BloodwingSigilGUID;
        ObjectGuid FrostwingSigilGUID;
        ObjectGuid PutricidePipeGUIDs[2];
        ObjectGuid PutricideGateGUIDs[2];
        ObjectGuid PutricideCollisionGUID;
        ObjectGuid FestergutGUID;
        ObjectGuid RotfaceGUID;
        ObjectGuid ProfessorPutricideGUID;
        ObjectGuid PutricideTableGUID;
        ObjectGuid BloodCouncilGUIDs[3];
        ObjectGuid BloodCouncilControllerGUID;
        ObjectGuid BloodQueenLanaThelGUID;
        ObjectGuid CrokScourgebaneGUID;
        ObjectGuid CrokCaptainGUIDs[4];
        ObjectGuid SisterSvalnaGUID;
        ObjectGuid ValithriaDreamwalkerGUID;
        ObjectGuid ValithriaLichKingGUID;
        ObjectGuid ValithriaTriggerGUID;
        ObjectGuid PutricadeTrapGUID;
        ObjectGuid SindragosaGauntletGUID;
        ObjectGuid SindragosaGUID;
        ObjectGuid SpinestalkerGUID;
        ObjectGuid RimefangGUID;
        ObjectGuid TheLichKingTeleportGUID;
        ObjectGuid TheLichKingGUID;
        ObjectGuid TheLichKingLhGUID;
        ObjectGuid HighlordTirionFordringGUID;
        ObjectGuid TerenasMenethilGUID;
        ObjectGuid ArthasPlatformGUID;
        ObjectGuid ArthasPrecipiceGUID;
        ObjectGuid FrozenThroneEdgeGUID;
        ObjectGuid FrozenThroneWindGUID;
        ObjectGuid FrozenThroneWarningGUID;
        ObjectGuid FrozenBolvarGUID;
        ObjectGuid PillarsChainedGUID;
        ObjectGuid PillarsUnchainedGUID;
        uint32 ColdflameJetsState;
        std::set<ObjectGuid::LowType> FrostwyrmGUIDs;
        std::set<ObjectGuid::LowType> SpinestalkerTrash;
        std::set<ObjectGuid::LowType> RimefangTrash;
        uint32 BloodQuickeningState;
        uint32 HeroicAttempts;
        uint16 BloodQuickeningMinutes;
        uint32 BloodPrinceTrashCount;
        bool IsBonedEligible;
        bool IsOozeDanceEligible;
        bool IsNauseaEligible;
        bool IsOrbWhispererEligible;
        bool IsSindragosaIntroDone;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_icecrown_citadel_InstanceMapScript(map);
    }
};

void AddSC_instance_icecrown_citadel()
{
    new instance_icecrown_citadel();
}

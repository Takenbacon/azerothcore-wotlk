-- DB update 2024_06_10_02 -> 2024_06_11_00
--
UPDATE `spell_script_names` SET `ScriptName`='spell_algalon_collapse_aura' WHERE `spell_id`=62018;
UPDATE `spell_script_names` SET `ScriptName`='spell_algalon_phase_punch_aura' WHERE `spell_id`=64412;
UPDATE `spell_script_names` SET `ScriptName`='spell_algalon_remove_phase_aura' WHERE `spell_id`=64445;
UPDATE `spell_script_names` SET `ScriptName`='spell_assembly_rune_of_summoning_aura' WHERE `spell_id`=62019;
UPDATE `spell_script_names` SET `ScriptName`='spell_aura_of_despair_aura' WHERE `spell_id`=62692;
UPDATE `spell_script_names` SET `ScriptName`='spell_ignis_scorch_aura' WHERE `spell_id` IN (62546, 63474);
UPDATE `spell_script_names` SET `ScriptName`='spell_ignis_slag_pot_aura' WHERE `spell_id` IN (62717, 63477);
UPDATE `spell_script_names` SET `ScriptName`='spell_load_into_catapult_aura' WHERE `spell_id`=64414;
UPDATE `spell_script_names` SET `ScriptName`='spell_mark_of_the_faceless_periodic_aura' WHERE `spell_id`=63276;
UPDATE `spell_script_names` SET `ScriptName`='spell_mimiron_p3wx2_laser_barrage_aura' WHERE `spell_id`=63274;
UPDATE `spell_script_names` SET `ScriptName`='spell_mimiron_rapid_burst_aura' WHERE `spell_id`=63382;
UPDATE `spell_script_names` SET `ScriptName`='spell_orbital_supports_aura' WHERE `spell_id` IN (64482, 65075, 65076, 65077);
UPDATE `spell_script_names` SET `ScriptName`='spell_saronite_vapors_dummy_aura' WHERE `spell_id`=63322;
UPDATE `spell_script_names` SET `ScriptName`='spell_shield_generator_aura' WHERE `spell_id`=64677;
UPDATE `spell_script_names` SET `ScriptName`='spell_shield_of_runes_aura' WHERE `spell_id` IN (62274, 63489);
UPDATE `spell_script_names` SET `ScriptName`='spell_systems_shutdown_aura' WHERE `spell_id`=62475;
UPDATE `spell_script_names` SET `ScriptName`='spell_tar_blaze_aura' WHERE `spell_id`=62292;
UPDATE `spell_script_names` SET `ScriptName`='spell_thorim_lightning_pillar_P2_aura' WHERE `spell_id`=62976;
UPDATE `spell_script_names` SET `ScriptName`='spell_thorim_trash_impale_aura' WHERE `spell_id` IN (62331, 62418);
UPDATE `spell_script_names` SET `ScriptName`='spell_transitus_shield_beam_aura' WHERE `spell_id`=48310;
UPDATE `spell_script_names` SET `ScriptName`='spell_ulduar_arachnopod_damaged_aura' WHERE `spell_id`=64770;
UPDATE `spell_script_names` SET `ScriptName`='spell_ulduar_energy_sap_aura' WHERE `spell_id` IN (64740, 64876);
UPDATE `spell_script_names` SET `ScriptName`='spell_ulduar_stone_grip_aura' WHERE `spell_id` IN (62056, 63985);
UPDATE `spell_script_names` SET `ScriptName`='spell_vehicle_circuit_overload_aura' WHERE `spell_id`=62399;
UPDATE `spell_script_names` SET `ScriptName`='spell_xt002_gravity_bomb' WHERE `spell_id` IN (63024, 64234);
UPDATE `spell_script_names` SET `ScriptName`='spell_yogg_saron_empowered_aura' WHERE `spell_id`=64161;
UPDATE `spell_script_names` SET `ScriptName`='spell_yogg_saron_grim_reprisal_aura' WHERE `spell_id`=63305;
UPDATE `spell_script_names` SET `ScriptName`='spell_yogg_saron_insane_aura' WHERE `spell_id`=63120;
UPDATE `spell_script_names` SET `ScriptName`='spell_yogg_saron_malady_of_the_mind_aura' WHERE `spell_id` IN (63830, 63881) AND `ScriptName` = 'spell_yogg_saron_malady_of_the_mind';
UPDATE `spell_script_names` SET `ScriptName`='spell_yogg_saron_protective_gaze_aura' WHERE `spell_id`=64174 AND `ScriptName` = 'spell_yogg_saron_protective_gaze';
UPDATE `spell_script_names` SET `ScriptName`='spell_yogg_saron_sanity_well_aura' WHERE `spell_id`=64169;
UPDATE `spell_script_names` SET `ScriptName`='spell_yogg_saron_shadow_beacon_aura' WHERE `spell_id`=64465;
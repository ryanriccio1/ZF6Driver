#pragma once

enum Gear
{
    PARK = 0,
    REVERSE,
    NEUTRAL,
    FIRST,
    SECOND,
    THIRD,
    FOURTH,
    FIFTH,
    SIXTH
};

enum Solenoid
{
    SELECTION_VALVE = 0,
    CLUTCH_A,
    CLUTCH_B,
    CLUTCH_C,
    BRAKE_D_CLUTCH_E
};

struct SolenoidConfig
{
    bool selection_valve;
    bool clutch_a;
    bool clutch_b;
    bool clutch_c;
    bool brake_d_clutch_e;
};

const SolenoidConfig solenoid_config[] = 
{
    // SV     A      B      C      DE
    {false, false, false, false,  true}, // PARK
    {false, false,  true, false,  true}, // REVERSE
    {false, false, false, false,  true}, // NEUTRAL
    {false,  true, false, false,  true}, // FIRST
    {false,  true, false,  true, false}, // SECOND
    {false,  true,  true, false, false}, // THIRD
    { true,  true, false, false,  true}, // FOURTH
    { true, false,  true, false,  true}, // FIFTH
    { true, false, false,  true,  true}  // SIXTH
};

// if engaged is false, it looks for incoming gears, if engaged is true, outgoing gears
SolenoidConfig get_solenoid_change(Gear current_gear, Gear new_gear, bool engaged = false)
{
    SolenoidConfig changing_solenoids = {false, false, false, false, false};
    SolenoidConfig current_conf = solenoid_config[current_gear];
    SolenoidConfig new_conf = solenoid_config[new_gear];

    // if current clutch is engaged, but changes on gear shift, mark as outgoing
    if (current_conf.selection_valve == engaged &&
        current_conf.selection_valve != new_conf.selection_valve)
        changing_solenoids.selection_valve = true;

    if (current_conf.clutch_a == engaged &&
        current_conf.clutch_a != new_conf.clutch_a)
        changing_solenoids.clutch_a = true;

    if (current_conf.clutch_b == engaged &&
        current_conf.clutch_b != new_conf.clutch_b)
        changing_solenoids.clutch_b = true;

    if (current_conf.clutch_c == engaged &&
        current_conf.clutch_c != new_conf.clutch_c)
        changing_solenoids.clutch_c = true;

    if (current_conf.brake_d_clutch_e == engaged &&
        current_conf.brake_d_clutch_e != new_conf.brake_d_clutch_e)
        changing_solenoids.brake_d_clutch_e = true;

    return changing_solenoids;
}

SolenoidConfig get_incoming_solenoids(Gear current_gear, Gear new_gear)
{
    return get_solenoid_change(current_gear, new_gear, false);
}

SolenoidConfig get_outgoing_solenoids(Gear current_gear, Gear new_gear)
{
    return get_solenoid_change(current_gear, new_gear, true);
}










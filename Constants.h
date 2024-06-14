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

SolenoidConfig get_incoming_solenoids(Gear current_gear, Gear new_gear)
{
    SolenoidConfig incoming_solenoids = {false, false, false, false, false};
    SolenoidConfig current_conf = solenoid_config[current_gear];
    SolenoidConfig new_conf = solenoid_config[new_gear];

    // if current clutch is disengaged, but changes on gear shift, mark as incoming
    if (current_conf.selection_valve == false &&
        current_conf.selection_valve != new_conf.selection_valve)
        incoming_solenoids.selection_valve = true;

    if (current_conf.clutch_a == false &&
        current_conf.clutch_a != new_conf.clutch_a)
        incoming_solenoids.clutch_a = true;

    if (current_conf.clutch_b == false &&
        current_conf.clutch_b != new_conf.clutch_b)
        incoming_solenoids.clutch_b = true;

    if (current_conf.clutch_c == false &&
        current_conf.clutch_c != new_conf.clutch_c)
        incoming_solenoids.clutch_c = true;

    if (current_conf.brake_d_clutch_e == false &&
        current_conf.brake_d_clutch_e != new_conf.brake_d_clutch_e)
        incoming_solenoids.brake_d_clutch_e = true;

    return incoming_solenoids;
}

SolenoidConfig get_outgoing_solenoids(Gear current_gear, Gear new_gear)
{
    SolenoidConfig outgoing_solenoids = {false, false, false, false, false};
    SolenoidConfig current_conf = solenoid_config[current_gear];
    SolenoidConfig new_conf = solenoid_config[new_gear];

    // if current clutch is engaged, but changes on gear shift, mark as outgoing
    if (current_conf.selection_valve == true &&
        current_conf.selection_valve != new_conf.selection_valve)
        outgoing_solenoids.selection_valve = true;

    if (current_conf.clutch_a == true &&
        current_conf.clutch_a != new_conf.clutch_a)
        outgoing_solenoids.clutch_a = true;

    if (current_conf.clutch_b == true &&
        current_conf.clutch_b != new_conf.clutch_b)
        outgoing_solenoids.clutch_b = true;

    if (current_conf.clutch_c == true &&
        current_conf.clutch_c != new_conf.clutch_c)
        outgoing_solenoids.clutch_c = true;

    if (current_conf.brake_d_clutch_e == true &&
        current_conf.brake_d_clutch_e != new_conf.brake_d_clutch_e)
        outgoing_solenoids.brake_d_clutch_e = true;

    return outgoing_solenoids;
}








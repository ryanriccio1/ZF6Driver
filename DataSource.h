#pragma once
#include <stdint.h>

class IDataSource
{
public:
    virtual void update() = 0;
    virtual uint16_t get_engine_demand() const = 0;
    virtual uint16_t get_vehicle_speed() const = 0;
    virtual uint16_t get_engine_rpm() const = 0;
    virtual uint16_t get_manifold_pressure() const = 0;
    virtual uint16_t get_throttle_position() const = 0;
    virtual uint16_t get_air_temperature() const = 0;
};
#pragma once
#include "DataSource.h"

class HTCANSource : public IDataSource
{
public:
    void update();
    uint16_t get_engine_demand() const override;
    uint16_t get_vehicle_speed() const override;
    uint16_t get_engine_rpm() const override;
    uint16_t get_manifold_pressure() const override;
    uint16_t get_throttle_position() const override;
    uint16_t get_air_temperature() const override;
};

void HTCANSource::update()
{
}

uint16_t HTCANSource::get_engine_demand() const
{
}

uint16_t HTCANSource::get_vehicle_speed() const
{
}

uint16_t HTCANSource::get_engine_rpm() const
{
}

uint16_t HTCANSource::get_manifold_pressure() const
{
}

uint16_t HTCANSource::get_throttle_position() const
{
}

uint16_t HTCANSource::get_air_temperature() const
{
}